/****************************************************************************************************************/
/*When this flag is defined the led is enabled. Every transition and received advertisementpacket triggers	  	*/
/*the LED to flicker depending on the settings set for that event.												*/
/****************************************************************************************************************/
#define CFG_ENABLE_LED

/****************************************************************************************************************/
/*Macro function for converting the amount of pulses to the required amount of callbacks needed for the pulses	*/
/****************************************************************************************************************/
#define CONVERT_TO_CB_AMOUNT(x) (x * 2)

/****************************************************************************************************************/
/*Settings for the time it take for one phase of the routine in milliseconds									*/
/****************************************************************************************************************/
#define ROUTINE_CYCLE_TIME_MS 		(uint32_t)(10000)
#define ROUTINE_CYCLE_TIME			(uint32_t)(ROUTINE_CYCLE_TIME_MS / 10)

#define ROUTINE_PAUSE_TIME_MS		(uint32_t)(5000)
#define ROUTINE_PAUSE_TIME			(uint32_t)(ROUTINE_PAUSE_TIME_MS/10)

/****************************************************************************************************************/
/*Settings for the time it take for one phase of the routine in milliseconds									*/
/****************************************************************************************************************/
#if defined(CFG_ENABLE_LED)
	#define TIMER_DIVIDER			(uint32_t)(10)
	
	#define LED_NONCONN_PULSES		(uint8_t)(2)
	#define LED_NONCONN_TIME_MS		(uint32_t)(250)

	#define LED_CONN_PULSES			(uint8_t)(3)
	#define LED_CONN_TIME_MS		(uint32_t)(250)
	
	#define LED_SCAN_PULSES			(uint8_t)(5)
	#define LED_SCAN_TIME_MS		(uint32_t)(100)

	#define LED_RECEIVED_TIME_MS	(uint32_t)(75)
	
	#define LED_NONCONN_TIMER_TIME	(uint32_t)(LED_NONCONN_TIME_MS / TIMER_DIVIDER)
	#define LED_RECEIVED_TIMER_TIME	(uint32_t)(LED_SCAN_TIME_MS / TIMER_DIVIDER)
	#define LED_CONN_TIMER_TIME		(uint32_t)(LED_CONN_TIME_MS / TIMER_DIVIDER)
	#define LED_SCAN_TIMER_TIME		(uint32_t)(LED_SCAN_TIME_MS / TIMER_DIVIDER)
#endif
