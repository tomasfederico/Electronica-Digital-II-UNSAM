/*****************************************************************************
								Final - Tomas Federico
 *****************************************************************************/
#include "board.h"
#include "FINAL.h"
#include "math.h"

#define SCU_BASE	0x40086000	// Direccion del System Control Unit

#define	SFSP2_10	0x128		// Offset del registro de configuracion del pin - Direccion del P2_10 (Led 1)
#define SFSP1_0 	0x080		// Direccion del P1_0 (Tecla 1)
//#define NVIC		0xE000E000	//Direccion del NVIC

//#define INTERRUPT_BASE	0x40086E00	// Direccion inicial de interrupciones
//#define INTERRUPT_LIN_0	0x00000000	// Direccion de la linea de interrupcion 0

//#define ISEL			0x40087000	// Direccion de la configuracion del modo de interrupcion

//#define IENR			0x40087004	// Direccion de la habilitacion del modo de interrupcion (rise)
//#define SIENR			0x40087008	// Direccion del set del modo de interrupcion (rise)
//#define CIENR			0x4008700C	// Direccion del clear del modo de interrupcion (rise)

//#define IENF			0x40087010	// Direccion de la habilitacion del modo de interrupcion (fall)
//#define SIENF			0x40087014	// Direccion del set del modo de interrupcion (fall)
//#define CIENF			0x40087018	// Direccion del clear del modo de interrupcion (fall)

//#define ISER1			0x104		// Direccion del iser 1

#define CTRL_STATUS		0xE000E010		// Systick Control and Status
#define RELOAD_VALUE	0xE000E014		// Systick Reload Value
//#define CURRENT_VALUE	0xE000E018		// Systick Current Value
//#define CALIB_VALUE	0xE000E01C		// Systick Calibration Value


#define ADC0_CR			0x400E3000	// A/D Control Register, aca se configura el ADC 0
#define ADC0_DR1		0x400E3014	// A/D Data Register ADC0_1, es el valor leido en 10 bits, del 6 al 15
#define ADC0_DR2		0x400E3018	// A/D Data Register ADC0_2, es el valor leido en 10 bits, del 6 al 15

#define SFSP4_1 		0x204 // Pin P4_1 ADC0_1
#define SFSP4_3 		0x20C // Pin P4_3 ADC0_0

#define ENAIO0			0x40086C88

#define DAC_CR			0x400E1000		// D/A converter register
#define DAC_CTRL		0x400E1004		// D/A control register
#define DAC_CNTVAL		0x400E1008		// D/A counter value register

#define ENAIO2			0x40086C90		// habilitar la salida analogica

#define GPIO_BASE		0x400F4000	// Direccion del GPIO

#define	GPIO_PORT0_TECLA_OFFSET	0x0004		// Offset del registro TECLA del puerto 0
#define	GPIO_PORT0_DIR_OFFSET	0x2000		// Offset del registro de direccion (DIR) del puerto 0
#define GPIO_PORT0_CLR_OFFSET	0x2280		// Offset del registro clear (CLR) del puerto 0
#define GPIO_PORT0_SET_OFFSET	0x2200		// Offset del registro set (SET) del puerto 0
	int i=0;
	int ciclos=99999990;
	int aux=0;
	int aux2=0;
	int aux3=0;
	int contador = 0;
	int termo=0;
	int pote=0;
	
	static char ccc[256];		// para debugging (usada por printf)
	static char bbb[256];		// para debugging (usada por printf)
	static char aaa[256];		// para debugging (usada por printf)
	
	int *ptr3_210b = (int *)((GPIO_BASE)+(GPIO_PORT0_CLR_OFFSET));	// Direccion del registro de
																	// clear del pin P2_10
	int *ptr4_210b = (int *)((GPIO_BASE)+(GPIO_PORT0_SET_OFFSET));	// Direccion del registro de
																	// set del pin P2_10
	//Declaro punteros DAC
	int *ptr_dac_cr = (int *)(DAC_CR);			//direccion del DAC	converter register
	int *ptr_dac_ctrl = (int *)(DAC_CTRL);		//direccion del DAC control register
	int *ptr_enaio2 = (int *)(ENAIO2);			//direccion de la habilitacion dela salida analogica		

	//Declaro punteros ADC
	int *ptr_adc0_cr = (int *)(ADC0_CR);		//direccion del ADC0 Control Register
	int *ptr_adc0_dr1 = (int *)(ADC0_DR1);		//direccion del ADC0 Data Register CH1
	int *ptr_adc0_dr2 = (int *)(ADC0_DR2);		//direccion del ADC0 Data Register CH2
	int *ptr_enaio0 = (int *)(ENAIO0);			//direccion de la habilitacion de las entradas analogicas	

