/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== gpiointerrupt.c ========
 *  This file contains the implementation of the Smart Thermostat project.
 *  It includes reading the room temperature, controlling an LED to simulate
 *  a heating system, and sending temperature data via UART. Button interrupts
 *  are used to adjust the temperature set-point.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>  // For snprintf()
#include <string.h> // For strlen()
#include <ti/drivers/Timer.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/GPIO.h>
#include "ti_drivers_config.h"

/* Global Variables */
volatile int setPoint = 25;  /* Set-point temperature (default 25°C) */
volatile int roomTemperature = 0;  /* Room temperature */
volatile unsigned int timeCounter = 0;  /* Seconds since reset */
volatile unsigned char TimerFlag = 0;  /* Timer flag */

/* UART and I2C Handles */
UART_Handle uart;
I2C_Handle i2c;

/* I2C Configuration */
static const struct {
    uint8_t address;
    uint8_t resultReg;
    char *id;
} sensors[3] = {
    { 0x48, 0x0000, "11X" },
    { 0x49, 0x0000, "116" },
    { 0x41, 0x0001, "006" }
};

/*
 *  ======== timerCallback ========
 *  Timer callback function
 *  This function is called at each timer interrupt to update the state machine
 *  and control the LED blinking according to the Morse code pattern.
 */
void timerCallback(Timer_Handle myHandle, int_fast16_t status) {
    TimerFlag = 1;  /* Set timer flag */
    timeCounter++;  /* Increment time counter */
}

/*
 *  ======== initTimer ========
 *  Function to initialize and start the timer
 *  This function sets up the timer with the specified parameters and starts it.
 */
void initTimer(void) {
    Timer_Handle timer0;
    Timer_Params params;

    Timer_init();
    Timer_Params_init(&params);
    params.period = 1000000;  /* 1 second period */
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    timer0 = Timer_open(CONFIG_TIMER_0, &params);
    if (timer0 == NULL) {
        while (1) {}
    }

    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
        while (1) {}
    }
}

/*
 *  ======== initUART ========
 *  Initialize UART for communication
 */
void initUART(void) {
    UART_Params uartParams;

    UART_init();
    UART_Params_init(&uartParams);
    uartParams.baudRate = 115200;

    uart = UART_open(CONFIG_UART_0, &uartParams);
    if (uart == NULL) {
        while (1) {}
    }
}

/*
 *  ======== initI2C ========
 *  Initialize I2C for temperature sensor
 */
void initI2C(void) {
    int8_t i, found;
    I2C_Params i2cParams;
    I2C_Transaction i2cTransaction;
    char output[64]; /* Buffer for output messages */
    uint8_t txBuffer[1];

    snprintf(output, 64, "Initializing I2C Driver - ");
    UART_write(uart, output, strlen(output));

    I2C_init();
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;

    i2c = I2C_open(CONFIG_I2C_0, &i2cParams);
    if (i2c == NULL) {
        snprintf(output, 64, "Failed\n\r");
        UART_write(uart, output, strlen(output));
        while (1);
    }

    snprintf(output, 64, "Passed\n\r");
    UART_write(uart, output, strlen(output));

    found = false;
    for (i = 0; i < 3; ++i) {
        i2cTransaction.slaveAddress = sensors[i].address;
        txBuffer[0] = sensors[i].resultReg;
        i2cTransaction.writeBuf = txBuffer;
        i2cTransaction.writeCount = 1;
        i2cTransaction.readBuf = NULL;
        i2cTransaction.readCount = 0;

        snprintf(output, 64, "Is this %s? ", sensors[i].id);
        UART_write(uart, output, strlen(output));
        if (I2C_transfer(i2c, &i2cTransaction)) {
            snprintf(output, 64, "Found\n\r");
            UART_write(uart, output, strlen(output));
            found = true;
            break;
        }
        snprintf(output, 64, "No\n\r");
        UART_write(uart, output, strlen(output));
    }

    if (found) {
        snprintf(output, 64, "Detected TMP Sensor\n\r");
        UART_write(uart, output, strlen(output));
    } else {
        snprintf(output, 64, "Temperature sensor not found\n\r");
        UART_write(uart, output, strlen(output));
    }
}

