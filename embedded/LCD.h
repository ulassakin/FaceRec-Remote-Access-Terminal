#ifndef LCD_I2C_H
#define LCD_I2C_H

#include <stdint.h>

// I2C pinleri (eger baska dosyada kullanmak istersen)
#define I2C_SCL      8  // PTC8
#define I2C_SDA      9  // PTC9
#define LCD_ADDR     0x27

// Delay fonksiyonu
void Delay_ms(uint32_t ms);

// I2C kontrol fonksiyonlari
void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write(uint8_t data);

// LCD kontrol fonksiyonlari
void LCD_Send(uint8_t data, uint8_t mode);
void LCD_Cmd(uint8_t cmd);
void LCD_Data(uint8_t data);
void LCD_Init(void);
void LCD_Print(char* str);

uint8_t I2C_ReadAck(void);
uint8_t I2C_ReadNack(void);


#endif
