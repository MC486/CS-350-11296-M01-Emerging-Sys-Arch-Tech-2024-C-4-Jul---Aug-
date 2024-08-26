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
 */
#include <stdint.h>
#include <stddef.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/GPIO.h>
#include "ti_drivers_config.h"

/* Define the state machine states */
typedef enum {STATE_DOT, STATE_DASH, STATE_INTER_CHAR, STATE_INTER_WORD} State;
State currentState = STATE_DOT;

/* Define Morse code messages */
char* morseMessageSOS = "...---...";
char* morseMessageOK = "---.-";
char* currentMessage = "...---...";
int currentIndex = 0;
uint32_t timerPeriod = 500000;
volatile int messageChangePending = 0;  /* Flag to indicate pending message change */


/*
 *  ======== timerCallback ========
 *  Timer callback function
 *  This function is called at each timer interrupt to update the state machine
 *  and control the LED blinking according to the Morse code pattern.
 */
void timerCallback(Timer_Handle myHandle, int_fast16_t status) {

    if (currentState == STATE_INTER_WORD && messageChangePending) {
        // Toggle the Morse code message
        currentMessage = (currentMessage == morseMessageSOS) ? morseMessageOK : morseMessageSOS;
        currentIndex = 0;  /* Reset the index */
        messageChangePending = 0;  /* Clear the pending flag */
        GPIO_toggle(CONFIG_GPIO_LED_1);  /* Toggle the green LED to indicate a change */
        printf("Current message: %s\n", currentMessage);  /* Print the current message for debugging */
    }

    switch (currentState) {
        case STATE_DOT:
            GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);   /* Turn on red LED for dot */
            GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);  /* Ensure green LED is off */
            currentState = STATE_INTER_CHAR;
            timerPeriod = 500000; /* Dot duration */
            break;
        case STATE_DASH:
            GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);  /* Ensure red LED is off */
            GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_ON);   /* Turn on green LED for dash */
            currentState = STATE_INTER_CHAR;
            timerPeriod = 1500000; /* Dash duration */
            break;
        case STATE_INTER_CHAR:
            GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);  /* Turn off red LED */
            GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);  /* Turn off green LED */
            currentIndex++;
            if (currentMessage[currentIndex] == '\0') {  /* If end of message */
                currentState = STATE_INTER_WORD;
                timerPeriod = 3500000; /* Inter-word gap duration */
            } else { /* More characters in message */
                currentState = (currentMessage[currentIndex] == '.') ? STATE_DOT : STATE_DASH;
                timerPeriod = 500000; /* Standard duration */
            }
            break;
        case STATE_INTER_WORD:
            GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);  /* Turn off red LED */
            GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);  /* Turn off green LED */
            currentIndex = 0;  /* Reset to start of message */
            currentState = (currentMessage[currentIndex] == '.') ? STATE_DOT : STATE_DASH;
            timerPeriod = 500000; /* Standard duration */
            break;
    }
    Timer_setPeriod(myHandle, Timer_PERIOD_US, timerPeriod); /* Update the timer period */
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
    params.period = timerPeriod;
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
 *  ======== gpioButtonFxn0 ========
 *  GPIO button interrupt callback function
 *  This function is called when the button is pressed and toggles the Morse code message
 *  only after the current message is complete.
 */
void gpioButtonFxn1(uint_least8_t index) {
    messageChangePending = 1;  /* Set the pending message change flag */
}



/*
 *  ======== mainThread ========
 *  Main function
 *  This function initializes the GPIO and Timer drivers, configures the pins,
 *  installs the button callback, and starts the timer.
 */
void *mainThread(void *arg0) {
    GPIO_init();
    Timer_init();

    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_LED_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
    GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);

    GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonFxn1);
    GPIO_enableInt(CONFIG_GPIO_BUTTON_1);

    initTimer();

    while (1) {
        /* Main loop */
    }
}
