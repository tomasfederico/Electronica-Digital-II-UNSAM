/*****************************************************************************
								Final - Tomas Federico
 *****************************************************************************/
#include "board.h"
#include "FINAL.h"
#include "math.h"

#define SCU_BASE	0x40086000	// Direccion del System Control Unit

#define	SFSP2_10	0x128		// Offset del registro de configuracion del pin - Direccion del P2_10 (Led 1)
#define SFSP1_0 	0x080		// Direccion del P1_0 (Tecla 1)

#define CTRL_STATUS		0xE000E010		// Systick Control and Status
#define RELOAD_VALUE	0xE000E014		// Systick Reload Value

#define ADC0_CR			0x400E3000	// A/D Control Register, aca se configura el ADC 0
#define ADC0_DR1		0x400E3014	// A/D Data Register ADC0_1, es el valor leido en 10 bits, del 6 al 15
#define ADC0_DR2		0x400E3018	// A/D Data Register ADC0_2, es el valor leido en 10 bits, del 6 al 15

#define DAC_CR			0x400E1000		// D/A converter register
#define DAC_CTRL		0x400E1004		// D/A control register
#define DAC_CNTVAL		0x400E1008		// D/A counter value register

#define ENAIO2			0x40086C90		// habilitar la salida analogica

#define GPIO_BASE		0x400F4000	// Direccion del GPIO

#define	GPIO_PORT0_TECLA_OFFSET	0x0004		// Offset del registro TECLA del puerto 0
#define	GPIO_PORT0_DIR_OFFSET	0x2000		// Offset del registro de direccion (DIR) del puerto 0
#define GPIO_PORT0_CLR_OFFSET	0x2280		// Offset del registro clear (CLR) del puerto 0
#define GPIO_PORT0_SET_OFFSET	0x2200		// Offset del registro set (SET) del puerto 0

//____________________________________________________________________________________________________________//

	int k=0;
	int valor_grados=0;
	int aux=0;
	int aux3=0;
	int termo=0;
	int pote=0;
	int GRADOS[98];
	int ADC[98];

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

//____________________________________________________________________________________________________________//

void SysTick_Handler(){
	
	//Para saber si entra a la interrupcion titila el led
	
	if (aux==0) {
	*ptr4_210b |= (1 << 14);		//SET P2_10		//prendo el led
	aux=1;
	
	*ptr_adc0_cr = (16777216+2097152+2) | (45<<8); // configuracion para leer CH1
	//bit 24 en 1 ,bit 21 en 1, bit 1 en 1, para el canal 1, (16777216+2097152+2=18874370) y (45<<8) magia
	
	for (k=0; k<98; k++){	
		if (ADC[k] < ((*ptr_adc0_dr1 >> 6)& 0x03FF)) {
		valor_grados = GRADOS[k];
		}else{
		
		}
	}	
	
	}
	
	else {
	*ptr3_210b |= (1 << 14);		//CLR P2_10		//apago el led
	aux=0;
	
	*ptr_adc0_cr = (16777216+2097152+4) | (45<<8); // configuracion para leer CH2
	//bit 24 en 1 ,bit 21 en 1, bit 2 en 1, para el canal 2, (16777216+2097152+4=18874372) y (45<<8) magia
	
	sprintf_mio(aaa,"\r\n---CH1 PV ->%d ---\r\n", valor_grados);
	DEBUGSTR(aaa);
	
	sprintf_mio(bbb,"\r\n---CH2 SP ->%d ---\r\n", ((*ptr_adc0_dr2 >> 6) & 0x03FF));
	DEBUGSTR(bbb);
	
	
	
	}
	

	//CONTROL DE TEMPERATURA
	
	termo = (*ptr_adc0_dr1 >> 6) & 0x03FF; //muevo 6 posiciones la lectura, y pongo a cero toda la parte alta
	pote = (*ptr_adc0_dr2 >> 6) & 0x03FF; //es decir del bit 10 al 32 vale 0, y del bit 0 al 9 vale lo leido
	
	aux3 = (1023 - termo - pote ) >> 1; //muevo la cuenta una posicion para que quede en 9 bits [0 a 511]
	
	aux3 =aux3 + 512; // se mueve de [0 a 1023]
	
	sprintf_mio(ccc,"\r\n---DAC ->%d ---\r\n", aux3); //aux3
	DEBUGSTR(ccc);
	
	aux3 = aux3 << 6;
	
	*ptr_dac_cr = aux3 | (1<<16);
	

}

//____________________________________________________________________________________________________________//

