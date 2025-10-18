/**
    Main Header: Contains general defines and selected portions of CMSIS files
    @file main.h
    @author Josh Brake
    @version 1.0 10/7/2020

    Modified by: Josaphat Ngoga
    Email: jngoga@g.hmc.edu
    Date: 17/10/2025
    Changes:Added new definitions for SPI and USART communication pins.
*/

#ifndef MAIN_H
#define MAIN_H

#include "STM32L432KC.h"

#define LED_PIN PB3 // LED pin for blinking on Port B pin 3
#define BUFF_LEN 32
#define SCK PA5
#define MOSI PA12
#define MISO PA6
#define CS PB1
#define USART_RX PA10
#define USART_TX PA9

#endif // MAIN_H