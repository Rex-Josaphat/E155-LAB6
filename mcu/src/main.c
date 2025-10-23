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
        *Added code to read temperature data (float) from the DS1722 sensor over SPI.
        *Modified webpage display format to implement HTML5 and styling
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "main.h"


//////////////////////////////// Web Sever Webpage///////////////////////////////////////////
char* webpageStart =
"<!DOCTYPE html><html><head><meta charset=\"utf-8\">"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
"<title>E155 Web Server Demo</title>"
// CSS
"<style>"
"  :root{--bg:#f6f7fb;--card:#ffffff;--text:#0f172a;--muted:#64748b;"
"        --accent:#2563eb;--accent2:#0ea5e9;--ring:rgba(37,99,235,.25);}"
"  *{box-sizing:border-box} body{margin:0;font:16px/1.5 system-ui,-apple-system,Segoe UI,Roboto,Ubuntu,Arial,sans-serif;color:var(--text);background:var(--bg);}"
"  .wrap{max-width:720px;margin:32px auto;padding:0 16px}"
"  .card{background:var(--card);border-radius:16px;padding:24px 20px;box-shadow:0 10px 30px rgba(0,0,0,.06);} "
"  h1{font-size:32px;margin:0 0 8px}"
"  h2{font-size:22px;margin:24px 0 8px}"
"  p{margin:8px 0;color:var(--muted)}"
"  .row{display:flex;flex-wrap:wrap;gap:10px;margin:10px 0 6px}"
"  .btn{appearance:none;border:0;border-radius:10px;padding:10px 14px;font-weight:600;cursor:pointer;}"
"  .btn-primary{background:linear-gradient(135deg,var(--accent),var(--accent2));color:#fff;}"
"  .btn-secondary{background:#e5e7eb;color:#111827}"
"  .btn:focus,.btn:hover{outline:0;box-shadow:0 0 0 4px var(--ring)}"
"  .stat{font-size:18px;color:var(--text);margin-top:6px}"
"  .footer{margin-top:18px;font-size:12px;color:var(--muted)}"
"</style>"
// Webpage Content 
"</head><body><div class=\"wrap\"><div class=\"card\">"
"<h1>E155 Web Server Webpage</h1>"
"<p>Onboard LED Control and read temperature from DS1722 sensor.</p>";

// LED status dispaly and controls
char* ledStr =
"<h2>LED Control</h2>"
"<div class=\"row\">"
"  <form action=\"ledon\"><input class=\"btn btn-primary\" type=\"submit\" value=\"Turn LED On\"></form>"
"  <form action=\"ledoff\"><input class=\"btn btn-secondary\" type=\"submit\" value=\"Turn LED Off\"></form>"
"</div>";

/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////

// Temperature display and controls
char* tempPageForm =
"<h2>Ambient Temperature</h2>"
"<p>Measured Ambient Temperature:</p>"
"<div class=\"row\">"
"  <form action=\"8bit\"><input class=\"btn btn-secondary\" type=\"submit\" value=\"8 Bit\"></form>"
"  <form action=\"9bit\"><input class=\"btn btn-secondary\" type=\"submit\" value=\"9 Bit\"></form>"
"  <form action=\"10bit\"><input class=\"btn btn-secondary\" type=\"submit\" value=\"10 Bit\"></form>"
"  <form action=\"11bit\"><input class=\"btn btn-secondary\" type=\"submit\" value=\"11 Bit\"></form>"
"  <form action=\"12bit\"><input class=\"btn btn-secondary\" type=\"submit\" value=\"12 Bit\"></form>"
"</div>";

// Closing tag
char* webpageEnd =
"<div class=\"footer\">Modified from original demo webpage by Josaphat Ngoga.</div>"
"</div></div></body></html>";

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

        sprintf(tempReq, "The ambient temperature is: %.4f&deg;C", ambTemp);
      
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
      
        sendString(USART, "<h2>LED Status</h2><p class=\"stat\">");
        sendString(USART, ledStatusStr);
        sendString(USART, "</p>");

        sendString(USART, tempPageForm);
        sendString(USART, "<p class=\"stat\">");
        sendString(USART, tempReq);
        sendString(USART, "</p>");

        sendString(USART, webpageEnd);
    }
}