/*
  spindle_control.c - spindle control methods
  Part of Grbl

  Copyright (c) 2020-2021 Bengt Marten Agren
  Copyright (c) 2012-2017 Sungeun K. Jeon for Gnea Research LLC
  Copyright (c) 2009-2011 Simen Svale Skogsrud

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

extern "C"
{
#include "grbl.h"
}
#include "board.h"
#include <timer.h>

using spindle_timer = tim_t<SPINDLE_TIMER_NO>;
using spindle_pwm = spindle_timer::pwm<SPINDLE_TIMER_CH, SPINDLE_PWM>;
using spindle_enable = output_t<SPINDLE_ENABLE>;
using spindle_direction = output_t<SPINDLE_DIRECTION>;


#ifdef VARIABLE_SPINDLE
  static float pwm_gradient; // Precalulated value to speed up rpm to PWM conversions.
  static uint16_t max_duty;
#endif


void spindle_init()
{
  #ifdef VARIABLE_SPINDLE
    // Configure variable spindle PWM and enable pin, if requried. On the Uno, PWM and enable are
    // combined unless configured otherwise.
    const auto f_pwm = 7800; // Hz
    const auto f_clk = spindle_timer::clock();
    const auto psc = 0;

    max_duty = f_clk / (f_pwm * (psc + 1)) - 1;
    spindle_timer::setup(psc, max_duty);
    spindle_pwm::setup();
    pwm_gradient = SPINDLE_PWM_RANGE/(settings.rpm_max-settings.rpm_min);
 
    #ifdef USE_SPINDLE_DIR_AS_ENABLE_PIN
      static_assert(false, "no need for USE_SPINDLE_DIR_AS_ENABLE_PIN");
    #endif
  #else
  #endif
  spindle_enable::setup();
  spindle_direction::setup();
  spindle_stop();
}


uint8_t spindle_get_state()
{
  #ifdef VARIABLE_SPINDLE
    if (spindle_pwm::duty() > 0) { // Check if PWM is enabled.
        if (spindle_direction::read())
            return SPINDLE_STATE_CCW;
        else
            return SPINDLE_STATE_CW;
    }
  #else
    }
    #ifdef INVERT_SPINDLE_ENABLE_PIN
      if (!spindle_enable::read()) {
    #else
      if (spindle_enable::read()) {
    #endif
        if (spindle_direction::read())
            return SPINDLE_STATE_CCW;
        else
            return SPINDLE_STATE_CW;
      }
  #endif
  return(SPINDLE_STATE_DISABLE);
}


// Disables the spindle and sets PWM output to zero when PWM variable spindle speed is enabled.
// Called by various main program and ISR routines. Keep routine small, fast, and efficient.
// Called by spindle_init(), spindle_set_speed(), spindle_set_state(), and mc_reset().
void spindle_stop()
{
  #ifdef VARIABLE_SPINDLE
    spindle_pwm::duty(0);
  #endif
  #ifdef INVERT_SPINDLE_ENABLE_PIN
    spindle_enable::set();
  #else
    spindle_enable::clear();
  #endif
}


#ifdef VARIABLE_SPINDLE
  // Sets spindle speed PWM output and enable pin, if configured. Called by spindle_set_state()
  // and stepper ISR. Keep routine small and efficient.
  void spindle_set_speed(uint8_t pwm_value)
  {
    spindle_pwm::duty(pwm_value * max_duty / 255); // Set PWM output level.
    if (pwm_value == SPINDLE_PWM_OFF_VALUE) {
      spindle_stop();
    } else {
      #ifdef INVERT_SPINDLE_ENABLE_PIN
        spindle_enable::clear();
      #else
        spindle_enable::set();
      #endif
    }
  }


  #ifdef ENABLE_PIECEWISE_LINEAR_SPINDLE
  
    // Called by spindle_set_state() and step segment generator. Keep routine small and efficient.
    uint8_t spindle_compute_pwm_value(float rpm) // 328p PWM register is 8-bit.
    {
      uint8_t pwm_value;
      rpm *= (0.010*sys.spindle_speed_ovr); // Scale by spindle speed override value.
      // Calculate PWM register value based on rpm max/min settings and programmed rpm.
      if ((settings.rpm_min >= settings.rpm_max) || (rpm >= RPM_MAX)) {
        rpm = RPM_MAX;
        pwm_value = SPINDLE_PWM_MAX_VALUE;
      } else if (rpm <= RPM_MIN) {
        if (rpm == 0.0) { // S0 disables spindle
          pwm_value = SPINDLE_PWM_OFF_VALUE;
        } else {
          rpm = RPM_MIN;
          pwm_value = SPINDLE_PWM_MIN_VALUE;
        }
      } else {
        // Compute intermediate PWM value with linear spindle speed model via piecewise linear fit model.
        #if (N_PIECES > 3)
          if (rpm > RPM_POINT34) {
            pwm_value = floor(RPM_LINE_A4*rpm - RPM_LINE_B4);
          } else 
        #endif
        #if (N_PIECES > 2)
          if (rpm > RPM_POINT23) {
            pwm_value = floor(RPM_LINE_A3*rpm - RPM_LINE_B3);
          } else 
        #endif
        #if (N_PIECES > 1)
          if (rpm > RPM_POINT12) {
            pwm_value = floor(RPM_LINE_A2*rpm - RPM_LINE_B2);
          } else 
        #endif
        {
          pwm_value = floor(RPM_LINE_A1*rpm - RPM_LINE_B1);
        }
      }
      sys.spindle_speed = rpm;
      return(pwm_value);
    }
    
  #else 
  
    // Called by spindle_set_state() and step segment generator. Keep routine small and efficient.
    uint8_t spindle_compute_pwm_value(float rpm) // 328p PWM register is 8-bit.
    {
      uint8_t pwm_value;
      rpm *= (0.010*sys.spindle_speed_ovr); // Scale by spindle speed override value.
      // Calculate PWM register value based on rpm max/min settings and programmed rpm.
      if ((settings.rpm_min >= settings.rpm_max) || (rpm >= settings.rpm_max)) {
        // No PWM range possible. Set simple on/off spindle control pin state.
        sys.spindle_speed = settings.rpm_max;
        pwm_value = SPINDLE_PWM_MAX_VALUE;
      } else if (rpm <= settings.rpm_min) {
        if (rpm == 0.0) { // S0 disables spindle
          sys.spindle_speed = 0.0;
          pwm_value = SPINDLE_PWM_OFF_VALUE;
        } else { // Set minimum PWM output
          sys.spindle_speed = settings.rpm_min;
          pwm_value = SPINDLE_PWM_MIN_VALUE;
        }
      } else { 
        // Compute intermediate PWM value with linear spindle speed model.
        // NOTE: A nonlinear model could be installed here, if required, but keep it VERY light-weight.
        sys.spindle_speed = rpm;
        pwm_value = floor((rpm-settings.rpm_min)*pwm_gradient) + SPINDLE_PWM_MIN_VALUE;
      }
      return(pwm_value);
    }
    
  #endif
#endif


// Immediately sets spindle running state with direction and spindle rpm via PWM, if enabled.
// Called by g-code parser spindle_sync(), parking retract and restore, g-code program end,
// sleep, and spindle stop override.
#ifdef VARIABLE_SPINDLE
  void spindle_set_state(uint8_t state, float rpm)
#else
  void _spindle_set_state(uint8_t state)
#endif
{
  if (sys.abort) { return; } // Block during abort.

  if (state == SPINDLE_DISABLE) { // Halt or set spindle direction and rpm.
  
    #ifdef VARIABLE_SPINDLE
      sys.spindle_speed = 0.0;
    #endif
    spindle_stop();
  
  } else {
    #ifdef VARIABLE_SPINDLE
      // NOTE: Assumes all calls to this function is when Grbl is not moving or must remain off.
      if (settings.flags & BITFLAG_LASER_MODE) { 
        if (state == SPINDLE_ENABLE_CCW) { rpm = 0.0; } // TODO: May need to be rpm_min*(100/MAX_SPINDLE_SPEED_OVERRIDE);
      }
      spindle_set_speed(spindle_compute_pwm_value(rpm));
    #endif
      // NOTE: Without variable spindle, the enable bit should just turn on or off, regardless
      // if the spindle speed value is zero, as its ignored anyhow.
    #ifdef INVERT_SPINDLE_ENABLE_PIN
        spindle_enable::clear();
    #else
        spindle_enable::set();
    #endif    
  }
  
  sys.report_ovr_counter = 0; // Set to report change immediately
}


// G-code parser entry-point for setting spindle state. Forces a planner buffer sync and bails 
// if an abort or check-mode is active.
#ifdef VARIABLE_SPINDLE
  void spindle_sync(uint8_t state, float rpm)
  {
    if (sys.state == STATE_CHECK_MODE) { return; }
    protocol_buffer_synchronize(); // Empty planner buffer to ensure spindle is set when programmed.
    spindle_set_state(state,rpm);
  }
#else
  void _spindle_sync(uint8_t state)
  {
    if (sys.state == STATE_CHECK_MODE) { return; }
    protocol_buffer_synchronize(); // Empty planner buffer to ensure spindle is set when programmed.
    _spindle_set_state(state);
  }
#endif
