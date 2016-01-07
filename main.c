/*****************************************************
 This program was produced by the
 CodeWizardAVR V2.05.3 Professional
 Automatic Program Generator
 © Copyright 1998-2011 Pavel Haiduc, HP InfoTech s.r.l.
 http://www.hpinfotech.com

 Project : 
 Version : 
 Date    : 21.09.2015
 Author  : usertt
 Company : 
 Comments: 


 Chip type               : ATmega8
 Program type            : Application
 AVR Core Clock frequency: 8,000000 MHz
 Memory model            : Small
 External RAM size       : 0
 Data Stack size         : 256
 *****************************************************/

#asm
.equ __w1_port=0x15 //PORTC
.equ __w1_bit=3
#endasm

//#define REAL_DATES //uncomment for use real date

#include <1wire.h>
#include <mega8.h>
#include <io.h>
#include <delay.h>


#include "ds3231_twi.c"
#include "nokia5110_lcd.c"
#include "ds18b20.h"

#ifdef REAL_DATES
#include "dates.h"
#endif

#ifndef REAL_DATES
#include "dates_dummy.h"
#endif

#define TRUE 1
#define FALSE 0

//count of ds18b20
#define MAX_DS18b20 2

#define LED_BACKLIGT PORTD.0

#define MODE_SHOW_MAIN_INFO 0
#define MODE_SET_DATE_YEAR 1
#define MODE_SET_DATE_MONTH 2
#define MODE_SET_DATE_DAY_OF_WEEK 3
#define MODE_SET_DATE_DAY 4
#define MODE_SET_TIME_HOUR 5
#define MODE_SET_TIME_MINUTE 6
#define MODE_SHOW_DAY_INFO 7

// Declare your global variables here

unsigned char *dayOfWeek[8] = {
    "Unk",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
    "Sun"
};

ds18b20_temperature_data_struct temperature[MAX_DS18b20];
unsigned char ds18b20_devices;
unsigned char rom_code[MAX_DS18b20][9];

unsigned char seconds;
unsigned char minutes;
unsigned char hours;
unsigned char day;
unsigned char date;
unsigned char month;
unsigned char year;

unsigned char i;

unsigned char mode;

bit dateInfoPresent;
unsigned char dateIndex;

bit button_1_on;
bit button_2_on;
bit button_3_on;


void printTwoDigit(unsigned char number);
void checkDate(void);
unsigned char getLastMonthDay(void);

void doBtn1Action(void){
    mode = mode < 6 ? (mode + 1) : 0;
}

void doBtn2Action(void){
    switch(mode){
        case MODE_SHOW_MAIN_INFO: {
            mode = MODE_SHOW_DAY_INFO;
            break;
        }
        case MODE_SET_DATE_YEAR: {
            year = year < 99 ? (year + 1) : 0;
            rtc_set_time(minutes, hours, day, date, month, year);
            break;
        }
        case MODE_SET_DATE_MONTH: {
            month = month < 12 ? (month + 1) : 1;
            rtc_set_time(minutes, hours, day, date, month, year);
            checkDate();
            break;
        }
        case MODE_SET_DATE_DAY_OF_WEEK: {
            day = day < 7 ? (day + 1) : 1;
            rtc_set_time(minutes, hours, day, date, month, year);
            checkDate();
            break;
        }
        case MODE_SET_DATE_DAY: {
            date = date < getLastMonthDay() ? (date + 1) : 1;
            rtc_set_time(minutes, hours, day, date, month, year);
            checkDate();
            break;
        }
        case MODE_SET_TIME_HOUR: {
            hours = hours < 23 ? (hours + 1) : 0;
            rtc_set_time(minutes, hours, day, date, month, year);
            break;
        }
        case MODE_SET_TIME_MINUTE: {
            minutes = minutes < 59 ? (minutes + 1) : 0;
            rtc_set_time(minutes, hours, day, date, month, year);
            break;
        }
        case MODE_SHOW_DAY_INFO: {
            mode = MODE_SHOW_MAIN_INFO;
            break;
        }
    }
}
void doBtn3Action(void){
    switch(mode){
        case MODE_SHOW_MAIN_INFO:
        case MODE_SHOW_DAY_INFO: {
            LED_BACKLIGT = ~LED_BACKLIGT;
            break;
        }
        case MODE_SET_DATE_YEAR: {
            year = year >0 ? (year - 1) : 99;
            rtc_set_time(minutes, hours, day, date, month, year);
            break;
        }
        case MODE_SET_DATE_MONTH: {
            month = month >1 ? (month - 1) : 12;
            rtc_set_time(minutes, hours, day, date, month, year);
            checkDate();
            break;
        }
        case MODE_SET_DATE_DAY_OF_WEEK: {
            day = day >1 ? (day - 1) : 7;
            rtc_set_time(minutes, hours, day, date, month, year);
            checkDate();
            break;
        }
        case MODE_SET_DATE_DAY: {
            date = date >1 ? (date - 1) : getLastMonthDay();
            rtc_set_time(minutes, hours, day, date, month, year);
            checkDate();
            break;
        }
        case MODE_SET_TIME_HOUR: {
            hours = hours >0 ? (hours - 1) : 23;
            rtc_set_time(minutes, hours, day, date, month, year);
            break;
        }
        case MODE_SET_TIME_MINUTE: {
            minutes = minutes >0 ? (minutes - 1) : 59;
            rtc_set_time(minutes, hours, day, date, month, year);
            break;
        }
    }
} 


