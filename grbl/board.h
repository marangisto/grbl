#pragma once

#include <gpio.h>

static const pin_t          LED = PA5;
static const pin_t          BTN = PC13;
static const input_type_t   BTN_TYPE = pull_down;
static const interrupt_t    BTN_ISR = interrupt::EXTI15_10;

static const int            SERIAL_USART = 2;
static const pin_t          SERIAL_TX = PA2;
static const pin_t          SERIAL_RX = PA3;
static const interrupt_t    SERIAL_ISR = interrupt::USART2;

static const int            TIMER_NO = 3;
static const interrupt_t    TIMER_ISR = interrupt::TIM3;

static const pin_t          X_STEP              = PB10;
static const pin_t          Y_STEP              = PB4;
static const pin_t          Z_STEP              = PB5;

static const pin_t          X_DIRECTION         = PB15;
static const pin_t          Y_DIRECTION         = PB14;
static const pin_t          Z_DIRECTION         = PB13;

static const pin_t          STEPPERS_DISABLE    = PB1;

static const pin_t          X_LIMIT             = PA6;
static const pin_t          Y_LIMIT             = PA7;
static const pin_t          Z_LIMIT             = PA9;
static const interrupt_t    LIMIT_ISR           = interrupt::EXTI9_5;

static const pin_t          CONTROL_RESET       = PC10;
static const pin_t          CONTROL_FEED_HOLD   = PC12;
static const pin_t          CONTROL_CYCLE_START = PC14;
static const pin_t          CONTROL_SAFETY_DOOR = PC15;
static const interrupt_t    CONTROL_ISR         = interrupt::EXTI15_10;

static const pin_t          PROBE               = PC11;

