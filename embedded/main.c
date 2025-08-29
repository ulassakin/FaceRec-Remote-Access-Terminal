#include <stdio.h>
#include <MKL25Z4.h>
#include <string.h>
#include "temp_mois.h"
#include "LCD.h"
#include "MainFile.h"
#include "IRremote.h"

typedef enum {
	STATE_LOCKED,
	STATE_WAIT_FOR_UART,
	STATE_PASSWORD,
	STATE_WRONG_PASS,
	STATE_MENU
} SystemState;

SystemState current_state = STATE_LOCKED;

int main(void){
	I2C_Init();
	LCD_Init();
	LED1_init();
	UART0_init(9600);
	
	LCD_Cmd(0x01);
	LCD_Cmd(0x80);
	LCD_Print("System Locked");
	face_UART_process();
	port_timer_init();
	temp_mois_main();
	MQ2_Init();
	MQ2_Read_Raw();
	MQ2_Read_PPM();
	face_UART_process();
	while (1) {
		
		switch (current_state) {

            case STATE_LOCKED:
                
									if (uart_data_ready) {
                    current_state = STATE_PASSWORD;
                    uart_data_ready = 0;
                    LCD_Cmd(0x01);
                    LCD_Print("Recognized");
                    Delay_ms(1000);
                    LCD_Cmd(0x01);
                    LCD_Print("Enter Password:");
                    LCD_Cmd(0xC0);
										Delay_ms(1000);
                }
                break;

            case STATE_PASSWORD:
							if (end_flag){
                if (is_ulas == 1) {
									LCD_Cmd(0x01);
									LCD_Cmd(0x80);
									LCD_Print("Access Granted");
									Delay_ms(1000);
									current_state = STATE_MENU;
									end_flag = 0;
								}
								else{
									current_state = STATE_WRONG_PASS;
									wrong = 1;
									end_flag  = 0;
								}
						}
						
								break;

            case STATE_WRONG_PASS:
                LCD_Cmd(0x01);
                LCD_Print("Wrong Password");
                Delay_ms(1000);
                LCD_Cmd(0x01);
                LCD_Print("Try Again");
                Delay_ms(1000);
                LCD_Cmd(0x01);
                LCD_Print("Enter Password:");
                LCD_Cmd(0xC0);
                current_state = STATE_PASSWORD;
                break;

            case STATE_MENU:
                
								LCD_Cmd(0x01);
								LCD_Cmd(0x80);
								LCD_Print("T: ");
								LCD_Data((temperature / 10) + '0');  // onluk basamak
								LCD_Data((temperature % 10) + '0');  // birlik basamak
								LCD_Print(" C ");
						
								LCD_Print("H: ");
								LCD_Data((humidity / 10) + '0');
								LCD_Data((humidity % 10) + '0');
								LCD_Print(" %");
						
								
						
								LCD_Cmd(0xC0);
								LCD_Print("Welcome, Ulas!");
						
								
								LCD_Cmd(0xD+-4);
								LCD_Print(uart_buffer);
								
								
								
								Delay_ms(10000); // Menü sabit kalir
								current_state = STATE_LOCKED;
								LCD_Cmd(0x01);
								LCD_Cmd(0x80);
								LCD_Print("System Locked");
								face_UART_process();
								
                break;

            default:
                current_state = STATE_LOCKED;
                break;
        }
	}
}