interrupt [TIM1_COMPA] void timer1_compa_isr(void) {
    if(!PINC.0){ 
        if(button_1_on){
            doBtn1Action();
        }
        button_1_on = !button_1_on;
    }
    if(!PINC.1){ 
        if(button_2_on){
            doBtn2Action();
        }
        button_2_on = !button_2_on;
    }
    if(!PINC.2){ 
        if(button_3_on){
            doBtn3Action();
        }
        button_3_on = !button_3_on;
    }
    TCNT1=0;
}

unsigned char getLastMonthDay(void){
    unsigned char retVal = 31;
    switch(month){
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            retVal = 31;
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            retVal = 30;
            break;
        case 2:
            retVal = year % 4 ==  0 ? 29 : 28;
            break;
    }
    return retVal;           
}

void readTemperatureValues(void){
    if (ds18b20_devices >= 1) {
        for (i = 0; i < ds18b20_devices; i++) {
        	temperature[i] = ds18b20_temperature_struct(&rom_code[i][0]);
        }
    }
}

void printTemperatureValue(ds18b20_temperature_data_struct temperature){
    unsigned char decades;
    lcd_putch(temperature.minusChar ? '-' : ' ' );
    decades = temperature.temperatureIntValue / 10;
    if(decades > 0) 
        lcd_putch(decades + 48);
    lcd_putch(temperature.temperatureIntValue % 10 + 48);
    if(temperature.halfDegree){
        lcd_putch('.');
        lcd_putch('5');
    }
}

void displayTime(void) {
    printTwoDigit(hours);
    lcd_putch(':');
    printTwoDigit(minutes);
    lcd_putch(':');    
    printTwoDigit(seconds);
}

void displayTimeBig(void) {
    writeCharBig(0,1,hours / 10 + 48);
    writeCharBig(14,1,hours % 10 + 48);
    writeCharBig(28,1,':');
    writeCharBig(34,1,minutes / 10 + 48);
    writeCharBig(48,1,minutes % 10 + 48);
    //show small seconds
    lcd_gotoxy(66, 3);
    lcd_putch(seconds / 10 + 48);
    lcd_putch(seconds % 10 + 48);
}

void displayDate(void) {
    printTwoDigit(date);
    lcd_putch('-');
    printTwoDigit(month);
    lcd_putch('-');    
    printTwoDigit(year);
    lcd_putch(' ');
    lcd_str(dayOfWeek[day]);
}

void displayMainInfo(void) {
    lcd_clear(); 
    displayTimeBig();

    if (ds18b20_devices >= 0) {
        lcd_gotoxy(0, 0);
        for (i = 0; i < ds18b20_devices; i++) {
            if(temperature[i].valid){
                printTemperatureValue(temperature[i]);
                lcd_putch('C');
                lcd_putch(' ');
            }
        }
    }
    if(dateInfoPresent) {
        lcd_gotoxy(66, 1);
        lcd_str("di");
    }

    lcd_gotoxy(10, 5);
    displayDate();
}

