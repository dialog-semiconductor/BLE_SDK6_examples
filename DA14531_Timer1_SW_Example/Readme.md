------

------

# DA14531 Timer1 Software Example

------

## Example description

This Software Example demonstrates using **TIMER1**. In this demonstration, TIMER1 is configured to Counting and Capturing mode

## Introduction

The DA14531 product family incorporates three identical HW timer blocks. 

1. **Timer 0** is a 16-bit general purpose software programmable timer, which has the ability of generating Pulse Width Modulated signals, namely PWM0 and PWM1.

<center>
 <figure>
  <img src="assets\timer0_block_diagram.png" width="90%" height="90%" >
  <figcaption><i><b>Timer 0 Block Diagram</b></i></figcaption>
</figure>
</center>

2. **Timer 1** is an 11-bit timer being able to count up or down. Is supports Counting and Input Campturing mode and it could be enabled in sleep as the vclock source is selectable betweeen System Clock (sys_clk) and Low Power Clock (lp_clk))

<center>
 <figure>
  <img src="assets\timer1_block_diagram.png" width="90%" height="90%">
  <figcaption><i><b>Timer 1 Block Diagram</b></i></figcaption>
</figure>
</center>

3. **Timer 2** is basically a PWM generator. It has 6 Pulse Width Modulated (PWM) outputs.

<center>
 <figure>
  <img src="assets\timer2_block_diagram.png" width="90%" height="90%">
  <figcaption><i><b>Timer 2 Block Diagram</b></i></figcaption>
</figure>
</center>

For getting more information about the HW Timers on DA14531, please refer datasheet.

## Software Configuration

- This example requires:
  - SDK6.0.12 or later
- **SEGGER’s J-Link** tools should be downloaded and installed.

## Hardware Configuration

- This example runs on the DA14531 Bluetooth Smart SoC device.
- The DA145xx / DA1458x Pro Development kit is needed for this example.
- Connect the USB Development kit to the host computer.
- Logic Analyzer would be very helpful for this demonstration
- In case of **Counting Mode**, jumpers should be placed on default setup
- In case of **Capturing Mode** , jumpers should be placed on default setup, plus extra wiring from P27 to P21 

## How to run the example

### Initial Setup

For initial setup of the example please refer to [this section of the dialog support portal](https://www.dialog-semiconductor.com/sites/default/files/sw-example-da145x-example-setup.pdf).

### Complile & Run

- Νavigate to project_environment folder and open the Keil project
- Compile and launch the example. You could download the firmware either into System-RAM or into SPI Flash with the usage of SPI Flash programmer from SmartSnippets Toolbox. When booting from SPI Flash, jumpers should be placed on on standard SPI flash setup.

### Macro Definitions

In the table below, the user Macro Definition are illustrated:

<table>
  <caption> <b> <i> Macros Definitions</i></b></caption>
<thead>
  <tr class="header">
  <th style="text-align: left;">Macro</th>
  <th style="text-align: left;">Description</th>
  </tr>
</thead>
<tbody>
  <tr class="odd">
  <td style="text-align: left;">COUNTING_MODE</td>
  <td style="text-align: left;">Enabling Counting Mode</td>
  </tr>
  <tr class="even">
  <td style="text-align: left;">CAPTURING_MODE</td>
  <td style="text-align: left;">Enabling CApturing Mode - PWM should be applied on CAPTURE GPIO</td>
  </tr>
  <tr class="even">
  <td style="text-align: left;">TIMER0_SOURCE</td>
  <td style="text-align: left;">Use Timer0 as PWM source. If is undefined, external PWM source should be applied. </td>
  </tr>
  <tr class="odd">
  <td style="text-align: left;">TIMER_ON</td>
  <td style="text-align: left;">Set TIMER0_ON_REG</td>
  </tr>
  <tr class="even">
  <td style="text-align: left;">PWM_HIGH</td>
  <td style="text-align: left;">Set TIMER0_RELOAD_M_REG</td>
  </tr>
  <tr class="even">
  <td style="text-align: left;">PWM_LOW</td>
  <td style="text-align: left;">Set TIMER0_RELOAD_N_REG</td>
  </tr>
    <tr class="even">
  <td style="text-align: left;">COUNTER_RELOAD_VALUE</td>
  <td style="text-align: left;">Set expiration time for Timer0</td>
  </tr>
</tbody>
</table>

### Counting Mode

- Use the following macro configuration in user_timer1.h:

```c
#define   COUNTING_MODE
#undef    CAPTURING_MODE
#undef    TIMER0_SOURCE
```

- The `timer1_general_user_callback_function()` callback function is executed upon each timer interrupt and the on board LED is changing state

### Capturing Mode - Timer0 Source

- Use the following macro configuration in user_timer1.h:

```c
#undef        COUNTING_MODE
#define       CAPTURING_MODE
#define       TIMER0_SOURCE
```

- The default Timer0 PWM configuration are the following. You can chenge them according your application

```c
#define     TIMER_ON        1000
#define     PWM_HIGH        500
#define     PWM_LOW         500
```

- Define the Timer0 expiration reload value. Default value is 200:

```c
#define TIMER0_EXPIRATION_COUNTER_RELOAD_VALUE     200
```

- An interrupt is generated in every positive edge of the Timer0 PWM signal and 
- The `timer1_event1_user_callback_function()` callback function is executed upon each timer interrupt and the on LED is changing state in every **positive edge**.

![timer1_capturing_cb](assets\timer1_capturing_cb.PNG)

### Capturing Mode - External PWM Source

Instead of using Timer0 to generate a PWM, you could also use **an external PWM source** applied on **CAPTURE GPIO**.

- Use the following macro configuration in `user_timer1.h`:

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