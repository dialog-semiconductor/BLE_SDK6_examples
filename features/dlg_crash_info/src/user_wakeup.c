
#include "wkupct_quadec.h" 
#include "user_wakeup.h"
#include "user_periph_setup.h"
#include "gpio.h"

typedef struct
{
	uwakeup_cb cb;
	GPIO_PORT port;
	GPIO_PIN	pin;
	uint8_t polarity;
	
}wakeup_config_t;


wakeup_config_t	wakeup_config								__SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

static void _uwakeup_cb(void);

static void uwakeup_reinit(void)
{
	wkupct_enable_irq( 	WKUPCT_PIN_SELECT(wakeup_config.port, wakeup_config.pin), 
										WKUPCT_PIN_POLARITY(wakeup_config.port, wakeup_config.pin, wakeup_config.polarity), 
										EVENTS_BEFORE_INTERRUPT, 
										DEBOUNCE_TIME);
	
	wkupct_register_callback(_uwakeup_cb);
}

static void _uwakeup_cb(void)
{
	uwakeup_reinit();
	wakeup_config.cb();
}



void uwakeup_init(uwakeup_cb cb, GPIO_PORT port, GPIO_PIN pin, uint8_t polarity )
{
	
	wakeup_config.cb = cb;
	wakeup_config.port = port;
	wakeup_config.pin = pin;
	wakeup_config.polarity = polarity;
	
	uwakeup_reinit();

	
}