int main(void) {
	
GRADOS [0] = 406 ;
GRADOS [1] = 401 ;
GRADOS [2] = 397 ;
GRADOS [3] = 393 ;
GRADOS [4] = 389 ;
GRADOS [5] = 385 ;
GRADOS [6] = 381 ;
GRADOS [7] = 378 ;
GRADOS [8] = 374 ;
GRADOS [9] = 370 ;
GRADOS [10] = 366 ;
GRADOS [11] = 363 ;
GRADOS [12] = 359 ;
GRADOS [13] = 356 ;
GRADOS [14] = 352 ;
GRADOS [15] = 349 ;
GRADOS [16] = 345 ;
GRADOS [17] = 342 ;
GRADOS [18] = 339 ;
GRADOS [19] = 336 ;
GRADOS [20] = 332 ;
GRADOS [21] = 329 ;
GRADOS [22] = 326 ;
GRADOS [23] = 323 ;
GRADOS [24] = 320 ;
GRADOS [25] = 317 ;
GRADOS [26] = 314 ;
GRADOS [27] = 311 ;
GRADOS [28] = 308 ;
GRADOS [29] = 305 ;
GRADOS [30] = 302 ;
GRADOS [31] = 300 ;
GRADOS [32] = 297 ;
GRADOS [33] = 294 ;
GRADOS [34] = 291 ;
GRADOS [35] = 289 ;
GRADOS [36] = 286 ;
GRADOS [37] = 283 ;
GRADOS [38] = 281 ;
GRADOS [39] = 278 ;
GRADOS [40] = 276 ;
GRADOS [41] = 273 ;
GRADOS [42] = 271 ;
GRADOS [43] = 268 ;
GRADOS [44] = 266 ;
GRADOS [45] = 263 ;
GRADOS [46] = 261 ;
GRADOS [47] = 259 ;
GRADOS [48] = 256 ;
GRADOS [49] = 254 ;
GRADOS [50] = 252 ;
GRADOS [51] = 250 ;
GRADOS [52] = 247 ;
GRADOS [53] = 245 ;
GRADOS [54] = 243 ;
GRADOS [55] = 241 ;
GRADOS [56] = 238 ;
GRADOS [57] = 236 ;
GRADOS [58] = 234 ;
GRADOS [59] = 232 ;
GRADOS [60] = 230 ;
GRADOS [61] = 228 ;
GRADOS [62] = 226 ;
GRADOS [63] = 224 ;
GRADOS [64] = 222 ;
GRADOS [65] = 220 ;
GRADOS [66] = 218 ;
GRADOS [67] = 216 ;
GRADOS [68] = 214 ;
GRADOS [69] = 212 ;
GRADOS [70] = 210 ;
GRADOS [71] = 208 ;
GRADOS [72] = 206 ;
GRADOS [73] = 204 ;
GRADOS [74] = 203 ;
GRADOS [75] = 201 ;
GRADOS [76] = 199 ;
GRADOS [77] = 197 ;
GRADOS [78] = 195 ;
GRADOS [79] = 193 ;
GRADOS [80] = 192 ;
GRADOS [81] = 190 ;
GRADOS [82] = 188 ;
GRADOS [83] = 186 ;
GRADOS [84] = 185 ;
GRADOS [85] = 183 ;
GRADOS [86] = 181 ;
GRADOS [87] = 180 ;
GRADOS [88] = 178 ;
GRADOS [89] = 176 ;
GRADOS [90] = 175 ;
GRADOS [91] = 173 ;
GRADOS [92] = 171 ;
GRADOS [93] = 170 ;
GRADOS [94] = 168 ;
GRADOS [95] = 167 ;
GRADOS [96] = 165 ;
GRADOS [97] = 164 ;
GRADOS [98] = 162 ;



ADC[0] = 562 ;
ADC[1] = 566 ;
ADC[2] = 571 ;
ADC[3] = 575 ;
ADC[4] = 579 ;
ADC[5] = 584 ;
ADC[6] = 588 ;
ADC[7] = 592 ;
ADC[8] = 596 ;
ADC[9] = 600 ;
ADC[10] = 604 ;
ADC[11] = 608 ;
ADC[12] = 612 ;
ADC[13] = 616 ;
ADC[14] = 620 ;
ADC[15] = 624 ;
ADC[16] = 627 ;
ADC[17] = 631 ;
ADC[18] = 635 ;
ADC[19] = 638 ;
ADC[20] = 642 ;
ADC[21] = 646 ;
ADC[22] = 649 ;
ADC[23] = 653 ;
ADC[24] = 656 ;
ADC[25] = 660 ;
ADC[26] = 663 ;
ADC[27] = 666 ;
ADC[28] = 670 ;
ADC[29] = 673 ;
ADC[30] = 676 ;
ADC[31] = 680 ;
ADC[32] = 683 ;
ADC[33] = 686 ;
ADC[34] = 689 ;
ADC[35] = 692 ;
ADC[36] = 695 ;
ADC[37] = 698 ;
ADC[38] = 702 ;
ADC[39] = 705 ;
ADC[40] = 708 ;
ADC[41] = 710 ;
ADC[42] = 713 ;
ADC[43] = 716 ;
ADC[44] = 719 ;
ADC[45] = 722 ;
ADC[46] = 725 ;
ADC[47] = 728 ;
ADC[48] = 730 ;
ADC[49] = 733 ;
ADC[50] = 736 ;
ADC[51] = 739 ;
ADC[52] = 741 ;
ADC[53] = 744 ;
ADC[54] = 747 ;
ADC[55] = 749 ;
ADC[56] = 752 ;
ADC[57] = 754 ;
ADC[58] = 757 ;
ADC[59] = 759 ;
ADC[60] = 762 ;
ADC[61] = 764 ;
ADC[62] = 767 ;
ADC[63] = 769 ;
ADC[64] = 772 ;
ADC[65] = 774 ;
ADC[66] = 776 ;
ADC[67] = 779 ;
ADC[68] = 781 ;
ADC[69] = 783 ;
ADC[70] = 786 ;
ADC[71] = 788 ;
ADC[72] = 790 ;
ADC[73] = 792 ;
ADC[74] = 795 ;
ADC[75] = 797 ;
ADC[76] = 799 ;
ADC[77] = 801 ;
ADC[78] = 803 ;
ADC[79] = 806 ;
ADC[80] = 808 ;
ADC[81] = 810 ;
ADC[82] = 812 ;
ADC[83] = 814 ;
ADC[84] = 816 ;
ADC[85] = 818 ;
ADC[86] = 820 ;
ADC[87] = 822 ;
ADC[88] = 824 ;
ADC[89] = 826 ;
ADC[90] = 828 ;
ADC[91] = 830 ;
ADC[92] = 832 ;
ADC[93] = 834 ;
ADC[94] = 836 ;
ADC[95] = 838 ;
ADC[96] = 839 ;
ADC[97] = 841 ;
ADC[98] = 843 ;

	
	//Declaro Punteros teclas y leds
	int *ptr1_210 = (int *)((SCU_BASE)+(SFSP2_10));					// Direccion del registro de
																	// conf. del pin P2_10 (GPIO0[14])
	int *ptr2_210 = (int *)((GPIO_BASE)+(GPIO_PORT0_DIR_OFFSET));	// Direccion del registro de
																	// direccion del pin P2_10
	
	int *ptr1_200 = (int *)((SCU_BASE)+(SFSP2_0));		// Direccion del registro de
														// conf. del pin P2_00 (GPIO5[0])
	int *ptr2_200 = (int *)((GPIO_BASE)+(0x2014));		// Direccion del registro de
														// direccion del pin P2_00
	
	int *ptr1_201 = (int *)((SCU_BASE)+(SFSP2_1));		// Direccion del registro de
														// conf. del pin P2_01 (GPIO5[1])
	int *ptr2_201 = (int *)((GPIO_BASE)+(0x2014));		// Direccion del registro de
														// direccion del pin P2_01						
	
	int *ptr1_202 = (int *)((SCU_BASE)+(SFSP2_2));		// Direccion del registro de
														// conf. del pin P2_02 (GPIO5[2])
	int *ptr2_202 = (int *)((GPIO_BASE)+(0x2014));		// Direccion del registro de
														// direccion del pin P2_02
	
	int *ptr1 = (int *)((SCU_BASE)+(SFSP2_10));					// Direccion del registro de
																// conf. del pin P2_10 (GPIO0[14])
	int *ptr11 = (int *)((SCU_BASE)+(SFSP1_0));					// Direccion del registro de
																// conf. del pin P1_0 (GPIO0[4])

	//Declaro Punteros Systick
	int *ptr_ctrl_status = (int *)(CTRL_STATUS);
	int *ptr_reload_value = (int *)(RELOAD_VALUE);
	//int *ptr_current_value = (int *)(CURRENT_VALUE);
	
	
	//Configuro Led's
	*ptr1_210 = (0x2 << 3) | (0x0);		//Funcion 0 en el SCU
	*ptr2_210 |= (1 << 14) ;			// pone el 1 en el bit 14 del DIR
	*ptr2_210 &= ~(1 << 4) ;			// creo que pone un 0 en el bit 4
	
	*ptr1_200 = (0x5 << 2) | (0x0);		//Funcion 4 en el SCU
	*ptr2_200 |= (1 << 0) ;			// pone el 1 en el bit 0 del DIR
	
	*ptr1_201 = (0x5 << 2) | (0x0);		//Funcion 4 en el SCU
	*ptr2_201 |= (1 << 1) ;			// pone el 1 en el bit 1 del DIR
	
	*ptr1_202 = (0x5 << 2) | (0x0);		//Funcion 4 en el SCU
	*ptr2_202 |= (1 << 2) ;			// pone el 1 en el bit 2 del DIR
	
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
	//*ptr_adc0_cr = 18874369; //bit 24 en 1 ,bit 21 en 1, bit 0 en 1, para el canal 0, (16777216+2097152+1=18874369)

	
	UART_Init();
	
//____________________________________________________________________________________________________________//

	while (1) {
	
	}
	return 0;
}
//____________________________________________________________________________________________________________//