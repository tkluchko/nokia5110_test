#include "ds3231_twi.h"

void twi_start(void) {
    TWCR = (1<<TWEA)|(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    
    while (!(TWCR & (1<<TWINT)))  {; }
}

void twi_stop(void) {
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

void twi_write(unsigned char _data)
{
    TWDR = _data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    
    while (!(TWCR & (1<<TWINT))) {;}
}

unsigned char twi_read(unsigned char _ack) {
    unsigned char _data;

    if (_ack==1)
    {
        TWCR = (1<<TWEA)|(1<<TWINT) | (1<<TWEN);
    }
    else
    {
        TWCR = (1<<TWINT) | (1<<TWEN);
    }
    while (!(TWCR & (1<<TWINT)))
    {
    }
    _data = TWDR;
    return _data;
    
   }
   
unsigned char bcd (unsigned char data) {
    return (((data & 0b11110000)>>4)*10 + (data & 0b00001111));
}
 unsigned char decToBcd(unsigned char val)
 {
   return ( (val/10*16) + (val%10) );
 }


   
void ds3231_init(void){
twi_start();                           //Кидаем команду "Cтарт" на шину I2C
twi_write(DS3231_I2C_ADDRESS_WRITE);              // 104 is DS3231 device address
twi_write(0x0E);                            //выставляемся в 14й байт
twi_write(0b00000000);                       //сбрасываем контрольные регистры
twi_write(0b10001000);                       //выставляем 1 на статус OSF и En32kHz
twi_stop();                         

   
} 

void rtc_get_time(unsigned char *secondsParam, unsigned char *minutesParam, unsigned char *hoursParam, unsigned char *dayParam, unsigned char *dateParam, unsigned char *monthParam, unsigned char *yearParam) {   

twi_start();                           //Кидаем команду "Cтарт" на шину I2C
twi_write(DS3231_I2C_ADDRESS_WRITE);              // 104 is DS3231 device address
twi_write(0x00);
twi_stop();

twi_start();
	twi_write(DS3231_I2C_ADDRESS_READ);

     *secondsParam = bcd(twi_read(1)); // get seconds
     *minutesParam = bcd(twi_read(1)); // get minutes
     *hoursParam   = bcd(twi_read(1));   // get hours
     *dayParam     = bcd(twi_read(1));
     *dateParam    = bcd(twi_read(1));
     *monthParam   = bcd(twi_read(1)); //temp month
     *yearParam    = bcd(twi_read(0));
     
twi_stop();    
}

void rtc_set_time(unsigned char minutesParam, unsigned char hoursParam, unsigned char dayParam, unsigned char dateParam, unsigned char monthParam, unsigned char yearParam) {
    twi_start();
    twi_write(DS3231_I2C_ADDRESS_WRITE);
    twi_write(0);
    twi_write(decToBcd(0));
    twi_write(decToBcd(minutesParam));
    twi_write(decToBcd(hoursParam));
    twi_write(decToBcd(dayParam));
    twi_write(decToBcd(dateParam));
    twi_write(decToBcd(monthParam));
    twi_write(decToBcd(yearParam));
    twi_stop();
}


