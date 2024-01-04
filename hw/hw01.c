/**
 * @file hw01.c
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-09-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "hw01.h"
#include "main.h"

/*****************************************************************************/
/*  Function Declaration                                                     */
/*****************************************************************************/

/*****************************************************************************/
/*  Global Variables                                                         */
/*****************************************************************************/

// Macro for 100 ms 
#define TICKS_MS_100 10000000

// Macro for which state we're in
#define STATE_STM 0
#define STATE_RM 1
#define STATE_SAM 2

// Macro for whether to change the second digit or minute digit
#define CHANGE_SEC 0
#define CHANGE_MIN 1

// Macro for whether the alarm is armed or not
#define ALARM_DISARMED 0
#define ALARM_ARMED 1

// Macro for whether the alarm is enabled or not
#define ALARM_DISABLED 0
#define ALARM_ENABLED 1

// Macro for whether the clock has gone through set alarm mode
// Currently unused
#define NOT_SEEN_SAM 0
#define SEEN_SAM 1


/**
 * @brief
 * Initializes the PSoC6 Peripherals used for HW01
 */
void hw01_peripheral_init(void)
{
    /* Initialize the pushbuttons */
    push_buttons_init();

    /* Initalize the LEDs */
    leds_init();

    /* Initialize the LCD */
    ece353_enable_lcd();

    /* Initialize the buzzer */
    pwm_buzzer_init();

    /* Intitialize a timer to generate an interrupt every 100mS*/
    hw01_timer_init(TICKS_MS_100); /* MODIFY with correct Ticks Count for 100mS*/
}



/**
 * @brief
 * Implements the main application for HW01
 */
