// STM32L432KC_SPI.c
// Josaphat Ngoga
// jngoga@g.hmc.edu
// 17/10/2025
// SPI functions for MCU

#include "STM32L432KC_SPI.h"

void initSPI(SPI_TypeDef* SPIx, int br, int cpol, int cpha) {
    // reset CR1 & CR2 registers
    SPIx->CR1 = 0;
    SPIx->CR2 = 0;

    // Configure baud rate
    SPIx->CR1 &= ~SPI_CR1_BR_Msk; 
    SPIx->CR1 |= _VAL2FLD(SPI_CR1_BR, br);

    // Configure cpol and cpha
    SPIx->CR1 &= ~(SPI_CR1_CPOL_Msk | SPI_CR1_CPHA_Msk);
    SPIx->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol);
    SPIx->CR1 |= _VAL2FLD(SPI_CR1_CPHA, cpha);

    // SPI behaviour configuration
    SPIx->CR1 |= SPI_CR1_MSTR; // Set MCU SPI as Master

    SPIx->CR1 &= ~SPI_CR1_LSBFIRST; // bit order into the DS1722 is MSB first
    SPIx->CR1 &= ~SPI_CR1_CRCEN;

    // Software CS
    SPIx->CR1 |= SPI_CR1_SSM;
    SPIx->CR1 |= SPI_CR1_SSI;

    // Set data and SPI format
    SPIx->CR2 |= (7 << SPI_CR2_DS_Pos);
    SPIx->CR2 |= SPI_CR2_SSOE;
    SPIx->CR2 &= ~SPI_CR2_FRF;
    SPIx->CR2 |= SPI_CR2_FRXTH; // Set FIFO reception threshold
    
    // SPI enable
    SPIx->CR1 &= ~SPI_CR1_SPE;
    SPIx->CR1 |= SPI_CR1_SPE;
}   

char spiSendReceive(SPI_TypeDef* SPIx, char send) {
    // Wait until TXE flag is set
    while (!(SPIx->SR & SPI_SR_TXE));

    SPIx->DR = send; // Send data

    // Wait until RXNE flag is set
    while (!(SPIx->SR & SPI_SR_RXNE));

    return (char)(SPIx->DR); // Read and return received data
}

void enableSPI(SPI_TypeDef* SPIx) {
    if (SPIx == SPI1) {
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    } else if (SPIx == SPI3) {
        RCC->APB1ENR1 |= RCC_APB1ENR1_SPI3EN;
    }
}