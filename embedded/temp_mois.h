#ifndef TEMP_MOIS_H
#define TEMP_MOIS_H

#include <stdint.h>

extern volatile uint8_t temperature;
extern volatile uint8_t humidity;

void set_pin_output(void);
void set_pin_input(void);
uint8_t read_pin(void);
void write_pin(uint8_t value);
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
uint8_t DHT11_Read(uint8_t *humidity, uint8_t *temperature);
int temp_mois_main(void);
#endif
