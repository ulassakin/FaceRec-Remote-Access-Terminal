#include <MKL25Z4.h>
//#include "temp_mois.h"
//#include "RTC.h"
#include "DS1302.h"

#define I2C_SCL      8  // PTC8
#define I2C_SDA      9  // PTC9
#define LCD_ADDR     0x27 

void Delay_ms(uint32_t ms);
void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write(uint8_t data);
void LCD_Send(uint8_t data, uint8_t mode);
void LCD_Cmd(uint8_t cmd);
void LCD_Data(uint8_t data);
void LCD_Init(void);
void LCD_Print(char* str);

/*
void Delay_ms(uint32_t ms) {
    SysTick->LOAD = (SystemCoreClock / 1000 - 1);
    SysTick->VAL = 0;
    SysTick->CTRL = 5;
    for (uint32_t i = 0; i < ms; i++) {
        while (!(SysTick->CTRL & (1 << 16)));
    }
    SysTick->CTRL = 0;
}
*/
void I2C_Init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    PORTC->PCR[I2C_SCL] = PORT_PCR_MUX(2); // SCL = PTC8
    PORTC->PCR[I2C_SDA] = PORT_PCR_MUX(2); // SDA = PTC9

    SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
    I2C0->F = 0x1F; // I2C frequency divider
    I2C0->C1 = I2C_C1_IICEN_MASK;
}

void I2C_Start(void) {
    I2C0->C1 |= I2C_C1_MST_MASK | I2C_C1_TX_MASK;
}

void I2C_Stop(void) {
    I2C0->C1 &= ~I2C_C1_MST_MASK;
}

void I2C_Write(uint8_t data) {
    I2C0->D = data;
    while (!(I2C0->S & I2C_S_IICIF_MASK));
    I2C0->S |= I2C_S_IICIF_MASK;
}

uint8_t I2C_WriteWithAck(uint8_t data) {
    I2C0->D = data;

    while (!(I2C0->S & I2C_S_IICIF_MASK));
    I2C0->S |= I2C_S_IICIF_MASK;

    return (I2C0->S & I2C_S_RXAK_MASK) ? 1 : 0;  // 1: NACK, 0: ACK
}


void LCD_Send(uint8_t data, uint8_t mode) {
    uint8_t upper = data & 0xF0;
    uint8_t lower = (data << 4) & 0xF0;
    uint8_t control = mode | 0x08; 

    I2C_Start();
    I2C_Write(LCD_ADDR << 1);

    uint8_t seq[] = {
        upper | control | 0x04, upper | control,  
        lower | control | 0x04, lower | control
    };

    for (int i = 0; i < 4; i++) {
        I2C_Write(seq[i]);
    }

    I2C_Stop();
    Delay_ms(2);
}

void LCD_Cmd(uint8_t cmd) {
    LCD_Send(cmd, 0x00);
}

void LCD_Data(uint8_t data) {
    LCD_Send(data, 0x01);
}

void LCD_Init(void) {
    Delay_ms(50); // LCD güçlendikten sonra bekleme

    LCD_Cmd(0x33); // Initialization
    LCD_Cmd(0x32); // Set to 4-bit
    LCD_Cmd(0x28); // 2 lines, 5x8 font
    LCD_Cmd(0x0C); // Display ON, cursor OFF
    LCD_Cmd(0x06); // Entry mode
    LCD_Cmd(0x01); // Clear display
    Delay_ms(2);
}

void LCD_Print(char *str) {
    while (*str) {
        LCD_Data(*str++);
    }
}

uint8_t I2C_ReadAck(void) {
    I2C0->C1 &= ~I2C_C1_TX_MASK;     // Receive mode
    I2C0->C1 &= ~I2C_C1_TXAK_MASK;   // ACK gönderilecek
    (void)I2C0->D;                   // Dummy read baslat
    while (!(I2C0->S & I2C_S_IICIF_MASK));
    I2C0->S |= I2C_S_IICIF_MASK;
    return I2C0->D;
}


