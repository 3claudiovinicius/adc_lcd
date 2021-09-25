/*
 * File:   potenciometro_adc.c
 * Author: Claudio Ohara
 *
 * Created on 24 de Setembro de 2021, 20:28
 */


//BIBLIOTECAS

#include <xc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//CONFIGURATION BITS

//CONFIG1L
#pragma config PLLDIV = 1 //?
#pragma config CPUDIV = OSC1_PLL2 //?
#pragma config USBDIV = 1 //?

//CONFIG1H
#pragma config FOSC = INTOSC_EC
#pragma config FCMEN = OFF
#pragma config IESO = OFF

//CONFIG2L
#pragma config PWRT = OFF
#pragma config BOR = ON
#pragma config BORV = 3
#pragma config VREGEN = OFF

//CONFIG2H
#pragma config WDT = OFF
#pragma config WDTPS = 32768  //?

//CONFIG3H
#pragma config CCP2MX = ON //?
#pragma config PBADEN = ON //?
#pragma config LPT1OSC = OFF
#pragma config MCLRE = OFF

//CONFIG4L
#pragma config STVREN = ON
#pragma config LVP = OFF
#pragma config ICPRT = OFF
#pragma config XINST = OFF

//CONFIG5L
#pragma config CP0 = OFF
#pragma config CP1 = OFF
#pragma config CP2 = OFF
#pragma config CP3 = OFF

//CONFIG5H
#pragma config CPB = OFF
#pragma config CPD = OFF

//CONFIG6L
#pragma config WRT0 = OFF
#pragma config WRT1 = OFF
#pragma config WRT2 = OFF
#pragma config WRT3 = OFF

//CONFIG6H
#pragma config WRTC = OFF
#pragma config WRTB = OFF
#pragma config WRTD = OFF

//CONFIG7L
#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTR2 = OFF
#pragma config EBTR3 = OFF

//CONFIG 7H
#pragma config EBTRB = OFF

#define rs LATD0
#define en LATD1
#define Idata LATB
#define vref 5

char data[10];
long int digital;
float voltage;

//LCD

void LCD_Init();
void LCD_Command(char);
void LCD_Char(char x);
void LCD_String(const char*);
void MSdelay(unsigned int);
void LCD_String_xy(char, char, const char*);

void Delay(unsigned int val){
	unsigned int i,j;
	for(i=0; i<=val; i++)
	for(j=0; j<81; j++);
}

//INTERRUPT

__interrupt(high_priority) void samples(void){
	digital = (ADRESH*256)|(ADRESL);
	voltage = digital*((float)vref/(float)1023);
	sprintf(data,"%.2f",voltage);
	strcat(data,"V");
	LCD_String_xy(2,4,data);
	PIR1bits.ADIF = 0;
}

//MAIN

void main(void){
	
	OSCCON = 0x72;
	
	TRISC = 0;
	TRISD = 0;
	TRISA = 0xFF;
	
	ADCON0 = ADCON0|1;
	ADCON1 = 0x0E;
	ADCON2 = 0x92;
	
	ADRESH = 0;
	ADRESL = 0;
	
	PIR1bits.ADIF = 0;
	PIE1bits.ADIE = 1;
	
	INTCONbits.PEIE = 1;
	INTCONbits.GIE = 1;
	
	LCD_Init();
	LCD_String_xy(1,1,"Voltage is ...");
	
	ADCON0bits.ADON = 1;
	
	while(1){
		ADCON0bits.GO = 1;
	}
}

//OUTRAS FUNÇÕES

void LCD_Init(){
	TRISB = 0;
	TRISD = 0;
	MSdelay(5);
	LCD_Command(0x38);
	LCD_Command(0x01);
	LCD_Command(0x06);
	LCD_Command(0x0C);
}

void LCD_Command(char cmd){
	Idata = cmd;
	rs = 0;
	en = 1;
	MSdelay(1);
	en = 0;
	MSdelay(3);
}

void LCD_Char(char dat){
	Idata = dat;
	rs = 1;
	en = 1;
	MSdelay(1);
	en = 0;
	MSdelay(3);
}

void LCD_String(const char *msg){
	while((*msg)!=0){
		LCD_Char(*msg);
		msg++;
	}
}

void LCD_String_xy(char row, char pos, const char *msg){
	char location = 0;
	if(row<=1){
		location = (0x80)| ((pos)&0x0F);
		LCD_Command(location);
	}else{
		location = (0xC0)| ((pos)&0x0F);
		LCD_Command(location);
	}
	LCD_String(msg);
}

void MSdelay(unsigned int val){
	unsigned int i,j;
	for(i = 0; i <= val; i++)
	for(j = 0; j < 81; j++);
}