/*
File: Lab_6_JHB.c
Original Author: Josh Brake
Email: jbrake@hmc.edu
Date: 9/14/19

Modified by: Josaphat Ngoga
Email: jngoga@g.hmc.edu
Date: 17/10/2025
Changes:*Added form features for the web to display LED status and temperature reading
        from the DS1722 temperature sensor via SPI communication.
        *Added SPI initialization code for communication with the DS1722 sensor.
        *Added code to read temperature data from the DS1722 sensor over SPI.
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "main.h"

/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////

//Defining the web page in two chunks: everything before the current time, and everything after the current time
char* webpageStart = "<!DOCTYPE html><html><head><title>E155 Web Server Demo Webpage</title>\
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
	</head>\
	<body><h1>E155 Web Server Demo Webpage</h1>";
char* ledStr = "<p>LED Control:</p><form action=\"ledon\"><input type=\"submit\" value=\"Turn the LED on!\"></form>\
	<form action=\"ledoff\"><input type=\"submit\" value=\"Turn the LED off!\"></form>";

char* tempPageForm = "<p>Measured Ambient Temperature:</p>\
    <form action=\"8bit\"><input type=\"submit\" value=\"8 Bit Resolution\"></form>\
    <form action=\"9bit\"><input type=\"submit\" value=\"9 Bit Resolution\"></form>\
    <form action=\"10bit\"><input type=\"submit\" value=\"10 Bit Resolution\"></form>\
    <form action=\"11bit\"><input type=\"submit\" value=\"11 Bit Resolution\"></form>\
    <form action=\"12bit\"><input type=\"submit\" value=\"12 Bit Resolution\"></form>";

char* webpageEnd   = "</body></html>";

//determines whether a given character sequence is in a char array request, returning 1 if present, -1 if not present
int inString(char request[], char des[]) {
	if (strstr(request, des) != NULL) {return 1;}
	return -1;
}

bool updateLEDStatus(char request[], bool currLED)
{
	// The request has been received. now process to determine whether to turn the LED on or off
	if (inString(request, "ledoff")==1) {
		digitalWrite(LED_PIN, PIO_LOW);
		currLED = 0;
	}
	else if (inString(request, "ledon")==1) {
		digitalWrite(LED_PIN, PIO_HIGH);
		currLED = 1;
	}

	return currLED;
}

/////////////////////////////////////////////////////////////////
// Solution Functions
/////////////////////////////////////////////////////////////////

// Decode temperature from SPI bits to float value
float readTemperatureSPI(uint8_t msb, uint8_t lsb){
    float temperature = (msb & 0b01111111);

    // Check if temperature is negative
    if ((msb >> 7) & 1) {temperature -= 128;} 

    // Apply resolution based on LSB
    float resolution = (float)((lsb >> 4) & 0x0F) * 0.0625;

    return temperature + resolution;
};

// Read temperature from DS1722 via SPI based on requested resolution
float updateTemperature(char request[]) {
    if (inString(request, "8bit") == 1) {
        digitalWrite(CS, PIO_HIGH);
        spiSendReceive(0x80);
        spiSendReceive(0b11100000);
        digitalWrite(CS, PIO_LOW);
    } else if (inString(request, "9bit") == 1) {
        digitalWrite(CS, PIO_HIGH);
        spiSendReceive(0x80);
        spiSendReceive(0b11100010);
        digitalWrite(CS, PIO_LOW);
    } else if (inString(request, "10bit") == 1) {
        digitalWrite(CS, PIO_HIGH);
        spiSendReceive(0x80);
        spiSendReceive(0b11100100);
        digitalWrite(CS, PIO_LOW);
    } else if (inString(request, "11bit") == 1) {
        digitalWrite(CS, PIO_HIGH);
        spiSendReceive(0x80);
        spiSendReceive(0b11100110);
        digitalWrite(CS, PIO_LOW);
    } else if (inString(request, "12bit") == 1) {
        digitalWrite(CS, PIO_HIGH);
        spiSendReceive(0x80);
        spiSendReceive(0b11101000);
        digitalWrite(CS, PIO_LOW);
    }
    // Read temperature bits
    digitalWrite(CS, PIO_HIGH);
    spiSendReceive(0x01);
    uint8_t lsb = spiSendReceive(0x00);
    digitalWrite(CS, PIO_LOW);

    digitalWrite(CS, PIO_HIGH);
    spiSendReceive(0x02);
    uint8_t msb = spiSendReceive(0x00);
    digitalWrite(CS, PIO_LOW);

    return readTemperatureSPI(msb, lsb); // return temperature as float
};

int main(void) {
    configureFlash();
    configureClock();

    gpioEnable(GPIO_PORT_A);
    gpioEnable(GPIO_PORT_B);
    gpioEnable(GPIO_PORT_C);

    pinMode(LED_PIN, GPIO_OUTPUT);
    
    RCC->APB2ENR |= (RCC_APB2ENR_TIM15EN);
    initTIM(TIM15);
    
    USART_TypeDef * USART = initUSART(USART1_ID, 125000);

    ////////////////////// SPI initialization/config code /////////////////////////////////////////////
    // Enable GPIO Pins
    pinMode(SCK, GPIO_ALT);
    pinMode(MOSI, GPIO_ALT);
    pinMode(MISO, GPIO_ALT);
    pinMode(CS, GPIO_OUTPUT);

    digitalWrite(CS, PIO_LOW);

    // Configure alternate function
    GPIOB->AFR[0] &= ~((0xF << GPIO_AFRL_AFSEL3_Pos) | // Clear bits
                       (0xF << GPIO_AFRL_AFSEL4_Pos) |
                       (0xF << GPIO_AFRL_AFSEL5_Pos));
    GPIOB->AFR[0] |=  (0x5 << GPIO_AFRL_AFSEL3_Pos) |   // PB3 SCK
                      (0x5 << GPIO_AFRL_AFSEL4_Pos) |   // PB4 MISO
                      (0x5 << GPIO_AFRL_AFSEL5_Pos);    // PB5 MOSI
    
    // Enable and configure MCU SPI1
    enableSPI(SPI1);
    initSPI(SPI1, 0b111, 0, 1); 

    // Configure DS1722 SPI connection
    digitalWrite(CS, PIO_HIGH);
    spiSendReceive(0x80);
    spiSendReceive(0xE8);
    digitalWrite(CS, PIO_LOW);

    while(1) {
        /* Wait for ESP8266 to send a request.
        Requests take the form of '/REQ:<tag>\n', with TAG begin <= 10 characters.
        Therefore the request[] array must be able to contain 18 characters.
        */
        
        // Receive web request from the ESP
        char request[BUFF_LEN] = "                  "; // initialize to known value
        int charIndex = 0;
        
        // Keep going until you get end of line character
        while(inString(request, "\n") == -1) {
          // Wait for a complete request to be transmitted before processing
          while(!(USART->ISR & USART_ISR_RXNE));
          request[charIndex++] = readChar(USART);
        }
      
        /////////////////////////////// SPI temperature reading code /////////////////////////////////////////
        // Record ambient temperature
        float ambTemp = updateTemperature(request);
        char tempReq [40];

        // sprintf(tempReq, "The ambient temperature is: %f&deg;C\n", ambTemp);
        sprintf(tempReq,"The ambient temperature is: %.4f&deg;C", ambTemp);
      
        // Update string with current LED state
      
        int led_status = updateLEDStatus(request, led_status);
      
        char ledStatusStr[20];
        if (led_status == 1)
          sprintf(ledStatusStr,"LED is on!");
        else if (led_status == 0)
          sprintf(ledStatusStr,"LED is off!");
      
        // finally, transmit the webpage over UART
        sendString(USART, webpageStart); // webpage header code
        sendString(USART, ledStr); // button for controlling LED
      
        sendString(USART, "<h2>LED Status</h2>");
        sendString(USART, "<p>");
        sendString(USART, ledStatusStr);
        sendString(USART, "</p>");

        sendString(USART, "<h2>Ambient Temperature</h2>");
        sendString(USART, tempPageForm);
        sendString(USART, "<p>");
        sendString(USART, tempReq);
        sendString(USART, "</p>");

        sendString(USART, webpageEnd);
    }
}