void hw01_main_app(void)
{
    // Minute and second on the actual clock
    uint8_t minute = 0;
    uint8_t second = 0;

    // Minute and second for the alarm
    uint8_t alarm_minute = 0;
    uint8_t alarm_second = 0;

    // Tells us if the clock has been into set alarm mode or not
    // If not, don't let us turn the alarm on or off
    // Unused unless it is a desired feature
    uint8_t has_seen_sam = NOT_SEEN_SAM;

    hw01_display_time(minute, second, HW01_LCD_TIME_COLOR);
    hw01_display_alarm(alarm_minute, alarm_second, HW01_LCD_ALARM_COLOR);
    // hw01_draw_bell();

    // Tells us what mode we're in
    static int state = STATE_STM;

    // if 0, second is the one being changed. if 1, minute is
    int update_second = CHANGE_SEC;

    // Color to display time as. Should be green normally, red when the alarm is going off
    int time_color = HW01_LCD_TIME_COLOR;

    while (1)
    {
        // FSM for the alarm clock
        switch(state){
            // Set Time Mode
            case STATE_STM:
                // Get rid of the bell. We don't need it right now
                hw01_erase_bell();

                // Blink the time
                if (ALERT_TIME_BLINK == HW01_ALERT_NONE){
                    hw01_display_time(minute, second, time_color);
                }
                else{
                    hw01_display_time(minute, second, LCD_COLOR_BLACK);
                }

                // If SW1 has been pressed (at all), increment the desired unit
                // With modulos so that the seconds cannot exceed 59
                // and minutes cannot exceed 2
                if (ALERT_SW1_SHORT == HW01_ALERT_BUTTON_LT_2S || ALERT_SW1_LONG == HW01_ALERT_BUTTON_GT_2S){
                    if (update_second == CHANGE_SEC){
                        second = (second + 1)%60;
                    }
                    else if (update_second == CHANGE_MIN){
                        minute = (minute+1)%3;
                    }
                    ALERT_SW1_SHORT = HW01_ALERT_NONE;
                    ALERT_SW1_LONG = HW01_ALERT_NONE;
                }

                // If SW2 has been pressed (at all), decrement the desired unit
                // With modulos so that the seconds cannot exceed 59
                // and minutes cannot exceed 2
                if (ALERT_SW2_SHORT == HW01_ALERT_BUTTON_LT_2S || ALERT_SW2_LONG == HW01_ALERT_BUTTON_GT_2S){
                    if (update_second == CHANGE_SEC){
                        if (second > 0){
                            second = second-1;
                        }
                        else{
                            second = 59;
                        }
                    }
                    else if (update_second == CHANGE_MIN){
                        if (minute > 0){
                            minute = minute-1;
                        }
                        else{
                            minute = 2;
                        }
                    }
                    ALERT_SW2_SHORT = HW01_ALERT_NONE;
                    ALERT_SW2_LONG = HW01_ALERT_NONE;
                }

                // If SW3 has been short-pressed, toggle the unit
                if (ALERT_SW3_SHORT == HW01_ALERT_BUTTON_LT_2S){
                    if (update_second == CHANGE_MIN){
                        update_second = CHANGE_SEC;
                    }
                    else{
                        update_second = CHANGE_MIN;
                    }
                    ALERT_SW3_SHORT = HW01_ALERT_NONE;
                    ALERT_SW3_LONG = HW01_ALERT_NONE;
                }

                // If SW3 has been long-pressed, switch to run mode
                if (ALERT_SW3_LONG == HW01_ALERT_BUTTON_GT_2S){
                    printf("switching to run mode\n\r");
                    state = STATE_RM;
                    ALERT_SW3_SHORT = HW01_ALERT_NONE;
                    ALERT_SW3_LONG = HW01_ALERT_NONE;
                }
                break;
            
            // Relevant variables to run mode

            // Tells us if we have reached a minute. 1 if yes, 0 if no 
            uint8_t reached_minute = 0;
            // Tells us if the alarm is to be enabled (if it's allowed to go off)
            uint8_t alarm_enable = ALARM_DISABLED;
            // Tells us if the alarm is armed (going off now)
            uint8_t alarm_armed = ALARM_DISARMED;

            // Run mode
            case STATE_RM:
                // Show the present time
                hw01_display_time(minute, second, time_color);
                // Show the present alarm
                hw01_display_alarm(alarm_minute, alarm_second, HW01_LCD_ALARM_COLOR);

                // If a second happens, update the second.
                if (ALERT_UPDATE_SECOND == HW01_ALERT_TIME_UPDATE){
                    second = (second + 1)%60;
                    if (second == 0){
                        reached_minute = 1;
                    }
                    ALERT_UPDATE_SECOND = HW01_ALERT_NONE;
                }
                // If we have reached a minute, update the minute
                if (reached_minute == 1){
                    minute = (minute+1)%3;
                    reached_minute = 0;
                }

                // If we have reached the alarm's time, arm it
                if (minute == alarm_minute && second == alarm_second){
                    alarm_armed = ALARM_ARMED;
                }

                // If the alarm is enabled, draw the bell
                if (alarm_enable == ALARM_ENABLED){
                    hw01_draw_bell();
                }
                // Otherwise, get rid of the bell
                else{
                    hw01_erase_bell();
                }

                // If the alarm is enabled (bell drawn) and armed (time reached), buzzzz
                // And set the time color to red (alarm happening)
                if (alarm_enable == ALARM_ENABLED && alarm_armed == ALARM_ARMED){
                    pwm_buzzer_start();
                    time_color = HW01_LCD_ALARM_ACTIVE_COLOR;
                }
                // Otherwise, kill the buzz
                // and set time color to green (normal)
                else{
                    pwm_buzzer_stop();
                    alarm_armed = ALARM_DISARMED;
                    time_color = HW01_LCD_TIME_COLOR;
                }

                // If SW1 has been long-pressed, disable and disarm the armed alarm
                // And get rid of the bell
                if (ALERT_SW1_LONG == HW01_ALERT_BUTTON_GT_2S){
                    if (alarm_armed == ALARM_ARMED){
                        alarm_enable = ALARM_DISABLED;
                        hw01_erase_bell();
                    }
                    alarm_armed = ALARM_DISARMED;
                }

                // If SW3 has been short-pressed, toggle the alarm's enable
                if (ALERT_SW3_SHORT == HW01_ALERT_BUTTON_LT_2S){ //&& has_seen_sam == SEEN_SAM){
                    if (alarm_enable == ALARM_DISABLED){
                        alarm_enable = ALARM_ENABLED;
                        hw01_draw_bell();
                    }
                    else{
                        if (alarm_armed == ALARM_DISARMED){
                            alarm_enable = ALARM_DISABLED;
                            alarm_armed = ALARM_DISARMED;
                            hw01_erase_bell();
                        }
                    }
                }
                
                // If SW3 has been long-pressed, switch to Set Alarm Mode
                if (ALERT_SW3_LONG == HW01_ALERT_BUTTON_GT_2S){
                    printf("switching to set alarm mode\n\r");
                    state = STATE_SAM;
                    update_second = CHANGE_SEC;
                    ALERT_SW3_SHORT = HW01_ALERT_NONE;
                    ALERT_SW3_LONG = HW01_ALERT_NONE;
                }
                break;
            
            // Set Alarm Mode
            case STATE_SAM:

                // Keep running the clock while setting the alarm
                // Same basic code as above
                hw01_display_time(minute, second, HW01_LCD_TIME_COLOR);
                if (ALERT_UPDATE_SECOND == HW01_ALERT_TIME_UPDATE){
                    second = (second + 1)%60;
                    if (second == 0){
                        reached_minute = 1;
                    }
                    ALERT_UPDATE_SECOND = HW01_ALERT_NONE;
                }
                if (reached_minute == 1){
                    minute = (minute+1)%3;
                    reached_minute = 0;
                }

                // Blink the alarm time
                if (ALERT_TIME_BLINK == HW01_ALERT_NONE){
                    hw01_display_alarm(alarm_minute, alarm_second, HW01_LCD_ALARM_COLOR);
                }
                else{
                    hw01_display_alarm(alarm_minute, alarm_second, LCD_COLOR_BLACK);
                }

                // If SW1 has been pressed, increment the desired unit until wraparound
                if (ALERT_SW1_SHORT == HW01_ALERT_BUTTON_LT_2S || ALERT_SW1_LONG == HW01_ALERT_BUTTON_GT_2S){
                    if (update_second == CHANGE_SEC){
                        alarm_second = (alarm_second + 1)%60;
                    }
                    else{
                        alarm_minute = (alarm_minute + 1)%3;
                    }
                    ALERT_SW1_SHORT = HW01_ALERT_NONE;
                    ALERT_SW1_LONG = HW01_ALERT_NONE;
                }

                // If SW2 has been pressed, decrement the desired unit until wraparound
                if (ALERT_SW2_SHORT == HW01_ALERT_BUTTON_LT_2S || ALERT_SW2_LONG == HW01_ALERT_BUTTON_GT_2S){
                    if (update_second == CHANGE_SEC){
                        if (alarm_second > 0){
                            alarm_second = alarm_second-1;
                        }
                        else{
                            alarm_second = 59;
                        }
                    }
                    else{
                        if (alarm_minute > 0){
                            alarm_minute = alarm_minute-1;
                        }
                        else{
                            alarm_minute = 2;
                        }
                    }
                    ALERT_SW2_SHORT = HW01_ALERT_NONE;
                    ALERT_SW3_LONG = HW01_ALERT_NONE;
                }

                // If SW3 has been short-pressed, toggle the unit of time
                if (ALERT_SW3_SHORT == HW01_ALERT_BUTTON_LT_2S){
                    if (update_second == CHANGE_SEC){
                        update_second = CHANGE_MIN;
                    }
                    else{
                        update_second = CHANGE_SEC;
                    }
                    ALERT_SW3_SHORT = HW01_ALERT_NONE;
                    ALERT_SW3_LONG = HW01_ALERT_NONE;

                }

                // If SW3 is long-pressed, switch back to run mode
                if (ALERT_SW3_LONG == HW01_ALERT_BUTTON_GT_2S){
                    printf("switch back to run mode\n\r");
                    ALERT_SW3_SHORT = HW01_ALERT_NONE;
                    ALERT_SW3_LONG = HW01_ALERT_NONE;
                    state = STATE_RM;
                    has_seen_sam = SEEN_SAM;
                }
        }
    }
}