/*
 *  ======== readTemp ========
 *  Read temperature from the TMP006 sensor via I2C.
 *  Returns the temperature value in degrees Celsius.
 */
int16_t readTemp(void) {
    int16_t temperature = 0;

    I2C_Transaction i2cTransaction;
    uint8_t txBuffer[1];
    uint8_t rxBuffer[2];

    i2cTransaction.slaveAddress = 0x41;  /* TMP006 I2C address */
    txBuffer[0] = 0x01;  /* TMP006 register for temperature reading */
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 2;

    if (I2C_transfer(i2c, &i2cTransaction)) {
        temperature = (rxBuffer[0] << 8) | (rxBuffer[1]);
        temperature *= 0.0078125;  /* Convert raw data to temperature value */

        if (rxBuffer[0] & 0x80) {
            temperature |= 0xF000;  /* Adjust for negative temperatures */
        }
    } else {
        UART_write(uart, "Error reading temperature sensor\n\r", 34);
    }

    return temperature;
}


/*
 *  ======== gpioButtonFxn0 ========
 *  GPIO button interrupt callback function.
 *  This function is triggered when SW2 is pressed and decreases the set-point temperature.
 */
void gpioButtonFxn0(uint_least8_t index) {
    setPoint--;  /* Decrease the set-point temperature by 1 degree */
}

/*
 *  ======== gpioButtonFxn1 ========
 *  GPIO button interrupt callback function.
 *  This function is triggered when SW4 is pressed and increases the set-point temperature.
 */
void gpioButtonFxn1(uint_least8_t index) {
    setPoint++;  /* Increase the set-point temperature by 1 degree */
}


/*
 *  ======== mainThread ========
 *  Main function where the program execution begins.
 *  Initializes all peripherals and enters the main loop to execute thermostat logic.
 */
void *mainThread(void *arg0) {
    GPIO_init();  /* Initialize GPIO */
    Timer_init();  /* Initialize Timer */
    initUART();  /* Initialize UART for data communication */
    initI2C();   /* Initialize I2C for temperature sensor */
    initTimer(); /* Initialize Timer for 1-second intervals */

    /* Configure GPIO pins */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);  /* Configure LED pin as output */
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);  /* Configure SW2 as input with pull-up resistor */
    GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);  /* Configure SW4 as input with pull-up resistor */

    /* Enable button interrupts */
    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, gpioButtonFxn0);  /* Set SW2 callback function */
    GPIO_enableInt(CONFIG_GPIO_BUTTON_0);  /* Enable interrupts for SW2 */
    GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonFxn1);  /* Set SW4 callback function */
    GPIO_enableInt(CONFIG_GPIO_BUTTON_1);  /* Enable interrupts for SW4 */

    /* Main loop - Executes every 1 second based on TimerFlag */
    while (1) {
        if (TimerFlag) {
            TimerFlag = 0;  /* Clear TimerFlag */

            /* Read temperature from the TMP006 sensor */
            roomTemperature = readTemp();

            /* Control LED based on temperature comparison */
            if (roomTemperature < setPoint) {
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);  /* Turn ON LED (Heater ON) */
            } else {
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF); /* Turn OFF LED (Heater OFF) */
            }

            /* Send data to UART - Format: <RoomTemp,SetPoint,HeaterStatus,TimeCounter> */
            char output[64];
            snprintf(output, sizeof(output), "<%02d,%02d,%d,%04d>\n",
                     roomTemperature, setPoint, GPIO_read(CONFIG_GPIO_LED_0), timeCounter);
            UART_write(uart, output, strlen(output));  /* Transmit data via UART */
        }
    }
}