uint8_t I2C_ReadNack(void) {
    I2C0->C1 &= ~I2C_C1_TX_MASK;         // Receive mode
    I2C0->C1 |= I2C_C1_TXAK_MASK;        // NACK gönderilecegini ayarla
    (void) I2C0->D;                      // Dummy read baslat
    while (!(I2C0->S & I2C_S_IICIF_MASK));
    I2C0->S |= I2C_S_IICIF_MASK;
    uint8_t data = I2C0->D;
    while (!(I2C0->S & I2C_S_IICIF_MASK));
    I2C0->S |= I2C_S_IICIF_MASK;
    I2C0->C1 &= ~I2C_C1_TXAK_MASK;       // NACK bitini temizle
    I2C0->C1 |= I2C_C1_TX_MASK;          // Transmit moda geri dön
    return data;
}

void RTC_Test_Connection(void) {
    I2C_Start();
    uint8_t ack = I2C_WriteWithAck(0xD0);  // RTC write address
    I2C_Stop();

    LCD_Cmd(0xC0);
    if (ack) {
        LCD_Print("NO ACK!");  // RTC cevap vermedi
    } else {
        LCD_Print("RTC OK");   // RTC bagli
    }
	}

int LCD_main(void){
	/* 
		I2C_Init();
    LCD_Init();
		RTC_SetTime(13, 41, 0);
		uint8_t temperature = 0;
    uint8_t humidity = 0;
	
		SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    PORTB->PCR[1] = PORT_PCR_MUX(1);
		
		RTC_Time now;
	
    while (1) {
        uint8_t status = DHT11_Read(&humidity, &temperature);

        if (status == 0) {
            LCD_Cmd(0x01); // ekrani temizle
            Delay_ms(2);

            // Sicakligi yaz
            LCD_Print("Temp: ");
            LCD_Data((temperature / 10) + '0');  // onluk basamak
            LCD_Data((temperature % 10) + '0');  // birlik basamak
            LCD_Print(" C");

            LCD_Cmd(0xC0); // 2. satira geç
            Delay_ms(2);

            // Nem yaz
            LCD_Print("Humidity: ");
            LCD_Data((humidity / 10) + '0');
            LCD_Data((humidity % 10) + '0');
            LCD_Print(" %");
					
						// 3. satir: saat
						RTC_GetTime(&now);
						char timeStr[20];
						sprintf(timeStr, "Saat: %02d:%02d:%02d", now.hour, now.minute, now.second);
						LCD_Cmd(0x94);  // 3. satir
						LCD_Print(timeStr);
					
        } else {
            LCD_Cmd(0x01);
            LCD_Print("Sensor Error");
        }

        Delay_ms(2000);
    }

    
	
		I2C_Init();
    LCD_Init();

    RTC_Time now;
    char buffer[20];

    while (1) {
        LCD_Cmd(0x01);         // ekrani temizle
        Delay_ms(2);

        LCD_Cmd(0x80);         // 1. satira geç
        LCD_Print("ZAMAN:");

        RTC_GetTime(&now);

        sprintf(buffer, "%02d:%02d:%02d", now.hour, now.minute, now.second);

        LCD_Cmd(0xC0);         // 2. satira geç
        LCD_Print(buffer);

        Delay_ms(1000);
    }


		I2C_Init();
    LCD_Init();

    LCD_Cmd(0x01);
    LCD_Print("RTC Test:");
    Delay_ms(1000);

    RTC_Test_Connection();

    while (1);


		I2C_Init();
    LCD_Init();

    while (1) {
        LCD_Cmd(0x01);
        LCD_Print("I2C TEST");
        Delay_ms(500);
    }




		I2C_Init();
    LCD_Init();

    LCD_Cmd(0x01);
    LCD_Print("RTC Test...");

    I2C_Start();
    I2C_Write(0xD0); // RTC write address
    I2C_Stop();

    LCD_Cmd(0xC0);
    LCD_Print("AFTER WRITE");

    while (1);


		I2C_Init();       // LCD için
    LCD_Init();
    ds1302_init();

    LCD_Cmd(0x01);
    LCD_Print("DS1302 TEST");
    Delay_ms(1000);

    ds1302_set_time(14, 30, 0);  // 14:30:00 olarak ayarla

    RTC_Time now;
    char buffer[20];

    while (1) {
        ds1302_get_time(&now);
        sprintf(buffer, "%02d:%02d:%02d", now.hour, now.minute, now.second);
        LCD_Cmd(0xC0);
        LCD_Print(buffer);
        Delay_ms(1000);
    }
*/

		I2C_Init();
    LCD_Init();
    


		while (1);
}
	

  