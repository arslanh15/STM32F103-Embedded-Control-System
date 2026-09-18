# STM32F103-Embedded-Control-System

A modular embedded control project developed for the STM32F103 using C, STM32 HAL, and PlatformIO.

The project integrates an MPU6500 IMU, ILI9341 TFT display, touch input, ADC, UART, I2C, SPI, and PWM-based servo control in a single embedded application.

## Features

- STM32F103 microcontroller
- STM32 HAL-based peripheral configuration
- MPU6500 communication over I2C
- Accelerometer and gyroscope data acquisition
- Accelerometer and gyroscope calibration
- Roll estimation using a complementary filter
- PWM-based servo motor control
- ADC input acquisition
- UART communication and debugging
- ILI9341 TFT display communication over SPI
- Touch input support
- Modular `.c` / `.h` driver structure
- PlatformIO development environment

## System Overview

The project combines several embedded subsystems in a single application.

```text
                     +----------------+
                     |   STM32F103    |
                     +-------+--------+
                             |
          +------------------+------------------+
          |                  |                  |
         I2C                SPI                ADC
          |                  |                  |
     +----+----+        +----+----+       Analog Input
     | MPU6500 |        | ILI9341 |
     +----+----+        |   TFT   |
          |             +---------+
          |
   Accel + Gyro
          |
   Calibration
          |
 Complementary
     Filter
          |
    Roll Angle
          |
          +--------------------+
                               |
                              PWM
                               |
                         +-----+-----+
                         |   Servo   |
                         +-----------+

Additional interfaces:
- UART for communication/debugging
- Touch input for user interaction
```

## Hardware

The project was developed using:

- STM32F103CBT6
- MPU6500 IMU
- ILI9341 TFT display
- Touch interface
- SG90 servo motor
- Analog input / potentiometer
- USB-to-UART programmer

## Software

- C
- STM32 HAL
- PlatformIO
- STM32Cube framework

## Project Structure

```text
STM32F103-Embedded-Control-System/
│
├── include/
│   ├── adc.h
│   ├── font.h
│   ├── i2c.h
│   ├── ili9341.h
│   ├── mpu6500.h
│   ├── orientation.h
│   ├── servo.h
│   ├── spi.h
│   ├── touch.h
│   └── uart.h
│
├── src/
│   ├── adc.c
│   ├── font.c
│   ├── i2c.c
│   ├── ili9341.c
│   ├── main.c
│   ├── mpu6500.c
│   ├── orientation.c
│   ├── servo.c
│   ├── spi.c
│   ├── touch.c
│   └── uart.c
│
├── lib/
├── test/
├── platformio.ini
└── README.md
```

## Modules

### MPU6500 Driver

The MPU6500 module handles communication with the IMU over I2C.

Main responsibilities include:

- Device initialization
- `WHO_AM_I` verification
- Accelerometer acquisition
- Gyroscope acquisition
- Raw sensor data conversion
- Accelerometer and gyroscope calibration

The sensor is accessed directly through its registers without using an external MPU6500 library.

### Orientation Estimation

The `orientation` module estimates the roll angle using accelerometer and gyroscope measurements.

A complementary filter is used to combine:

- Accelerometer-based angle estimation
- Gyroscope angular-rate integration

Conceptually:

```text
roll =
    alpha * (previous_roll + gyro_rate * dt)
    +
    (1 - alpha) * accelerometer_angle
```

This combines the short-term response of the gyroscope with the long-term reference provided by the accelerometer.

## Servo Control

Servo motors are controlled using timer-generated PWM signals.

The servo module is responsible for:

- PWM initialization
- Pulse-width generation
- Servo position control

The project was tested using SG90 micro servo motors.

## ILI9341 TFT Display

The ILI9341 display is controlled through SPI.

The display module contains functions for operations such as:

- Display initialization
- Filling the screen
- Drawing rectangular regions
- Drawing images
- Drawing scaled images
- Displaying graphical interface elements

A separate font module is used for display-related graphical data.

## Touch Input

The project also contains a touch input module used for interaction with the graphical interface.

Touch coordinates are acquired and processed independently from the display driver.

## ADC

The ADC module provides analog input acquisition using the STM32 ADC peripheral.

It can be used for external analog controls such as a potentiometer.

## UART

UART is used for serial communication and debugging.

It allows internal sensor and system values to be monitored from a serial terminal during development.

## I2C

The I2C module provides the low-level communication functions used by the MPU6500 driver.

Typical operations include:

```c
I2C1_WriteRegister(...);
I2C1_ReadRegister(...);
I2C1_ReadRegisters(...);
```

This keeps the sensor driver separated from the low-level peripheral implementation.

## SPI

The SPI module provides communication used by the TFT and related peripherals.

Keeping SPI communication in a separate module allows the display and touch drivers to remain more modular.

## Main Application

The `main.c` file integrates the individual modules into the final application.

The general execution flow is:

```text
HAL Initialization
        |
Peripheral Initialization
        |
MPU6500 Initialization
        |
IMU Calibration
        |
Display Initialization
        |
Servo Initialization
        |
        v
      Main Loop
        |
        +--> Read IMU
        |
        +--> Estimate Orientation
        |
        +--> Read Analog / Touch Input
        |
        +--> Update Servo Position
        |
        +--> Update Display
```

## Design Approach

The project was intentionally divided into separate modules instead of placing all functionality inside `main.c`.

For example:

```text
mpu6500.c      -> IMU driver
orientation.c  -> orientation estimation
servo.c        -> PWM / servo control
ili9341.c      -> TFT driver
touch.c        -> touch input
adc.c          -> analog acquisition
i2c.c          -> I2C peripheral
spi.c          -> SPI peripheral
uart.c         -> UART communication
```

This structure keeps hardware drivers and application logic separated and makes individual components easier to test, modify, and reuse.

## PlatformIO

The project is built using PlatformIO.

A typical configuration is:

```ini
[env:genericSTM32F103CB]
platform = ststm32
board = genericSTM32F103CB
framework = stm32cube
upload_protocol = serial
```

Build the project using:

```text
PlatformIO: Build
```

and upload it using a supported serial programmer.

## Notes

This repository was created as part of my embedded systems studies and practical development work.

The objective of the project was to gain hands-on experience with:

- STM32 peripheral configuration
- Embedded C
- Sensor communication
- I2C and SPI protocols
- IMU data processing
- PWM generation
- Servo control
- ADC acquisition
- UART communication
- TFT display drivers
- Touch interfaces
- Modular embedded software design

The implementation is intentionally kept readable so that the interaction between the individual peripherals, drivers, and application logic can be followed directly from the source code.
