## Morse Code Project - `gpiointerrupt.c`

### Project Overview
This project implements a Morse code simulator on a Texas Instruments CC3220x LaunchPad. The system uses LEDs to represent Morse code signals, with one LED indicating a dot and another indicating a dash. The program allows users to toggle between two different Morse code messages (e.g., "SOS" and "OK") using a button on the board. The project showcases skills in embedded systems, specifically in utilizing GPIO, timers, and interrupts.

### What Did I Do Well?
I successfully implemented a state machine to control the timing and sequencing of the Morse code signals. The use of interrupts to handle button presses was effective and ensured that the system could switch between messages without missing a beat.

### Where Could I Improve?
The readability and maintainability of the code could be improved by breaking down some of the larger functions into smaller, more focused functions. Additionally, more robust error handling could be added to handle edge cases, such as button debouncing.

### Tools and Resources Added to Support Network
For this project, I made extensive use of the TI Code Composer Studio and the TI documentation for the CC3220x LaunchPad. These tools will be valuable resources for future embedded systems projects. Additionally, the experience gained in using GPIO, timers, and interrupts is a key addition to my skill set.

### Transferable Skills
The skills developed in this project, particularly in state machine design, use of hardware interrupts, and timer configuration, are highly transferable to other embedded systems projects. These skills are essential for creating responsive and efficient systems.

### How Did I Make the Project Maintainable, Readable, and Adaptable?
I made the project maintainable by organizing the code into clear sections, with each part of the system (GPIO setup, timer setup, state machine, etc.) clearly delineated. Comments were added throughout the code to explain the purpose of each section and critical lines. The code was kept adaptable by using variables and macros for key parameters, making it easy to adjust the timing or LED behavior without needing to rewrite significant portions of the code.
