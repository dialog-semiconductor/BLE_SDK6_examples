#ifndef __USER_WAKEUP_H__
#define __USER_WAKEUP_H__


#include "stdint.h"
#include "gpio.h"

#define DEBOUNCE_TIME 						(63) //MAX Debounce 
#define EVENTS_BEFORE_INTERRUPT		(1)


typedef void (*uwakeup_cb)(void);

void uwakeup_init(uwakeup_cb cb, GPIO_PORT port, GPIO_PIN pin, uint8_t polarity );

#endif //__USER_WAKEUP_H__

