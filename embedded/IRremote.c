#include <MKL25Z4.h>
#include <stdio.h>

volatile uint32_t buffer = 0;
volatile uint8_t bit_count = 0;
volatile uint32_t current_time = 0;
volatile uint32_t previous_time = 0;
volatile uint32_t delta = 0;

volatile int index = 3;
volatile uint8_t can_pass[4] = {0};

volatile int is_ulas = 0;
volatile int is_joe = 0;

volatile int end_flag = 0;
volatile int wrong = 1;
volatile uint8_t ulas_pass[4] = {0xC2,0xA8,0xA8,0xA2};// og: A2 A8 A8 C2
volatile uint8_t joe_pass[4] = {0x62,0x22,0x90,0xA2};// og: A2 90 22 62

#define NEC_START_MIN 11000
#define NEC_ZERO_MIN 1300
#define NEC_ZERO_MAX 1800
#define NEC_ONE_MIN 2200
#define NEC_ONE_MAX 3300

void port_timer_init(void);
void TPM1_IRQHandler(void);
void delay(void);
void LED_init(void);
void delay1(void);

void delay(void) {
    for (volatile int i = 0; i < 1000000; i++) {
        __asm("NOP");
    }
}

void delay1(void) {
    for (volatile int i = 0; i < 50000; i++) {
        __asm("NOP");
    }
}

void LED_init(void){
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[5] |= PORT_PCR_MUX(1);
	PTD->PDDR |= (1 << 5);
	PTD->PCOR = (1 << 5);
}

void port_timer_init(void){
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	PORTA->PCR[13] |= PORT_PCR_MUX(3);//Use alt3 for TPM1_CH1
	
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	TPM1->SC = 0;
	TPM1->MOD = 0xFFFF; // Just for safety
	TPM1->CONTROLS[1].CnSC = TPM_CnSC_ELSB_MASK | TPM_CnSC_CHIE_MASK;
	
	TPM1->SC |= TPM_SC_PS(4) | TPM_SC_CMOD(1);
	
	NVIC_ClearPendingIRQ(TPM1_IRQn);
	NVIC_EnableIRQ(TPM1_IRQn);
	
}

void TPM1_IRQHandler(void){
	TPM1->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;// Clear the channel flag

	if (index >= 0){
		current_time = TPM1->CONTROLS[1].CnV;
		delta = (current_time - previous_time) & 0xFFFF; // avoiding overflow calculation issues
		previous_time = current_time;
		
		if(bit_count < 32){
			if((delta < NEC_ZERO_MAX) && (delta > NEC_ZERO_MIN)){
				buffer <<= 1;
				bit_count += 1;
			}
			else if ((delta > NEC_ONE_MIN) && (delta < NEC_ONE_MAX)){
				buffer <<= 1;
				buffer |= 1;
				bit_count += 1;
			}
		}
	
		else if (bit_count == 32){
			uint8_t button_code = (buffer >> 8) & 0xFF;//mask LSB 8bits and get 8 bits
			can_pass[index] = button_code;
			index -= 1;
			
	
			/*
			
			for (int i = 7; i>=0;i--){
				if ((button_code >> i) & 1){
					PTD->PSOR = (1 << 5);
				}
				else{
					PTD->PCOR = (1 << 5);
				}
				delay();
			}
			
			*/
    
			bit_count = 0;
			buffer = 0;
			
}		
	


}
	else if (index == -1){
/*		
		for (int i = 3; i >= 0; i--) {
				uint8_t code = can_pass[i];
				for (int j = 7; j >= 0; j--) {
					if ((code >> j) & 1)
            PTD->PSOR = (1 << 5);
					else
            PTD->PCOR = (1 << 5);
					delay();
				}
			}
		
		for (int k = 3; k >= 0; k--) {
				uint8_t code = ulas_pass[k];
				for (int b = 7; b >= 0; b--) {
					if ((code >> b) & 1)
            PTD->PSOR = (1 << 5);
					else
            PTD->PCOR = (1 << 5);
					delay();
				}
			}
	*/		
		is_ulas = 1;
		is_joe = 1;
		
		for (int a = 3;a >= 0;a--){
			if(can_pass[a] != ulas_pass[a]){
				is_ulas = 0;
				wrong = 0;
			}
			
			if(can_pass[a] != joe_pass[a]){
				is_joe = 0;
			}
		}
		
		if (is_ulas){
			PTD->PSOR = (1 << 5);
			delay1();
			PTD->PCOR = (1 << 5);
		}
		
		else if (is_joe){
			PTD->PSOR = (1 << 5);
			delay1();
			PTD->PCOR = (1 << 5);
			delay1();
			PTD->PSOR = (1 << 5);
			delay1();
			PTD->PCOR = (1 << 5);
		}
		else{
			PTD->PSOR = (1 << 5);
			
		}
		
		index = 3;
		
		end_flag = 1;
	}
	
}

int Ir_main(){
	LED_init();
	port_timer_init();
	
	while(1){
		
	}
}

