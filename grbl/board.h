/*
  board.h - pin and peripheral assignments
  Part of Jird

  Copyright (c) 2020-2021 Bengt Marten Agren

  Jird is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Jird is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Jird.  If not, see <http://www.gnu.org/licenses/>.
*/

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

static const int            STEP_TIMER_NO = 2;
static const interrupt_t    STEP_TIMER_ISR = interrupt::TIM2;

static const int            RESET_TIMER_NO = 15;
static const interrupt_t    RESET_TIMER_ISR = interrupt::TIM1_BRK_TIM15;

static const pin_t          X_STEP              = PB1;
static const pin_t          Y_STEP              = PB2;
static const pin_t          Z_STEP              = PB3;

static const pin_t          X_DIRECTION         = PB4;
static const pin_t          Y_DIRECTION         = PB5;
static const pin_t          Z_DIRECTION         = PB6;

static const pin_t          STEPPERS_DISABLE    = PB0;

static const pin_t          X_LIMIT             = PA6;
static const pin_t          Y_LIMIT             = PA7;
static const pin_t          Z_LIMIT             = PA8;
static const interrupt_t    LIMIT_ISR           = interrupt::EXTI9_5;

static const pin_t          CONTROL_RESET       = PA10;
static const pin_t          CONTROL_FEED_HOLD   = PA11;
static const pin_t          CONTROL_CYCLE_START = PA12;
static const pin_t          CONTROL_SAFETY_DOOR = PA13;
static const interrupt_t    CONTROL_ISR         = interrupt::EXTI15_10;

static const pin_t          PROBE               = PC10;
static const pin_t          COOLANT_FLOOD       = PC11;
static const pin_t          COOLANT_MIST        = PC12;

static const int            SPINDLE_TIMER_NO    = 3;
static const channel_t      SPINDLE_TIMER_CH    = CH3;

static const pin_t          SPINDLE_PWM         = PC8;
static const pin_t          SPINDLE_ENABLE      = PA1;
static const pin_t          SPINDLE_DIRECTION   = PA4;

