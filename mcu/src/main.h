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

#include "D:\MicroPs\E155-Lab6\mcu\src\lib\STM32L432KC.h"

#define LED_PIN PA6 // LED pin for blinking on Port B pin 3
#define BUFF_LEN 32
#define SCK PB3
#define MOSI PB5
#define MISO PB4
#define CS PA11
#define USART_RX PA10
#define USART_TX PA9

#endif // MAIN_H