#include <MKL25Z4.h>
#include <string.h>
#include "LCD.h"
#include "MainFile.h"



#define LED_PIN 1
#define LED_PORT PTD
#define LED_GPIO GPIOD

volatile uint8_t uart_data_ready = 0;
volatile char uart_buffer[64];

static char buffer[64];
static uint8_t buffer_index = 0;

void UART0_init(uint32_t baud_rate);
void LED1_init(void);

void face_UART_process(void) {
    
		
		
		while (1){
	
    
    
			
			
			if (UART0->S1 & UART_S1_RDRF_MASK) {
        char received = UART0->D;
				
				
				
				
			
        if (received == '\n') {
					buffer[buffer_index] = '\0';
				
					strncpy(uart_buffer, buffer, buffer_index + 1);
					uart_data_ready = 1;
					
					buffer_index = 0;
					
					LED_GPIO->PSOR = (1 << LED_PIN);
					
					break;
				}
				
				else {
					if(buffer_index < sizeof(buffer) - 1){
						buffer[buffer_index++] = received;
					}
				}
				
				
    
     
    }

    
	}	
}

		



void LED1_init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;             // Port D clock enable
    PORTD->PCR[LED_PIN] = PORT_PCR_MUX(1);          // PTD1 as GPIO
    LED_GPIO->PDDR |= (1 << LED_PIN);               // Output
    LED_GPIO->PCOR = (1 << LED_PIN);                // Baslangiçta kapali
}

void UART0_init(uint32_t baud_rate) {
    uint16_t sbr;
    uint32_t sysclk = 20971520;

    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
    SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

    PORTA->PCR[1] = PORT_PCR_MUX(2);  // UART0_RX
    PORTA->PCR[2] = PORT_PCR_MUX(2);  // UART0_TX

    UART0->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

    sbr = (uint16_t)(sysclk / (16 * baud_rate));
    UART0->BDH = (sbr >> 8) & 0x1F;
    UART0->BDL = sbr & 0xFF;

    UART0->C1 = 0;
    UART0->C2 = UART_C2_TE_MASK | UART_C2_RE_MASK;
}