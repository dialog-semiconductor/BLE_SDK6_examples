
------

# DA14531 Timer1 Software Example

------

## Example Description

This software example demonstrates using **TIMER1** hardware block. The sw example exposes the basic functions that **TIMER1** offers through the following use cases:
1. **Simple counting** which is enabled via setting the **ENABLE_TMR_COUNTING** definition to 1 in the demo_config.h file. In this case **TIMER1** is used as an ordinary counter/timer and exposes the capability of the timer to run while sleep and wake up the device.
2. **Frequency measurement** exposes the counting capability of the hw and is enabled via setting the **ENABLE_FREQ_COUNTING** definition to 1 in the demo_config.h file. In this case **TIMER1** is used as a frequency counter and measures the frequency which is applied on a software specified pin.
3. **Pulse measurement** exposes the capturing capability of the hw and is enabled via setting the **ENABLE_PULSE_MEASURING** definition to 1 in the demo_config.h file. In this case **TIMER1** is used to measure the length of a low or a high pulse applied on a specified pin. 

## Introduction

The DA14531 product family incorporates three HW timer blocks Timer0, Timer1, Timer2. From the affordmentioned timers only Timer1 is capable of running while the device is in sleep mode since its physicaly located in a seperate power domain. Thus timer 1 can be used as a wake up source while the device is in extended or deep sleep.  

**Timer 1** is an 11-bit timer being able to count up or down. It supports Counting and Input Capturing mode and use as clock source either the **System Clock (sys_clk)** or **Low Power Clock (lp_clk)**. Off course when the device is in sleep mode and the timer should be kept active the LP clock source should be selected.

![timer1_block_diagram](assets/timer1_block_diagram.PNG)

