#ifndef _DATES_H_
#define _DATES_H_

#define DATE_SIZE 2

flash unsigned char dates[DATE_SIZE][2] = {
    {0,0},
    {7,1}
};

unsigned char *msg[DATE_SIZE] = {
    "А Сегодня все Очень Уныло",
    "Тестовый День"
};

#endif