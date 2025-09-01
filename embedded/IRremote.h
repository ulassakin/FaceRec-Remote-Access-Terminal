#ifndef IR_H
#define IR_H

#include <MKL25Z4.h>
#include <stdint.h>

// ==== NEC Timing Constants ====
#define NEC_START_MIN 11000
#define NEC_ZERO_MIN 1300
#define NEC_ZERO_MAX 1800
#define NEC_ONE_MIN 2200
#define NEC_ONE_MAX 3300

// ==== Global Degiskenler ====
extern volatile uint32_t buffer;
extern volatile uint8_t bit_count;
extern volatile uint32_t current_time;
extern volatile uint32_t previous_time;
extern volatile uint32_t delta;

extern volatile int index;
extern volatile uint8_t can_pass[4];
extern int end_flag;
extern volatile int is_ulas;
extern volatile int is_joe;
extern volatile int wrong;
extern volatile uint8_t ulas_pass[4];
extern volatile uint8_t joe_pass[4];

// ==== Fonksiyon Prototipleri ====
void port_timer_init(void);
void TPM1_IRQHandler(void);
void delay(void);
void delay1(void);
void LED_init(void);

#endif