</center>- Refer to the following application note for [DA14531 known hardware limitations](https://www.dialog-semiconductor.com/da14531_HW_Limitation  "known hardware limitations"). 
- Dialog Software [Forum link](https://www.dialog-semiconductor.com/forum).

For getting more information about the HW Timers on DA14531, please refer to the [datasheet](https://www.dialog-semiconductor.com/da14531_datasheet).
 
***Useful Note*** :    

    Timer0 and Timer2 examples are provided within the SDK. The projects are located under the following SDK paths:

    - Timer0 example: <sdk_root_directory>\projects\target_apps\peripheral_examples\timer0.
    - Timer2 example: <sdk_root_directory>\projects\target_apps\peripheral_examples\timer2.


## Timer 1

Timer1 is powerd by the **PD_TIM power domain** which can be kept powered even if the system enters sleep mode. It can operate either in **count up** or **count down** mode, generate an interrupt when the timer reaches the **max/min value** or the **threshold**. As mentioned in the **Example description**, three different functionalities are demonstrated in this software example:

1. **Timer Functionality**: In this example the DA14531 device is in sleep mode and **TIMER1** HW is running in the background. When the timer overflows it will generate an interrupt and wake up the device.

2. **Capturing Functionality**: In this sw configuration the sw exposes the capture capabilities of the timer by measuring in microseconds the amount of time a GPIO is forced low or high. When starting up the sw will check the status of the assigned GPIO (by default P11). The current status of the line will be assigned as the idle state of the line. Then the sw will monitor the GPIO via timer1. Upon a rising or a falling edge timer1 will capture the current value of the timer and will wake up on every overflow interrupt in order to count the overflows (if any) until the GPIO is released and returned to its idle state. As soon as the GPIO is released and returns to its idle state the corresponding falling or rising edge will also be captured and the sw will report the diff between the two events on the next timer1 overflow interrupt.

    ***Usefull Note***:

    The demo is using the SW2 button for toggling the line high or low, due to the bouncing of the switch miscounts may occur, especially if the system clock is used as a clock source for timer1. In 


3. **Counter Functionality**: In the sw configuration 

## Software Configuration

- This example requires:
  - SDK6.0.12 or later
  - **SEGGER’s J-Link** tools should be downloaded and installed.

## Hardware Configuration

- This example runs on the DA14531 Bluetooth Smart SoC device.
- The ***DA145xx Pro Development Kit*** is needed for this example.
- Connect the ***DA145xx Pro Development Kit***  to the host computer via the USB connector. 
- Use a Logic Analyzer to verify the example (optional)
- In case of **Counting Mode**, jumpers should be placed on default setup.
- In case of **Capturing Mode**, jumpers should be placed on default setup, plus extra wiring from J2.27 to J2.21.  


![hw_setup](assets/hw_setup.PNG)

## How to run the example

### Initial Setup

- For the initial setup, please refer to [this section](https://www.dialog-semiconductor.com/sites/default/files/sw-example-da145x-example-setup.pdf).

- For the DA14531 Getting started guide you can refer to this [link](https://www.dialog-semiconductor.com/da14531-getting-started).

### Compile & Run

- Νavigate to the `project_environment` folder and open the `Keil` project.
- Compile and launch the example. You should download the firmware either into System-RAM or SPI Flash through the SPI Flash programmer of the SmartSnippets Toolbox. When booting from SPI Flash, jumpers should be placed on the standard SPI flash setup.

### Macro Definitions

In the table below, the user macro definitions are illustrated:

<table>
<thead>
  <tr class="header">
  <th style="text-align: left;">Macro</th>
  <th style="text-align: left;">Description</th>
  </tr>
</thead>
<tbody>
  <tr class="odd">
  <td style="text-align: left;">COUNTING_MODE</td>
  <td style="text-align: left;">Counting Mode of Timer1. Toggling <i><b>LED GPIO (Default Configuration)</b></i></td>
  </tr>
  <tr class="even">
  <td style="text-align: left;">CAPTURING_MODE</td>
  <td style="text-align: left;">Capturing Mode of Timer1. Supply PWM input source to <i><b>CAPTURE GPIO</b></i></td>
  </tr>
  <tr class="even">
  <td style="text-align: left;">TIMER0_SOURCE</td>
  <td style="text-align: left;">In case of Capturing Mode, if TIMER0_SOURCE is defined, PWM from SW Timer0 should be applied on CAPTURE GPIO.	Otherwise, PWM from an external source should be applied on CAPTURE GPIO. </td>
  </tr>
  <tr class="odd">
  <td style="text-align: left;">TIMER_ON</td>
  <td style="text-align: left;">Congigure TIMER0_ON_REG register</td>
  </tr>
  <tr class="even">
  <td style="text-align: left;">PWM_HIGH</td>
  <td style="text-align: left;">Congigure TIMER0_RELOAD_M_REG register</td>
  </tr>
  <tr class="even">
  <td style="text-align: left;">PWM_LOW</td>
  <td style="text-align: left;">Congigure TIMER0_RELOAD_N_REG register   </td>
  </tr>
    <tr class="even">
  <td style="text-align: left;">COUNTER_RELOAD_VALUE</td>
  <td style="text-align: left;">Define Timer0 reload value</td>
  </tr>
</tbody>
</table>

### GPIOs Definitions
In this section, GPIOs definitions are demonstrated as defined in `user_periph_setup.h` header file.

<table>
<thead>
  <tr class="header">
  <th style="text-align: left;">Name</th>
  <th style="text-align: left;">DA14531</th>
  <th style="text-align: left;">DA145xx DK</th>
  <th style="text-align: left;">Description</th>
  </tr>
</thead>
<tbody>
  <tr class="odd">
  <td style="text-align: left;">LED</td>
  <td style="text-align: left;">P0_9</td>
  <td style="text-align: left;">J2.29</td>
  <td style="text-align: left;">Toggling LED in Counting Mode configuration</td>
  </tr>
  <tr class="even">
  <td style="text-align: left;">CAPTURE</td>
  <td style="text-align: left;">P0_1</td>
  <td style="text-align: left;">J2.21</td>
  <td style="text-align: left;">Supply a PWM in Capturing Mode configuration</td>
  </tr>
  <tr class="even">
  <td style="text-align: left;">PWM0</td>
  <td style="text-align: left;">P0_7</td>
  <td style="text-align: left;">J2.27</td>
  <td style="text-align: left;">PWM0 output from Timer0</td>
  </tr>
  <tr class="odd">
  <td style="text-align: left;">PWM1</td>
  <td style="text-align: left;">P0_8</td>
  <td style="text-align: left;">J2.28</td>
  <td style="text-align: left;">PWM1 output from Timer0</td>
  </tr>
</tbody>
</table>

### Counting Mode

1. Use the following macro configuration in `user_timer1.h` in order to run the example in Counting mode:

```c
#define   COUNTING_MODE
#undef    CAPTURING_MODE
```

2. The `timer1_general_user_callback_function()` callback function is executed upon Timer1 interrupt. The on board LED is toggling.

### Capturing Mode with Timer0 PWM Input Source

1. Use the following macro configuration in `user_timer1.h` for Capturing a PWM input source from Timer1 applied on **CAPTURE GPIO**.

```c
#undef        COUNTING_MODE
#define       CAPTURING_MODE
#define       TIMER0_SOURCE
```

2. Timer0 default PWM configurations are shown below. Configuration can be changed according to application requirements.

```c
#define     TIMER_ON        1000
#define     PWM_HIGH        500
#define     PWM_LOW         500
```

3. Define the Timer0 reload value. Default value is 200.

```c
#define TIMER0_EXPIRATION_COUNTER_RELOAD_VALUE     200
```

4. An interrupt is generated on every rising edge of the Timer0 PWM signal and the `timer1_event1_user_callback_function()` callback function is used for handling timer's interrupt The on board LED is toggling following the **positive edge** of the input pulse.

![timer1_capturing_cb](assets/timer1_capturing_cb.PNG)


### Capturing Mode with External PWM Input Source

1. **An external PWM source** can be used to generate a PWM pulse, applied on **CAPTURE GPIO**, instead of using Timer0. Timer1 captures the input PWM pulse following the rising edge of the pulse.

2. Following macro configuration in `user_timer1.h` should be used:

```c
#undef        COUNTING_MODE
#define       CAPTURING_MODE
#undef        TIMER0_SOURCE
```

## Known Limitations

- Refer to the following application note for [DA14531 known hardware limitations](https://www.dialog-semiconductor.com/da14531_HW_Limitation  "known hardware limitations"). 
- Dialog Software [Forum link](https://www.dialog-semiconductor.com/forum).

## License

------

 Copyright (c) 2019 Dialog Semiconductor. All rights reserved.

 This software ("Software") is owned by Dialog Semiconductor. By using this Software
 you agree that Dialog Semiconductor retains all intellectual property and proprietary
 rights in and to this Software and any use, reproduction, disclosure or distribution
 of the Software without express written permission or a license agreement from Dialog
 Semiconductor is strictly prohibited. This Software is solely for use on or in
 conjunction with Dialog Semiconductor products.

 EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.

------