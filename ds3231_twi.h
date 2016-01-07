#ifndef _DS323TWI_H_
#define _DS323TWI_H_

//#define TWEN    2
//#define TWIE    0
//#define TWINT   7
//#define TWEA    6
//#define TWSTA   5
//#define TWSTO   4
//#define TWWC    3

#define DS3231_I2C_ADDRESS_WRITE 0b11010000
#define DS3231_I2C_ADDRESS_READ 0b11010001
   
void ds3231_init(void);
void rtc_get_time(unsigned char *secondsParam, unsigned char *minutesParam, unsigned char *hoursParam, unsigned char *dayParam, unsigned char *dateParam, unsigned char *monthParam, unsigned char *yearParam);
void rtc_set_time(unsigned char minutesParam, unsigned char hoursParam, unsigned char dayParam, unsigned char dateParam, unsigned char monthParam, unsigned char yearParam);

#endif