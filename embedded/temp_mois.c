#include <MKL25Z4.h>
#include <stdint.h>
#include "temp_mois.h"

#define DHT11_PORT     PORTB
#define DHT11_GPIO     PTB
#define DHT11_PIN      1

volatile uint8_t temperature = 0;
volatile uint8_t humidity = 0;

// GPIO yön kontrolü
void set_pin_output(void) {
    DHT11_PORT->PCR[DHT11_PIN] &= ~PORT_PCR_MUX_MASK;
    DHT11_PORT->PCR[DHT11_PIN] |= PORT_PCR_MUX(1);
    DHT11_GPIO->PDDR |= (1 << 1);
}

void set_pin_input(void) {
    DHT11_PORT->PCR[DHT11_PIN] &= ~PORT_PCR_MUX_MASK;
    DHT11_PORT->PCR[DHT11_PIN] |= PORT_PCR_MUX(1);
    DHT11_GPIO->PDDR &= ~(1 << 1);
}

uint8_t read_pin(void) {
    return (DHT11_GPIO->PDIR & (1 << 1)) ? 1 : 0;
}

void write_pin(uint8_t value) {
    if (value)
        DHT11_GPIO->PSOR = (1 << 1);
    else
        DHT11_GPIO->PCOR = (1 << 1);
}

// Delay fonksiyonlari (SysTick ile)
void Delay_us(uint32_t us) {
    SysTick->LOAD = (SystemCoreClock / 1000000 - 1);
    SysTick->VAL = 0;
    SysTick->CTRL = 5;
    for (uint32_t i = 0; i < us; i++) {
        while (!(SysTick->CTRL & (1 << 16)));
    }
    SysTick->CTRL = 0;
}

void Delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        Delay_us(1000);
    }
}

// DHT11 veri okuma
uint8_t DHT11_Read(uint8_t *humidity, uint8_t *temperature) {
    uint8_t data[5] = {0};
    uint32_t time;

    // Start sinyali gönder
    set_pin_output();
    write_pin(0);
    Delay_ms(20);
    write_pin(1);
    Delay_us(30);
    set_pin_input();

    // DHT11 cevabi
    time = 0;
    while (read_pin() && time++ < 100) Delay_us(1);
    if (time >= 100) return 1; // timeout

    time = 0;
    while (!read_pin() && time++ < 100) Delay_us(1);
    time = 0;
    while (read_pin() && time++ < 100) Delay_us(1);

    // 40 bit oku
    for (int i = 0; i < 40; i++) {
        while (!read_pin()); // LOW bekle

        Delay_us(30);
        if (read_pin()) {
            data[i / 8] |= (1 << (7 - (i % 8)));
            while (read_pin()); // HIGH bit sonu
        }
    }

    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum != data[4]) return 2;

    *humidity = data[0];
    *temperature = data[2];
    return 0;
}

int temp_mois_main(void){
		SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    DHT11_PORT->PCR[DHT11_PIN] = PORT_PCR_MUX(1);

    

		SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
		PORTD->PCR[0] |= PORT_PCR_MUX(1);
		PTD->PDDR |= (1 << 0);

	
    
        uint8_t status = DHT11_Read(&humidity, &temperature);

				
				
        if (status == 0) {
            
					
						for (int u = 0;u < 8;u++){
							
							/*
							uint8_t bit = (temperature >> u) & 0x01;
							
							if(bit){
								PTD->PSOR = (1 << 0);
							}
							
							else{
								PTD->PCOR = (1 << 0);
							}
							
							Delay_ms(500);
							*/
							
							
							
						}
        
				
				} 
				
				else {
            // Hata olustu (status = 1 timeout, 2 checksum hatasi)
            // Gerekirse hata durumunu baska bir degiskende saklayabilirsin
        }

        Delay_ms(2000); // 2 saniyede bir okuma
}




