// STM32L432KC_SPI.c
// Josaphat Ngoga
// jngoga@g.hmc.edu
// 17/10/2025
// SPI functions for MCU

#include "STM32L432KC.h"
#include "STM32L432KC_SPI.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_RCC.h"

void initSPI(SPI_TypeDef* SPIx, int br, int cpol, int cpha) {
    // reset CR1 & CR2 registers
    SPIx->CR1 = 0;
    SPIx->CR2 = 0;

    // Configure baud, mode, polarity, and phase
    SPIx->CR1 |= _VAL2FLD(SPI_CR1_BR, br);
    SPIx->CR1 |= SPI_CR1_MSTR;
    SPIx->CR1 &= ~(SPI_CR1_LSBFIRST | SPI_CR1_CRCEN);
    SPIx->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol) | _VAL2FLD(SPI_CR1_CPHA, cpha);

    // Software CS
    SPIx->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);

    // Set data and SPI format
    SPIx->CR2 |= (7U << SPI_CR2_DS_Pos) | SPI_CR2_FRXTH;  // Set FIFO reception threshold
    SPIx->CR2 &= ~SPI_CR2_FRF;
    SPIx->CR2 |= SPI_CR2_SSOE;

    // Enable
    SPIx->CR1 |= SPI_CR1_SPE;
}

char spiSendReceive(char send) {
    // Wait until TXE flag is set 
    while(!(SPI1->SR & SPI_SR_TXE));

    *(volatile char *) (&SPI1->DR) = send; // Send data over SPI
    
    // Wait until RXNE flag is set (Data is received)
    while(!(SPI1->SR & SPI_SR_RXNE));
    
    char rec = (volatile char) SPI1->DR; // Read received data 
    return rec; // Return received data
}

void enableSPI(SPI_TypeDef* SPIx) {
    if (SPIx == SPI1) {
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    } else if (SPIx == SPI3) {
        RCC->APB1ENR1 |= RCC_APB1ENR1_SPI3EN;
    }
}