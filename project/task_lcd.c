/**
 * @file task_lcd.c
 * @author Lucas Franke (lmfranke@wisc.edu) & Aditya Kamasani (kamasani@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "task_lcd.h"

TaskHandle_t TaskHandle_LCD;
extern QueueHandle_t Queue_IMU;
extern QueueHandle_t Queue_LTR329;
extern QueueHandle_t Queue_IO_Expander;

// enum to tell us if we're the left player or not
typedef enum{
    INDETERMINATE,
    IS_LEFT,
    IS_RIGHT
} is_left_t;
is_left_t left_state = INDETERMINATE;

/**
 * @brief Bullet structure
 * 
 */
typedef struct{
    uint16_t x;
    uint8_t y;
    uint16_t color;
    uint8_t direction;
} bullet_t;

/**
 * @brief Pretty much just draws the title screen
 * 
 * @param main_color color to draw the title screen as. decided by light sensor
 */
void splash_screen_state(uint16_t main_color){
    lcd_draw_image(
        320 / 2,
        240 / 2,
        TitleScreenWidthPixels,
        TitleScreenHeightPixels,
        TitleScreenBitmaps,
        main_color,
        LCD_COLOR_BLACK);
    
}

/**
 * @brief Draws what happens in the actual play state
 * 
 * @param main_color color to draw the cannons as. decided by light sensor
 * @param y_cannon_left left cannon y pos
 * @param y_cannon_right right cannon y pos
 */
void normal_game_state(uint16_t main_color, uint8_t y_cannon_left, uint8_t y_cannon_right){
    lcd_draw_image(
    13,
    y_cannon_left,
    laser_cannon_width_pixels,
    laser_cannon_height_pixels,
    laser_cannon_left_bitmaps,
    main_color,
    LCD_COLOR_BLACK);

    lcd_draw_image(
    320-13,
    y_cannon_right,
    laser_cannon_width_pixels,
    laser_cannon_height_pixels,
    laser_cannon_right_bitmaps,
    main_color,
    LCD_COLOR_BLACK);
}

/**
 * @brief Just draws the game over screen
 * 
 * @param main_color color to draw the game over screen as. decided by light sensor
 */
void game_over_state(uint16_t main_color){
    printf("drawing game over\n\r");
    lcd_draw_image(
        320 / 2,
        240 / 2,
        GameOverWidthPixels,
        GameOverHeightPixels,
        GameOverBitmaps,
        main_color,
        LCD_COLOR_BLACK);
}

/**
 * @brief draws the bullet
 * 
 * @param bullet bullet object we want to draw
 */
void draw_bullet(bullet_t bullet){
    lcd_draw_rectangle(bullet.x-4, 3, bullet.y, 2, LCD_COLOR_BLACK);
    lcd_draw_rectangle(bullet.x, 3, bullet.y, 2, bullet.color);
    lcd_draw_rectangle(bullet.x+4, 3, bullet.y, 2, LCD_COLOR_BLACK);
}

/**
 * @brief This is our main control task. it administrates pretty much everything
 * 
 * @param Parameters 
 */