void SysTick_Handler(){
	
	//Para saber si entra a la interrupcion titila el led
	
	if (aux==0) {
	*ptr4_210b |= (1 << 14);		//SET P2_10		//prendo el led
	aux=1;
	
	*ptr_adc0_cr = 16777216+2097152+2| (45<<8); //bit 24 en 1 ,bit 21 en 1, bit 1 en 1, para el canal 0, (16777216+2097152+2=18874370)
	
	
	sprintf_mio(aaa,"\r\n---CH1 ->%d ---\r\n", (*ptr_adc0_dr1 >> 7) & 0x03FF);

	DEBUGSTR(aaa);
	
	}
	
	else {
	*ptr3_210b |= (1 << 14);		//CLR P2_10		//apago el led
	aux=0;
	
	*ptr_adc0_cr = 16777216+2097152+4| (45<<8); //bit 24 en 1 ,bit 21 en 1, bit 2 en 1, para el canal 0, (16777216+2097152+2=18874370)
	
	
	sprintf_mio(bbb,"\r\n---CH2 ->%d ---\r\n", (*ptr_adc0_dr2 >> 7) & 0x03FF);

	DEBUGSTR(bbb);
	
	}
	
	
	//CONTROL DE TEMPERATURA
	
	termo = (*ptr_adc0_dr1 >> 6) & 0x03FF; //ya termo y pote, estan en 9 bits. tengo q poner todo 0 del bit 10 al 32, como?
	pote = (*ptr_adc0_dr2 >> 6) & 0x03FF;
	
	aux3 = (termo - pote ) >> 1;
	//aux3 = termo - pote + 511; // aux3 [0 a 1023]  termo [0 a 511]  pote  [0 a 511]
	
	
	
	aux3 =aux3 + 512;
	
	sprintf_mio(ccc,"\r\n---DAC ->%d ---\r\n", aux3);

	DEBUGSTR(ccc);
	
	aux3 = aux3 << 6;
	
	*ptr_dac_cr = aux3 | (1<<16);
	
	
	
}

int main(void) {
	
	//Declaro Punteros teclas y leds
	int *ptr1_210 = (int *)((SCU_BASE)+(SFSP2_10));					// Direccion del registro de
																	// conf. del pin P2_10 (GPIO0[14])
	int *ptr2_210 = (int *)((GPIO_BASE)+(GPIO_PORT0_DIR_OFFSET));	// Direccion del registro de
																	// direccion del pin P2_10
	
	
	int *ptr1 = (int *)((SCU_BASE)+(SFSP2_10));					// Direccion del registro de
																// conf. del pin P2_10 (GPIO0[14])
	int *ptr11 = (int *)((SCU_BASE)+(SFSP1_0));					// Direccion del registro de
																// conf. del pin P1_0 (GPIO0[4])

	//Declaro Punteros Systick
	int *ptr_ctrl_status = (int *)(CTRL_STATUS);
	int *ptr_reload_value = (int *)(RELOAD_VALUE);
	//int *ptr_current_value = (int *)(CURRENT_VALUE);
	
	
	//Declaro Punteros ADC
	int *ptr1_41 = (int *)((SCU_BASE)+(SFSP4_1));					// Direccion del registro de
																	// conf. del pin P4_1 (ADC0_1)
	int *ptr1_43 = (int *)((SCU_BASE)+(SFSP4_3));					// Direccion del registro de
																	// conf. del pin P4_3 (ADC0_0)
	
	
	//Configuro Led
	*ptr1_210 = (0x2 << 3) | (0x0);		//Funcion 0 en el SCU
	*ptr2_210 |= (1 << 14) ;			// pone el 1 en el bit 14 del DIR
	*ptr2_210 &= ~(1 << 4) ;			// creo que pone un 0 en el bit 4
	
	//Configuro Systick
	*ptr_ctrl_status = 3;
	*ptr_reload_value = (0x16E3600); //valor en hexa que equivale a 12000000, para que se ejecute cada 1 seg B71B00
	
	//Configuro Tecla
	*ptr1 = (0x2 << 3) | (0x0);
	*ptr11 = (0x2 << 3) | (0x0) | (1 <<6);
	
	//Configuro DAC
	
	*ptr_enaio2 = 1;
	*ptr_dac_ctrl = 14; //para poner el bit 1 , 2 y 3 en TRUE
	
	//Configuro ADC
	//*ptr_enaio0 = 0;//3
	//*ptr_adc0_cr = 18874369; //bit 24 en 1 ,bit 21 en 1, bit 0 en 1, para el canal 0, (16777216+2097152+1=18874369)
	//*ptr1_41 = 7; //(00111) pull up y pull down van deshabilitados
	//*ptr1_43 = 7; //(10111)??
	
	UART_Init();

	while (1) {
	
	}
	return 0;
}