void displayDayInfo(void) {
    lcd_clear(); 
    lcd_gotoxy(0, 0);
    lcd_str("Day info");
    lcd_gotoxy(0, 2);
    lcd_str(msg[dateIndex]);
    lcd_gotoxy(10, 5);
    displayDate();
}

void printTwoDigit(unsigned char number){
    lcd_putch(number / 10 + 48);
    lcd_putch(number % 10 + 48);
}
void displaySetDate(unsigned char mode) {
    lcd_clear();
    lcd_gotoxy(0, 0);
    switch(mode){
        
        case MODE_SET_DATE_YEAR:
            lcd_str("Set year");       
            break;
        case MODE_SET_DATE_MONTH:
            lcd_str("Set month");       
            break;
        case MODE_SET_DATE_DAY_OF_WEEK:
            lcd_str("Set day week");    
            break;
        case MODE_SET_DATE_DAY:
            lcd_str("Set day");       
            break;
    } 
    lcd_gotoxy(0, 2);
    displayDate();
}

void displaySetTime(unsigned char mode) {
    
    lcd_clear();
    lcd_gotoxy(0, 0);
    switch(mode){
        case MODE_SET_TIME_HOUR:
            lcd_str("Set hours");       
            break;
        case MODE_SET_TIME_MINUTE:
            lcd_str("Set minute");       
            break;
    } 
    lcd_gotoxy(0, 2);
    displayTime();
}

void displayInfo(void){
    switch(mode){
        case MODE_SHOW_MAIN_INFO:
            displayMainInfo();       
            break;

        case MODE_SET_DATE_YEAR:
        case MODE_SET_DATE_MONTH:
        case MODE_SET_DATE_DAY_OF_WEEK:
        case MODE_SET_DATE_DAY:
            displaySetDate(mode);       
            break;               
        
        case MODE_SET_TIME_HOUR:
        case MODE_SET_TIME_MINUTE:
            displaySetTime(mode);       
            break;
       
        case MODE_SHOW_DAY_INFO:
            displayDayInfo();       
            break;
    } 
}

void checkDate(void){
    unsigned char i;
    dateIndex = 0;
    dateInfoPresent = FALSE;
    for (i = 0; i < DATE_SIZE; i++) {    
        if (dates[i][0] == date && dates[i][1] == month) {
            dateIndex = i;
            dateInfoPresent = TRUE;
            return;
        }
    }
}

void main(void) {
// Declare your local variables here
    unsigned char tempCounter = 0;
    
    PORTB = 0x00;
	DDRB = 0xFF;

	PORTC = 0x07;
	DDRC = 0xF8;

	PORTD = 0x00;
	DDRD = 0xFF;
    
    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: 7,813 kHz
    // Mode: Normal top=0xFFFF
    // OC1A output: Discon.
    // OC1B output: Discon.
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer1 Overflow Interrupt: Off
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: On
    // Compare B Match Interrupt: Off
    TCCR1A=0x00;
    TCCR1B=0x05;
    OCR1AH=0x03;
    OCR1AL=0x0D;
 
 //twi_master_init(100);
	TWBR = 0x0C;
	TWAR = 0xD0;
	TWCR = 0x44;

	w1_init();
	ds18b20_devices = w1_search(0xf0, rom_code);           

    // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK=0x10;
 

    // Global enable interrupts
    #asm("sei")

	ds3231_init();
	lcd_init();
	lcd_contrast(0x40);

    rtc_get_time(&seconds, &minutes, &hours, &day, &date, &month, &year); 

    //skip first values
    if (ds18b20_devices >= 0) {
        for (i = 0; i < ds18b20_devices; i++) {    
        ds18b20_temperature(&rom_code[i][0]);
        }
     }

    checkDate();

	while (1) {
		rtc_get_time(&seconds, &minutes, &hours, &day, &date, &month, &year); 
        if(hours == 0 && minutes == 0){
            checkDate();
        }
        if(tempCounter == 20){
            if(mode == MODE_SHOW_MAIN_INFO) readTemperatureValues();
            tempCounter = 0;
        }
		displayInfo();
        tempCounter++;
        delay_ms(100);
   }
}