void task_lcd(void *Parameters){
    
    // start with the main color white
    static uint16_t main_color = LCD_COLOR_WHITE;

    // initializes the cannons
    static uint16_t y_cannon_left = 120;
    static uint16_t y_cannon_right = 120;

    // initializes the bullets
    static uint8_t bullet_speed = 2;
    static bool right_bullet_fired = false;
    static bool left_bullet_fired = false;
    static bullet_t right_bullet;
    right_bullet.y = y_cannon_right;
    right_bullet.x = 320-13;
    right_bullet.color = main_color;
    right_bullet.direction = 0;
    static bullet_t left_bullet;
    left_bullet.y = y_cannon_left;
    left_bullet.x = 13;
    left_bullet.color = main_color;
    left_bullet.direction = 0;

    // starts the game in title screen state
    static uint8_t game_state = 0;
    
    static uint8_t selection_count = 0;

    // health of our cannon
    static uint8_t our_health = 7;

    while(1){

        // cannon speed
        static int8_t speed = 0;

        // y value read by imu
        static int16_t y = 0;


        // this decides the color to be displayed
        uint8_t light_value;

        xQueueReceive(Queue_LTR329, &light_value, 1);

        if (light_value > 55){
            main_color = LCD_COLOR_RED;
        }
        else{
            main_color = LCD_COLOR_BLUE;
        }

        // Received from the imu
        xQueueReceive(Queue_IMU, &y, 0);

        // logic to get our cannon's speed to where we want it based on the data read by the imu
        speed = y>>8;
        speed = speed/16;


        // Reads in the buttons
        uint8_t sw1_pressed = 0;
        uint8_t sw2_pressed = 0;
        uint8_t sw3_pressed = 0;
        xQueueReceive(Queue_SW1, &sw1_pressed, 0);
        xQueueReceive(Queue_SW2, &sw2_pressed, 0);
        xQueueReceive(Queue_SW3, &sw3_pressed, 0);

        if (sw1_pressed == 1){
            printf("sw1 presssed\n\r");
            // sw1_pressed = 0;
        }
        if (sw2_pressed == 1){
            printf("sw2 presssed\n\r");
            // sw2_pressed = 0;
        }

        // resets the eeprom
        if (sw3_pressed == 1){
            printf("sw3 pressed\n\r");
            uint8_t reset_eeprom = 0xEE;
            xQueueSend(Queue_EEPROM, &reset_eeprom, 1);
            sw3_pressed = 0;
        }

        // initialize the characters to read to from the uart
        static uint8_t char_received = 0x00;
        static uint8_t char_received_previous;

        // state machine for game
        switch (game_state){
            case 0:
                
                // starts us off right
                if (selection_count == 0){
                    our_health = 7;
                    lcd_clear_screen(LCD_COLOR_BLACK);
                    selection_count++;
                }
                splash_screen_state(main_color);
                

                char_received_previous = char_received;

                xQueueReceive(Queue_UART_RX, &char_received, 0);

                if (char_received != 0){
                }

                if (char_received == '\n'){
                    char_received = char_received_previous;
                }

                // if we select player 1 for ourselves, we send 0x5A and we are the left board, and we switch to state 1
                if (char_received != 0x5A && sw1_pressed == 1){
                    if (selection_count <= 1){
                        lcd_clear_screen(LCD_COLOR_BLACK);
                        left_state = IS_LEFT;
                        selection_count++;
                        uint8_t left_select[2] = {0x5A, '\n'};
                        remote_uart_tx_string_polling(left_select);
                        // remote_uart_tx_char_async(left_select);
                        printf("transmitted %d to other board hopefully\n\r", left_select[0]);
                    }
                    game_state = 1;
                }
                // if we receive 5A from the other board, we're the right board and switch to state 1
                else if (char_received == 0x5A){
                    printf("received 5A\n");

                    left_state = IS_RIGHT;

                    lcd_clear_screen(LCD_COLOR_BLACK);
                    char_received = 0x00;
                    game_state = 1;
                }
                sw1_pressed = 0;
                sw2_pressed = 0;
                break;
            // main gameplay state
            case 1:
                // display our health on the io expander leds
                xQueueSend(Queue_IO_Expander, &our_health, 1);

                // if we're dead, game's over
                if(our_health == 0 ){
                    game_state = 2;
                }    

                // displays the game
                normal_game_state(main_color, y_cannon_left , y_cannon_right);

                char_received_previous = char_received;

                // receive from uart
                xQueueReceive(Queue_UART_RX, &char_received, 0);

                // if the other board tells us they've lost, we switch into state 2 and play the victory sound
                if(char_received == Game_Won){
                    printf("we won\n\r");
                    char_received = 0x00;
                    game_state = 2;
                     xTaskNotifyGive(TaskHandle_Buzzer_Victory);
                }
                
                // if the other board tells us they've fired a bullet, we now know they've fired
                if (char_received == 0xFF){
                    if (left_state == IS_LEFT){
                        
                        right_bullet_fired = true;
                    }
                    else if (left_state == IS_RIGHT){
                        left_bullet_fired = true;
                    }
                    char_received = char_received_previous;
                }
                if (char_received == '\n' || char_received == 0){
                    char_received = char_received_previous;
                }

                // left board logic
                if (left_state == IS_LEFT){
                    
                    // if we press switch 1 and we haven't fired a shot, fire a shot
                    if (sw1_pressed == 1 && left_bullet_fired == false){
                        left_bullet_fired = true;
                    }

                    // if the right bullet has been shot, display it
                    if (right_bullet_fired == true){
                        static uint8_t right_fired_count = 0;
                        // initial right bullet setup
                        if (right_fired_count == 0){
                            right_bullet.y = y_cannon_right;
                            right_bullet.x = 320-13;
                            right_fired_count++;
                        }
                        
                        // draw the bullet
                        draw_bullet(right_bullet);

                        // if the bullet hasn't reached the left, move the bullet to the left
                        if (right_bullet.x != 13){
                            right_bullet.x = right_bullet.x-bullet_speed;
                        }
                        // if it has reached the left side, reset the bullet and determine if we take damage
                        else{
                            right_bullet.x = 320-13;
                            right_fired_count = 0;
                            if ((right_bullet.y >= y_cannon_left-20) && (right_bullet.y <= y_cannon_left+20)){
                                // if our health is above 1, we decrease our health by 2, can get shot up to 4 times
                                if (our_health > 1){
                                    our_health -= 2;
                                     xTaskNotifyGive(TaskHandle_Buzzer_Damage);
                                }
                                // if our health is at 1, set our health to zero and tell the other board they won
                                else{
                                    our_health = 0;
                                    uint8_t Game_over[2] = {Game_Won, '\n'};
                                    uint8_t transmit_won = 0;
                                    while(transmit_won < 50){
                                        remote_uart_tx_string_polling(Game_over);
                                        transmit_won++;
                                    }
                                }
                            }
                            // the other board can shoot again
                            right_bullet_fired = false;
                        }
                    }

                    // left bullet display
                    if (left_bullet_fired == true){
                        // initial left bullet setup
                        static uint8_t left_fired_count = 0;
                        if (left_fired_count == 0){
                            left_bullet.y = y_cannon_left;
                            left_bullet.x = 13;
                            while (left_fired_count < 100){
                                uint8_t left_bullet_fired_transmit[2] = {0xFF, '\n'};
                                remote_uart_tx_string_polling(left_bullet_fired_transmit);
                                left_fired_count++;
                            }
                        }
                        
                        // draws the bullet
                        draw_bullet(left_bullet);
                         
                        // moves the bullet to the right
                        if (left_bullet.x != 320-13){
                            left_bullet.x = left_bullet.x+bullet_speed;
                        }
                        // resets the bullet if it's at the right
                        else{
                            left_fired_count = 0;
                            left_bullet.x = 13;
                            left_bullet_fired = false;
                        }
                    }

                    // left cannon drawing logic
                     if (((y_cannon_left + 20 + speed)  < 239) && ((y_cannon_left - 20 + speed) > 0)){
                        y_cannon_left+=speed;
                        // tells the other board we've moved
                        uint8_t y_cannon_left_transmit[2] = {y_cannon_left, '\n'};
                        remote_uart_tx_string_polling(y_cannon_left_transmit);
                    }

                    // sets the right cannon's y position to be whatever character we've received, provided that character isn't /n or 0xFF
                    y_cannon_right = char_received;
                }

                // right board logic
                else if (left_state == IS_RIGHT){

                    // shoot the gun if we haven't fired and sw1 is pressed
                    if (sw1_pressed == 1 && right_bullet_fired == false){
                        right_bullet_fired = true;
                    }

                    // if we have shot a bullet, show it
                    if (right_bullet_fired == true){
                        // initial bullet setup
                        static uint8_t right_fired_count = 0;
                        if (right_fired_count == 0){
                            right_bullet.y = y_cannon_right;
                            right_bullet.x = 320-13;
                            // tell the other board we've fired
                            while (right_fired_count < 100){
                                uint8_t right_bullet_fired_transmit[2] = {0xFF, '\n'};
                                remote_uart_tx_string_polling(right_bullet_fired_transmit);
                                right_fired_count++;
                            }
                        }
                        
                        //draw our bullet
                        draw_bullet(right_bullet);

                        // if right bullet isn't at the left, move it left
                        if (right_bullet.x != 13){
                            right_bullet.x = right_bullet.x-bullet_speed;
                        }

                        // if it is at the left, reset it
                        else{
                            right_bullet.x = 320-13;
                            right_fired_count = 0;
                            right_bullet_fired = false;
                        }
                    }

                    // if the opponent has fired
                    if (left_bullet_fired == true){
                        // initial bullet setup
                        static uint8_t left_fired_count = 0;
                        if (left_fired_count == 0){
                            left_bullet.y = y_cannon_left;
                            left_bullet.x = 13;
                            left_fired_count++;
                        }
                        
                        // draw the left bullet
                        draw_bullet(left_bullet);

                        // if left bullet hasn't reached the right, move it to the right
                        if (left_bullet.x != 320-13){
                            left_bullet.x = left_bullet.x+bullet_speed;
                        }

                        // if the left bullet has reached the right
                        else{
                            left_fired_count = 0;
                            left_bullet.x = 13;
                            // if the bullet hits us, decrease our health and make the sound
                            if ((left_bullet.y >= y_cannon_right-20) && (left_bullet.y <= y_cannon_right+20)){
                                if (our_health > 1){
                                    our_health -= 2;
                                     xTaskNotifyGive(TaskHandle_Buzzer_Damage);
                                    // pwm_damage();
                                }
                                // if our health is at 1 when we got shot, set it to zero and send the game over byte over the uart
                                else{
                                    our_health = 0;
                                    uint8_t Game_over[2] = {Game_Won, '\n'};
                                    remote_uart_tx_string_polling(Game_over);
                                    uint8_t transmit_won = 0;
                                    while(transmit_won < 50){
                                        remote_uart_tx_string_polling(Game_over);
                                        transmit_won++;
                                    }
                                    // game_state = 2;
                                }
                            }

                            // reset the bullet fired state
                            left_bullet_fired = false;
                        }
                    }
                    
                    // logic to move the right cannon
                     if (((y_cannon_right - 20 + speed) > 0) && ((y_cannon_right + 20 + speed) < 239)){
                        y_cannon_right+=speed;
                        // send the other board our position if we've moved
                        uint8_t y_cannon_right_transmit[2] = {y_cannon_right, '\n'};
                        remote_uart_tx_string_polling(y_cannon_right_transmit);
                    }

                    // if the char we received isn't \n or 0xFF, draw the left cannon at that position
                    y_cannon_left = char_received;
                }

                // reset the switches
                sw1_pressed = 0;
                sw2_pressed = 0;
                break;
            
            static uint8_t game_over_count = 0;
            case 2:
                // look for sw1 input, make sure it's at zero before the next game starts
                xQueueReceive(Queue_SW1, &sw1_pressed, 0);
                sw1_pressed = 0;

                // initial game over setup
                if (game_over_count == 0){
                    lcd_clear_screen(LCD_COLOR_BLACK);
                    printf("our health: %d\n\r", our_health);
                    game_over_state(main_color);
                    // sends the eeprom the info for it to determine our win streak
                    xQueueSend(Queue_EEPROM, &our_health, 1);

                    // reset the cannon positions
                    y_cannon_left = 120;
                    y_cannon_right = 120;
                    game_over_count++;
                }

                // wait for uart info from the other board
                xQueueReceive(Queue_UART_RX, &char_received, 0);
                if (char_received != 0){
                    // printf("char received: %d\n\r", char_received);
                }

                if (char_received == '\n'){
                    char_received = char_received_previous;
                }

                // if we select player 1 for ourselves, we send 0x5A and we are the left board
                // this all just resets the game to the play state when we hit sw2
                if (char_received != 0x5A && sw2_pressed == 1){
                    if (game_over_count <= 1){
                        lcd_clear_screen(LCD_COLOR_BLACK);
                        selection_count++;
                        uint8_t left_select[2] = {0x5A, '\n'};
                        remote_uart_tx_string_polling(left_select);
                        // remote_uart_tx_char_async(left_select);
                        printf("transmitted %d to other board hopefully\n\r", left_select[0]);
                    }
                    // put our health back to zero and start the game again
                    our_health = 7;
                    game_over_count = 0;
                    char_received = 0x00;
                    game_state = 1;
                }

                // if the other board wants to start, we start the game
                else if (char_received == 0x5A){
                    printf("received 5A\n");

                    lcd_clear_screen(LCD_COLOR_BLACK);
                    our_health = 7;
                    char_received = 0x00;
                    game_over_count = 0;
                    game_state = 1;
                }
                break;

        }
        
    }
}

void task_lcd_init(){
    ece353_enable_lcd();

    xTaskCreate(
        task_lcd,
        "Task LCD",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &TaskHandle_LCD
    );
}

