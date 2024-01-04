/**
 * @file hw02.c
 * @author Lucas Franke (lmfranke@wisc.edu) and Aditya Kamasani (kamasani@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-09-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "main.h"
#include "hw02.h"
#include "hw02-lcd-staff.h"
#include "hw02-images.h"


/*****************************************************************************/
/*  Global Variables                                                         */
/*****************************************************************************/
char HW02_DESCRIPTION[] = "ECE353: HW02 Lucas Franke, Aditya Kamasani";
#define TICKS_MS_100 10000000

typedef enum {
    ALERT_TIMER_NONE,
    ALERT_TIMER_100MS
}alert_timer_t;
volatile alert_timer_t ALERT_TIMER = ALERT_TIMER_NONE; 

typedef enum{
    ALERT_SW_NONE,
    ALERT_SW_1,
    ALERT_SW_2
} alert_sw_t;
volatile alert_sw_t ALERT_SW1 = ALERT_SW_NONE;
volatile alert_sw_t ALERT_SW2 = ALERT_SW_NONE;

typedef enum{
    PLAYER1_SELECTION,
    O_PLAYER_TURN,
    X_PLAYER_TURN,
    O_PLAYER_WINS,
    X_PLAYER_WINS,
    TIE
} game_states_t;

// Possible values for the game state
typedef enum{
    SQUARE_OPEN,
    O_SQUARE_TEMP,
    X_SQUARE_TEMP,
    O_SQUARE_PERM,
    X_SQUARE_PERM
} square_state_t;

game_states_t state = PLAYER1_SELECTION;

// Tells us who the starting player is/was
typedef enum{
    O_PLAYER,
    X_PLAYER
} start_player_t;
start_player_t start_player = O_PLAYER;

// States of all the squares and temp squares
square_state_t squares[9] = {SQUARE_OPEN, SQUARE_OPEN, SQUARE_OPEN, SQUARE_OPEN, SQUARE_OPEN, SQUARE_OPEN, SQUARE_OPEN, SQUARE_OPEN, SQUARE_OPEN};
square_state_t temp_squares[9] = {SQUARE_OPEN, SQUARE_OPEN, SQUARE_OPEN, SQUARE_OPEN, SQUARE_OPEN, SQUARE_OPEN, SQUARE_OPEN, SQUARE_OPEN, SQUARE_OPEN};

// Tells us which square the cursor is currently in
uint8_t current_square = 5;
uint16_t square_loc[2] = {CENTER_COL, CENTER_ROW};

/* Timer object and timer_cfg object */
cyhal_timer_t timer_obj_2;

cyhal_timer_cfg_t timer_cfg_2 =
    {
        .compare_value = 0,              /* Timer compare value, not used */
        .period = 0,                     /* number of timer ticks */
        .direction = CYHAL_TIMER_DIR_UP, /* Timer counts up */
        .is_compare = false,             /* Don't use compare mode */
        .is_continuous = true,           /* Run timer indefinitely */
        .value = 0                       /* Initial value of counter */
};

// Objects to tell us the current and previous positions of the joystick
joystick_position_t curr_position = JOYSTICK_POS_CENTER;
joystick_position_t prev_position = JOYSTICK_POS_CENTER;

// enum to tell us if we're player 1 or not
typedef enum{
    INDETERMINATE,
    IS_PLAYER_1,
    IS_NOT_PLAYER_1
} is_player1_t;
is_player1_t p1_state = INDETERMINATE;

// enum to tell us if it's our turn
typedef enum{
    UNKNOWN,
    OUR_TURN,
    THEIR_TURN
} is_our_turn_t;
is_our_turn_t our_turn = UNKNOWN;

/*****************************************************************************/
/*  Interrupt Handlers                                                       */
/*****************************************************************************/

// Interrupt handler. Sends an alert and checks the buttons every 100ms
void Handler_Timer_2(void *handler_arg, cyhal_timer_event_t event)
{   
    // Tell us if the switch has been pressed
    volatile static uint8_t handler_called_sw1 = 0;
    volatile static uint8_t handler_called_sw2 = 0;
    // Define the port where we get our buttonsa
    uint32_t buttons_reg_val = PORT_BUTTONS->IN;

    // Tells us if the button is being pressed (and how long)
    if (((buttons_reg_val & SW1_MASK) == 0)){
        handler_called_sw1 = handler_called_sw1 + 1;
    }

    // Upon release or not being pressed:
    else{
        // If SW1 was long-pressed, tell us that SW1 has been long-pressed
        if (handler_called_sw1 >= 1){
            printf("handler1: %d\n\r",handler_called_sw1);
            ALERT_SW1 = ALERT_SW_1;
        }
        // If SW1 hasn't been pressed, clear the alerts
        else{
            ALERT_SW1= ALERT_SW_NONE;
        }
        // Reset the SW1 checker to 0
        handler_called_sw1 = 0;
    }

    // Tells us if the button is being pressed (and how long)
    if (((buttons_reg_val & SW2_MASK) == 0)){
        handler_called_sw2 = handler_called_sw2 + 1;
    }

    // Upon release or not being pressed:
    else{
        // If SW1 was long-pressed, tell us that SW1 has been long-pressed
        if (handler_called_sw2 >= 1){
            printf("handler2: %d\n\r",handler_called_sw2);
            ALERT_SW2 = ALERT_SW_2;
        }
        // If SW1 hasn't been pressed, clear the alerts
        else{
            ALERT_SW2= ALERT_SW_NONE;
        }
        // Reset the SW1 checker to 0
        handler_called_sw2 = 0;
    }


    ALERT_TIMER = ALERT_TIMER_100MS;
}

/*****************************************************************************/
/*  HW02 Functions                                                           */
/*****************************************************************************/

// Evaluates a bunch of different cases for where the current square is and moves it left accordingly
void move_left(void)
{
    if (current_square == 1){
        current_square = 3;
        square_loc[0] = RIGHT_COL;
        square_loc[1] = UPPER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 2){
        current_square = 1;
        square_loc[0] = LEFT_COL;
        square_loc[1] = UPPER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 3){
        current_square = 2;
        square_loc[0] = CENTER_COL;
        square_loc[1] = UPPER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 4){
        current_square = 6;
        square_loc[0] = RIGHT_COL;
        square_loc[1] = CENTER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 5){
        current_square = 4;
        square_loc[0] = LEFT_COL;
        square_loc[1] = CENTER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 6){
        current_square = 5;
        square_loc[0] = CENTER_COL;
        square_loc[1] = CENTER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 7){
        current_square = 9;
        square_loc[0] = RIGHT_COL;
        square_loc[1] = LOWER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 8){
        current_square = 7;
        square_loc[0] = LEFT_COL;
        square_loc[1] = LOWER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 9){
        current_square = 8;
        square_loc[0] = CENTER_COL;
        square_loc[1] = LOWER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
}

// Evaluates a bunch of different cases for where the current square is and moves it right accordingly
void move_right(void)
{
    if (current_square == 1){
        current_square = 2;
        square_loc[0] = CENTER_COL;
        square_loc[1] = UPPER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 2){
        current_square = 3;
        square_loc[0] = RIGHT_COL;
        square_loc[1] = UPPER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 3){
        current_square = 1;
        square_loc[0] = LEFT_COL;
        square_loc[1] = UPPER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 4){
        current_square = 5;
        square_loc[0] = CENTER_COL;
        square_loc[1] = CENTER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 5){
        current_square = 6;
        square_loc[0] = RIGHT_COL;
        square_loc[1] = CENTER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 6){
        current_square = 4;
        square_loc[0] = LEFT_COL;
        square_loc[1] = CENTER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 7){
        current_square = 8;
        square_loc[0] = CENTER_COL;
        square_loc[1] = LOWER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 8){
        current_square = 9;
        square_loc[0] = RIGHT_COL;
        square_loc[1] = LOWER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 9){
        current_square = 7;
        square_loc[0] = LEFT_COL;
        square_loc[1] = LOWER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
}

// Evaluates a bunch of different cases for where the current square is and moves it up accordingly
void move_up(void)
{
    if (current_square == 1){
        current_square = 7;
        square_loc[0] = LEFT_COL;
        square_loc[1] = LOWER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 2){
        current_square = 8;
        square_loc[0] = CENTER_COL;
        square_loc[1] = LOWER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 3){
        current_square = 9;
        square_loc[0] = RIGHT_COL;
        square_loc[1] = LOWER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 4){
        current_square = 1;
        square_loc[0] = LEFT_COL;
        square_loc[1] = UPPER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 5){
        current_square = 2;
        square_loc[0] = CENTER_COL;
        square_loc[1] = UPPER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 6){
        current_square = 3;
        square_loc[0] = RIGHT_COL;
        square_loc[1] = UPPER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 7){
        current_square = 4;
        square_loc[0] = LEFT_COL;
        square_loc[1] = CENTER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 8){
        current_square = 5;
        square_loc[0] = CENTER_COL;
        square_loc[1] = CENTER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 9){
        current_square = 6;
        square_loc[0] = RIGHT_COL;
        square_loc[1] = CENTER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
}

// Evaluates a bunch of different cases for where the current square is and moves it down accordingly
void move_down(void)
{
    if (current_square == 1){
        current_square = 4;
        square_loc[0] = LEFT_COL;
        square_loc[1] = CENTER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 2){
        current_square = 5;
        square_loc[0] = CENTER_COL;
        square_loc[1] = CENTER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 3){
        current_square = 6;
        square_loc[0] = RIGHT_COL;
        square_loc[1] = CENTER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 4){
        current_square = 7;
        square_loc[0] = LEFT_COL;
        square_loc[1] = LOWER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 5){
        current_square = 8;
        square_loc[0] = CENTER_COL;
        square_loc[1] = LOWER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 6){
        current_square = 9;
        square_loc[0] = RIGHT_COL;
        square_loc[1] = LOWER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 7){
        current_square = 1;
        square_loc[0] = LEFT_COL;
        square_loc[1] = UPPER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 8){
        current_square = 2;
        square_loc[0] = CENTER_COL;
        square_loc[1] = UPPER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
    else if (current_square == 9){
        current_square = 3;
        square_loc[0] = RIGHT_COL;
        square_loc[1] = UPPER_ROW;
        printf("current square: %d\n\r", current_square);
        return;
    }
}

// Draws the crosshatch background of the board
void board_background(void)
{
  // Horizontal lines
  lcd_draw_rectangle_centered(SCREEN_CENTER_COL, UPPER_HORIZONTAL_LINE_Y, LINE_LENGTH, LINE_WIDTH, LCD_COLOR_BLUE);
  lcd_draw_rectangle_centered(SCREEN_CENTER_COL, LOWER_HORIZONTAL_LINE_Y, LINE_LENGTH, LINE_WIDTH, LCD_COLOR_BLUE);

  // Vertical Lines
  lcd_draw_rectangle_centered(LEFT_HORIZONTAL_LINE_X, SCREEN_CENTER_ROW, LINE_WIDTH, LINE_LENGTH, LCD_COLOR_BLUE);
  lcd_draw_rectangle_centered(RIGHT_HORIZONTAL_LINE_X, SCREEN_CENTER_ROW, LINE_WIDTH, LINE_LENGTH, LCD_COLOR_BLUE);
}

// My least favorite. Draws the squares as they're supposed to be. Way to many states to think about. Gave me a headache
void draw_squares(void){

    // Square 1

    // 0 open no temp
    if (squares[0] == SQUARE_OPEN && temp_squares[0] != O_SQUARE_TEMP && temp_squares[0] != X_SQUARE_TEMP){
        // lcd_draw_image(LEFT_COL, UPPER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        // lcd_draw_image(LEFT_COL, UPPER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        lcd_draw_rectangle_centered(LEFT_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, FG_COLOR_UNCLAIMED);
    }
    // 0 O perm no temp
    else if (squares[0] == O_SQUARE_PERM && temp_squares[0] != O_SQUARE_TEMP && temp_squares[0] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(LEFT_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_O);
        lcd_draw_image(LEFT_COL, UPPER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_O, BG_COLOR_O);
    }
    // 0 O perm w/ temp
    else if (squares[0] == O_SQUARE_PERM && (temp_squares[0] == O_SQUARE_TEMP || temp_squares[0] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(LEFT_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(LEFT_COL, UPPER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 0 X perm no temp
    else if (squares[0] == X_SQUARE_PERM && temp_squares[0] != O_SQUARE_TEMP && temp_squares[0] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(LEFT_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_X);
        lcd_draw_image(LEFT_COL, UPPER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_X, BG_COLOR_X);
    }
    // 0 X perm w/ temp
    else if (squares[0] == X_SQUARE_PERM && (temp_squares[0] == O_SQUARE_TEMP || temp_squares[0] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(LEFT_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(LEFT_COL, UPPER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 0 open O temp
    else if (squares[0] == SQUARE_OPEN && temp_squares[0] == O_SQUARE_TEMP){
        lcd_draw_rectangle_centered(LEFT_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(LEFT_COL, UPPER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }
    // 0 open X temp
    else if (squares[0] == SQUARE_OPEN && temp_squares[0] == X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(LEFT_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(LEFT_COL, UPPER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }

    // Square 2

    // 1 open no temp
    if (squares[1] == SQUARE_OPEN && temp_squares[1] != O_SQUARE_TEMP && temp_squares[1] != X_SQUARE_TEMP){
        // lcd_draw_image(CENTER_COL, UPPER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        // lcd_draw_image(CENTER_COL, UPPER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        lcd_draw_rectangle_centered(CENTER_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, FG_COLOR_UNCLAIMED);
    }
    // 1 O perm no temp
    else if (squares[1] == O_SQUARE_PERM && temp_squares[1] != O_SQUARE_TEMP && temp_squares[1] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(CENTER_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_O);
        lcd_draw_image(CENTER_COL, UPPER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_O, BG_COLOR_O);
    }
    // 1 O perm w/ temp
    else if (squares[1] == O_SQUARE_PERM && (temp_squares[1] == O_SQUARE_TEMP || temp_squares[1] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(CENTER_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(CENTER_COL, UPPER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 1 X perm no temp
    else if (squares[1] == X_SQUARE_PERM && temp_squares[1] != O_SQUARE_TEMP && temp_squares[1] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(CENTER_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_X);
        lcd_draw_image(CENTER_COL, UPPER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_X, BG_COLOR_X);
    }
    // 1 X perm w/ temp
    else if (squares[1] == X_SQUARE_PERM && (temp_squares[1] == O_SQUARE_TEMP || temp_squares[1] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(CENTER_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(CENTER_COL, UPPER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 1 open O temp
    else if (squares[1] == SQUARE_OPEN && temp_squares[1] == O_SQUARE_TEMP){
        lcd_draw_rectangle_centered(CENTER_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(CENTER_COL, UPPER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }
    // 1 open X temp
    else if (squares[1] == SQUARE_OPEN && temp_squares[1] == X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(CENTER_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(CENTER_COL, UPPER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }

    // Square 3

    // 2 open no temp
    if (squares[2] == SQUARE_OPEN && temp_squares[2] != O_SQUARE_TEMP && temp_squares[2] != X_SQUARE_TEMP){
        // lcd_draw_image(RIGHT_COL, UPPER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        // lcd_draw_image(RIGHT_COL, UPPER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        lcd_draw_rectangle_centered(RIGHT_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, FG_COLOR_UNCLAIMED);
    }
    // 2 O perm no temp
    else if (squares[2] == O_SQUARE_PERM && temp_squares[2] != O_SQUARE_TEMP && temp_squares[2] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(RIGHT_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_O);
        lcd_draw_image(RIGHT_COL, UPPER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_O, BG_COLOR_O);
    }
    // 2 O perm w/ temp
    else if (squares[2] == O_SQUARE_PERM && (temp_squares[2] == O_SQUARE_TEMP || temp_squares[2] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(RIGHT_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(RIGHT_COL, UPPER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 2 X perm no temp
    else if (squares[2] == X_SQUARE_PERM && temp_squares[2] != O_SQUARE_TEMP && temp_squares[2] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(RIGHT_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_X);
        lcd_draw_image(RIGHT_COL, UPPER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_X, BG_COLOR_X);
    }
    // 2 X perm w/ temp
    else if (squares[2] == X_SQUARE_PERM && (temp_squares[2] == O_SQUARE_TEMP || temp_squares[2] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(RIGHT_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(RIGHT_COL, UPPER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 2 open O temp
    else if (squares[2] == SQUARE_OPEN && temp_squares[2] == O_SQUARE_TEMP){
        lcd_draw_rectangle_centered(RIGHT_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(RIGHT_COL, UPPER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }
    // 2 open X temp
    else if (squares[2] == SQUARE_OPEN && temp_squares[2] == X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(RIGHT_COL, UPPER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(RIGHT_COL, UPPER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }

    // Square 4

    // 3 open no temp
    if (squares[3] == SQUARE_OPEN && temp_squares[3] != O_SQUARE_TEMP && temp_squares[3] != X_SQUARE_TEMP){
        // lcd_draw_image(LEFT_COL, CENTER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        // lcd_draw_image(LEFT_COL, CENTER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        lcd_draw_rectangle_centered(LEFT_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, FG_COLOR_UNCLAIMED);
    }
    // 3 O perm no temp
    else if (squares[3] == O_SQUARE_PERM && temp_squares[3] != O_SQUARE_TEMP && temp_squares[3] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(LEFT_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_O);
        lcd_draw_image(LEFT_COL, CENTER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_O, BG_COLOR_O);
    }
    // 3 O perm w/ temp
    else if (squares[3] == O_SQUARE_PERM && (temp_squares[3] == O_SQUARE_TEMP || temp_squares[3] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(LEFT_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(LEFT_COL, CENTER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 3 X perm no temp
    else if (squares[3] == X_SQUARE_PERM && temp_squares[3] != O_SQUARE_TEMP && temp_squares[3] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(LEFT_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_X);
        lcd_draw_image(LEFT_COL, CENTER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_X, BG_COLOR_X);
    }
    // 3 X perm w/ temp
    else if (squares[3] == X_SQUARE_PERM && (temp_squares[3] == O_SQUARE_TEMP || temp_squares[3] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(LEFT_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(LEFT_COL, CENTER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 3 open O temp
    else if (squares[3] == SQUARE_OPEN && temp_squares[3] == O_SQUARE_TEMP){
        lcd_draw_rectangle_centered(LEFT_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(LEFT_COL, CENTER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }
    // 3 open X temp
    else if (squares[3] == SQUARE_OPEN && temp_squares[3] == X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(LEFT_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(LEFT_COL, CENTER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }

    // Square 5

    // 4 open no temp
    if (squares[4] == SQUARE_OPEN && temp_squares[4] != O_SQUARE_TEMP && temp_squares[4] != X_SQUARE_TEMP){
        // lcd_draw_image(CENTER_COL, CENTER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        // lcd_draw_image(CENTER_COL, CENTER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        lcd_draw_rectangle_centered(CENTER_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, FG_COLOR_UNCLAIMED);
    }
    // 4 O perm no temp
    else if (squares[4] == O_SQUARE_PERM && temp_squares[4] != O_SQUARE_TEMP && temp_squares[4] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(CENTER_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_O);
        lcd_draw_image(CENTER_COL, CENTER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_O, BG_COLOR_O);
    }
    // 4 O perm w/ temp
    else if (squares[4] == O_SQUARE_PERM && (temp_squares[4] == O_SQUARE_TEMP || temp_squares[4] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(CENTER_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(CENTER_COL, CENTER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 4 X perm no temp
    else if (squares[4] == X_SQUARE_PERM && temp_squares[4] != O_SQUARE_TEMP && temp_squares[4] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(CENTER_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_X);
        lcd_draw_image(CENTER_COL, CENTER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_X, BG_COLOR_X);
    }
    // 4 X perm w/ temp
    else if (squares[4] == X_SQUARE_PERM && (temp_squares[4] == O_SQUARE_TEMP || temp_squares[4] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(CENTER_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(CENTER_COL, CENTER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 4 open O temp
    else if (squares[4] == SQUARE_OPEN && temp_squares[4] == O_SQUARE_TEMP){
        lcd_draw_rectangle_centered(CENTER_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(CENTER_COL, CENTER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }
    // 4 open X temp
    else if (squares[4] == SQUARE_OPEN && temp_squares[4] == X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(CENTER_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(CENTER_COL, CENTER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }

    // Square 6

    // 5 open no temp
    if (squares[5] == SQUARE_OPEN && temp_squares[5] != O_SQUARE_TEMP && temp_squares[5] != X_SQUARE_TEMP){
        // lcd_draw_image(RIGHT_COL, CENTER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        // lcd_draw_image(RIGHT_COL, CENTER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        lcd_draw_rectangle_centered(RIGHT_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, FG_COLOR_UNCLAIMED);
    }
    // 5 O perm no temp
    else if (squares[5] == O_SQUARE_PERM && temp_squares[5] != O_SQUARE_TEMP && temp_squares[5] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(RIGHT_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_O);
        lcd_draw_image(RIGHT_COL, CENTER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_O, BG_COLOR_O);
    }
    // 5 O perm w/ temp
    else if (squares[5] == O_SQUARE_PERM && (temp_squares[5] == O_SQUARE_TEMP || temp_squares[5] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(RIGHT_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(RIGHT_COL, CENTER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 5 X perm no temp
    else if (squares[5] == X_SQUARE_PERM && temp_squares[5] != O_SQUARE_TEMP && temp_squares[5] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(RIGHT_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_X);
        lcd_draw_image(RIGHT_COL, CENTER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_X, BG_COLOR_X);
    }
    // 5 X perm w/ temp
    else if (squares[5] == X_SQUARE_PERM && (temp_squares[5] == O_SQUARE_TEMP || temp_squares[5] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(RIGHT_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(RIGHT_COL, CENTER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 5 open O temp
    else if (squares[5] == SQUARE_OPEN && temp_squares[5] == O_SQUARE_TEMP){
        lcd_draw_rectangle_centered(RIGHT_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(RIGHT_COL, CENTER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }
    // 5 open X temp
    else if (squares[5] == SQUARE_OPEN && temp_squares[5] == X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(RIGHT_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(RIGHT_COL, CENTER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }

    // Square 7

    // 6 open no temp
    if (squares[6] == SQUARE_OPEN && temp_squares[6] != O_SQUARE_TEMP && temp_squares[6] != X_SQUARE_TEMP){
        // lcd_draw_image(LEFT_COL, LOWER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        // lcd_draw_image(LEFT_COL, LOWER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        lcd_draw_rectangle_centered(LEFT_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, FG_COLOR_UNCLAIMED);
    }
    // 6 O perm no temp
    else if (squares[6] == O_SQUARE_PERM && temp_squares[6] != O_SQUARE_TEMP && temp_squares[6] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(LEFT_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_O);
        lcd_draw_image(LEFT_COL, LOWER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_O, BG_COLOR_O);
    }
    // 6 O perm w/ temp
    else if (squares[6] == O_SQUARE_PERM && (temp_squares[6] == O_SQUARE_TEMP || temp_squares[6] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(LEFT_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(LEFT_COL, LOWER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 6 X perm no temp
    else if (squares[6] == X_SQUARE_PERM && temp_squares[6] != O_SQUARE_TEMP && temp_squares[6] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(LEFT_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_X);
        lcd_draw_image(LEFT_COL, LOWER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_X, BG_COLOR_X);
    }
    // 6 X perm w/ temp
    else if (squares[6] == X_SQUARE_PERM && (temp_squares[6] == O_SQUARE_TEMP || temp_squares[6] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(LEFT_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(LEFT_COL, LOWER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 6 open O temp
    else if (squares[6] == SQUARE_OPEN && temp_squares[6] == O_SQUARE_TEMP){
        lcd_draw_rectangle_centered(LEFT_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(LEFT_COL, LOWER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }
    // 6 open X temp
    else if (squares[6] == SQUARE_OPEN && temp_squares[6] == X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(LEFT_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(LEFT_COL, LOWER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }

    // Square 8

    // 7 open no temp
    if (squares[7] == SQUARE_OPEN && temp_squares[7] != O_SQUARE_TEMP && temp_squares[7] != X_SQUARE_TEMP){
        // lcd_draw_image(CENTER_COL, LOWER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        // lcd_draw_image(CENTER_COL, LOWER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        lcd_draw_rectangle_centered(CENTER_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, FG_COLOR_UNCLAIMED);
    }
    // 7 O perm no temp
    else if (squares[7] == O_SQUARE_PERM && temp_squares[7] != O_SQUARE_TEMP && temp_squares[7] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(CENTER_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_O);
        lcd_draw_image(CENTER_COL, LOWER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_O, BG_COLOR_O);
    }
    // 7 O perm w/ temp
    else if (squares[7] == O_SQUARE_PERM && (temp_squares[7] == O_SQUARE_TEMP || temp_squares[7] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(CENTER_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(CENTER_COL, LOWER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 7 X perm no temp
    else if (squares[7] == X_SQUARE_PERM && temp_squares[7] != O_SQUARE_TEMP && temp_squares[7] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(CENTER_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_X);
        lcd_draw_image(CENTER_COL, LOWER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_X, BG_COLOR_X);
    }
    // 7 X perm w/ temp
    else if (squares[7] == X_SQUARE_PERM && (temp_squares[7] == O_SQUARE_TEMP || temp_squares[7] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(CENTER_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(CENTER_COL, LOWER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 7 open O temp
    else if (squares[7] == SQUARE_OPEN && temp_squares[7] == O_SQUARE_TEMP){
        lcd_draw_rectangle_centered(CENTER_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(CENTER_COL, LOWER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }
    // 7 open X temp
    else if (squares[7] == SQUARE_OPEN && temp_squares[7] == X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(CENTER_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(CENTER_COL, LOWER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }

    // Square 9

    // 8 open no temp
    if (squares[8] == SQUARE_OPEN && temp_squares[8] != O_SQUARE_TEMP && temp_squares[8] != X_SQUARE_TEMP){
        // lcd_draw_image(RIGHT_COL, LOWER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        // lcd_draw_image(RIGHT_COL, LOWER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, FG_COLOR_UNCLAIMED);
        lcd_draw_rectangle_centered(RIGHT_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, FG_COLOR_UNCLAIMED);
    }
    // 8 O perm no temp
    else if (squares[8] == O_SQUARE_PERM && temp_squares[8] != O_SQUARE_TEMP && temp_squares[8] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(RIGHT_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_O);
        lcd_draw_image(RIGHT_COL, LOWER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_O, BG_COLOR_O);
    }
    // 8 O perm w/ temp
    else if (squares[8] == O_SQUARE_PERM && (temp_squares[8] == O_SQUARE_TEMP || temp_squares[8] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(RIGHT_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(RIGHT_COL, LOWER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 8 X perm no temp
    else if (squares[8] == X_SQUARE_PERM && temp_squares[8] != O_SQUARE_TEMP && temp_squares[8] != X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(RIGHT_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_X);
        lcd_draw_image(RIGHT_COL, LOWER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_X, BG_COLOR_X);
    }
    // 8 X perm w/ temp
    else if (squares[8] == X_SQUARE_PERM && (temp_squares[8] == O_SQUARE_TEMP || temp_squares[8] == X_SQUARE_TEMP)){
        lcd_draw_rectangle_centered(RIGHT_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_CLAIMED);
        lcd_draw_image(RIGHT_COL, LOWER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_CLAIMED, BG_COLOR_CLAIMED);
    }
    // 8 open O temp
    else if (squares[8] == SQUARE_OPEN && temp_squares[8] == O_SQUARE_TEMP){
        lcd_draw_rectangle_centered(RIGHT_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(RIGHT_COL, LOWER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }
    // 8 open X temp
    else if (squares[8] == SQUARE_OPEN && temp_squares[8] == X_SQUARE_TEMP){
        lcd_draw_rectangle_centered(RIGHT_COL, LOWER_ROW, SQUARE_SIZE, SQUARE_SIZE, BG_COLOR_UNCLAIMED);
        lcd_draw_image(RIGHT_COL, LOWER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, FG_COLOR_UNCLAIMED, BG_COLOR_UNCLAIMED);
    }
}

// Function to check who has won or if there's a tie
void check_win_and_tie(void){
    uint8_t board_full = 1;
    for (int i = 0; i < 9; i++){
        if (squares[i] == SQUARE_OPEN){
            board_full = 0;
        }
    }

    // Check top row for either players' win
    if (squares[0] == O_SQUARE_PERM && squares[1] == O_SQUARE_PERM && squares[2] == O_SQUARE_PERM ){
        state = O_PLAYER_WINS;
        printf("O wins\n\r");
        return;
    }
    else if (squares[0] == X_SQUARE_PERM && squares[1] == X_SQUARE_PERM && squares[2] == X_SQUARE_PERM ){
        state = X_PLAYER_WINS;
        printf("X wins\n\r");
        return;
    }
    
    // Check center row for both players' wins
    if (squares[3] == O_SQUARE_PERM && squares[4] == O_SQUARE_PERM && squares[5] == O_SQUARE_PERM ){
        state = O_PLAYER_WINS;
        printf("O wins\n\r");
        return;
    }
    else if (squares[3] == X_SQUARE_PERM && squares[4] == X_SQUARE_PERM && squares[5] == X_SQUARE_PERM ){
        state = X_PLAYER_WINS;
        printf("X wins\n\r");
        return;
    }

    // Check lower row for both players' wins
    if (squares[6] == O_SQUARE_PERM && squares[7] == O_SQUARE_PERM && squares[8] == O_SQUARE_PERM ){
        state = O_PLAYER_WINS;
        printf("O wins\n\r");
        return;
    }
    else if (squares[6] == X_SQUARE_PERM && squares[7] == X_SQUARE_PERM && squares[8] == X_SQUARE_PERM ){
        state = X_PLAYER_WINS;
        printf("X wins\n\r");
        return;
    }

    // Check left column for both players' wins
    if (squares[0] == O_SQUARE_PERM && squares[3] == O_SQUARE_PERM && squares[6] == O_SQUARE_PERM ){
        state = O_PLAYER_WINS;
        printf("O wins\n\r");
        return;
    }
    else if (squares[0] == X_SQUARE_PERM && squares[3] == X_SQUARE_PERM && squares[6] == X_SQUARE_PERM ){
        state = X_PLAYER_WINS;
        printf("X wins\n\r");
        return;
    }

    // Check center column for both players' wins
    if (squares[1] == O_SQUARE_PERM && squares[4] == O_SQUARE_PERM && squares[7] == O_SQUARE_PERM ){
        state = O_PLAYER_WINS;
        printf("O wins\n\r");
        return;
    }
    else if (squares[1] == X_SQUARE_PERM && squares[4] == X_SQUARE_PERM && squares[7] == X_SQUARE_PERM ){
        state = X_PLAYER_WINS;
        printf("X wins\n\r");
        return;
    }

    // Check right column for both players' wins
    if (squares[2] == O_SQUARE_PERM && squares[5] == O_SQUARE_PERM && squares[8] == O_SQUARE_PERM ){
        state = O_PLAYER_WINS;
        printf("O wins\n\r");
        return;
    }
    else if (squares[2] == X_SQUARE_PERM && squares[5] == X_SQUARE_PERM && squares[8] == X_SQUARE_PERM ){
        state = X_PLAYER_WINS;
        printf("X wins\n\r");
        return;
    }


    // Check bottom left to top right diagonal
    if (squares[6] == O_SQUARE_PERM && squares[4] == O_SQUARE_PERM && squares[2] == O_SQUARE_PERM ){
        state = O_PLAYER_WINS;
        printf("O wins\n\r");
        return;
    }
    else if (squares[6] == X_SQUARE_PERM && squares[4] == X_SQUARE_PERM && squares[2] == X_SQUARE_PERM ){
        state = X_PLAYER_WINS;
        printf("X wins\n\r");
        return;
    }

    // Check top left to bottom right diagonal
    if (squares[0] == O_SQUARE_PERM && squares[4] == O_SQUARE_PERM && squares[8] == O_SQUARE_PERM ){
        state = O_PLAYER_WINS;
        printf("O wins\n\r");
        return;
    }
    else if (squares[0] == X_SQUARE_PERM && squares[4] == X_SQUARE_PERM && squares[8] == X_SQUARE_PERM ){
        state = X_PLAYER_WINS;
        printf("X wins\n\r");
        return;
    }

    // If the board is full and nobody won, they tied
    if (board_full == 1){
        state = TIE;
        printf("Tie\n\r");
    }

}

/**
 * @brief
 * Initializes the PSoC6 Peripherals used for HW01
 */
void hw02_peripheral_init(void)
{
    /* Initialize the pushbuttons */
    push_buttons_init();

    /* Initialize the LCD */
    ece353_enable_lcd();

    /* Initialize the joystick*/
    joystick_init();

    /* Initialize the remote UART */
    remote_uart_init();

    remote_uart_tx_interrupts_init();
    
    remote_uart_rx_interrupts_init();

    /* Initialize Timer to generate interrupts every 100mS*/
    timer_init(&timer_obj_2, &timer_cfg_2, TICKS_MS_100, Handler_Timer_2);
}

/**
 * @brief
 * Implements the main application for HW02
 */
void hw02_main_app(void)
{
    static joystick_position_t input_pos = JOYSTICK_POS_CENTER;
    uint8_t o_loc = 5;
    uint8_t x_loc = 5;
    button_state_t button = BUTTON_INACTIVE;

    uint8_t char_received[1] = {0x00};
    uint8_t other_player_character[1] = {0};
    uint8_t our_player_character[1] = {0};

    while(1)
    {
        if (ALERT_TIMER == ALERT_TIMER_100MS)
        {
            /* Set Interrupt flag back to false*/
            ALERT_TIMER = ALERT_TIMER_NONE;

            /* Save the previous position */
            prev_position = curr_position;

            /* Get the current position */
            curr_position = joystick_get_pos();

            // /* If the joystick was moved from center, get its new position*/
            if (curr_position != prev_position && prev_position == JOYSTICK_POS_CENTER && curr_position != JOYSTICK_POS_CENTER && state != PLAYER1_SELECTION)
            {
                input_pos = curr_position;

                if (input_pos == JOYSTICK_POS_LEFT){
                    printf("moving: ");
                    joystick_print_pos(curr_position);
                    move_left();
                }

                else if (input_pos == JOYSTICK_POS_RIGHT){
                    printf("moving: ");
                    joystick_print_pos(curr_position);
                    move_right();                    
                }

                else if (input_pos == JOYSTICK_POS_UP){
                    printf("moving: ");
                    joystick_print_pos(curr_position);
                    move_up();
                }

                else if (input_pos == JOYSTICK_POS_DOWN){
                    printf("moving: ");
                    joystick_print_pos(curr_position);
                    move_down();   
                }
            }

        }

        if (ALERT_SW1 == ALERT_SW_1){
            printf("button 1 pressed\n");
            button = BUTTON_SW1_PRESSED;
            ALERT_SW1 = ALERT_SW_NONE;
        }

        if (ALERT_SW2 == ALERT_SW_2){
            printf("button 2 pressed\n");
            button = BUTTON_SW2_PRESSED;
            ALERT_SW2 = ALERT_SW_NONE;
        }

        static uint8_t selection_count = 0;
        switch(state)
        {
            case PLAYER1_SELECTION:
                if (selection_count < 1){
                    lcd_select_player1();
                    printf("drew the thing\n\r");
                    selection_count++;
                }
                remote_uart_rx_char_polling(char_received);
                // printf("char_received: %d\n\r", char_received[0]);
                // we are the starting board
                if (char_received[0] == 0x00 && button == BUTTON_SW2_PRESSED && p1_state == INDETERMINATE){
                    if (selection_count < 2){
                        lcd_clear_screen(LCD_COLOR_BLACK);
                        tic_tac_toe_draw_grid();
                        selection_count++;
                        button = BUTTON_INACTIVE;
                        uint8_t msg[2];
                        // char msg[2] = {0x5A, '\n'};
                        memset(msg, 0, 2);
                        msg[0] = 0x5A;
                        msg[1] = '\n';
                        // send the start byte
                        remote_uart_tx_string_polling(msg);
                        printf("transmitted %d to other board hopefully\n\r", msg[0]);
                        our_turn = OUR_TURN;
                    }
                    char check_ack_p1[1] = {0};
                    // wait for the ack byte
                    while (check_ack_p1[0] != 0xF0){
                        remote_uart_rx_char_polling(check_ack_p1);
                        // printf("we received %d\n\r", check_ack_p1[0]);
                    }
                    p1_state = IS_PLAYER_1;
                }
                // if the other board is the starting board
                else if (char_received[0] == 0x5A){
                    // printf("char received: %d\n\r", char_received[0]);
                    printf("received 5A\n");
                    uint8_t ack_p1[2];
                    memset(ack_p1, 0, 2);
                    ack_p1[0] = 0xF0;
                    ack_p1[1] = '\n';
                    // send the ack, switch to the next substate
                    remote_uart_tx_string_polling(ack_p1);
                    printf("sent F0\n");
                    p1_state = IS_NOT_PLAYER_1;
                    our_turn = THEIR_TURN;
                    char_received[0] = 0x00;
                }

                // if we are the first player
                if (p1_state == IS_PLAYER_1){
                    // draw the background
                    static int p1_count = 0;
                    if (p1_count < 1){
                        p1_count++;
                        lcd_draw_rectangle_centered(CENTER_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, LCD_COLOR_WHITE);
                        lcd_draw_image(CENTER_COL, CENTER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, LCD_COLOR_BLACK, LCD_COLOR_WHITE);
                        printf("displaying o\n\r");
                    }
                    // if we press button 1, alternate which player character is displayed
                    if (button == BUTTON_SW1_PRESSED){
                        button = BUTTON_INACTIVE;
                        if (start_player == O_PLAYER){
                            lcd_draw_rectangle_centered(CENTER_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, LCD_COLOR_WHITE);
                            lcd_draw_image(CENTER_COL, CENTER_ROW, X_WIDTH, X_HEIGHT, Bitmaps_X, LCD_COLOR_BLACK, LCD_COLOR_WHITE);
                            printf("displaying x\n\r");
                            start_player = X_PLAYER;
                        }
                        else{
                            printf("switching to o player\n\r");
                            lcd_draw_rectangle_centered(CENTER_COL, CENTER_ROW, SQUARE_SIZE, SQUARE_SIZE, LCD_COLOR_WHITE);
                            lcd_draw_image(CENTER_COL, CENTER_ROW, O_WIDTH, O_HEIGHT, Bitmaps_O, LCD_COLOR_BLACK, LCD_COLOR_WHITE);
                            printf("displaying o\n\r");
                            start_player = O_PLAYER;
                        }
                    }
                    // if button 2 is pressed, start the game!
                    if (button == BUTTON_SW2_PRESSED){
                        if (start_player == O_PLAYER){
                            uint8_t p1_o_send[2] = {0x4F, '\n'};
                            // send the o byte
                            remote_uart_tx_string_polling(p1_o_send);
                            printf("sent 0x4F\n\r");
                            uint8_t check_o_ack[1] = {0};
                            // look for ack
                            while (check_o_ack[0] == 0){
                                remote_uart_rx_char_polling(check_o_ack);
                                // printf("we received %d\n\r", check_o_ack[0]);

                            }
                            our_player_character[0] = 0x4F;
                            other_player_character[0] = 0x58;
                            state = O_PLAYER_TURN;
                        }
                        else{
                            uint8_t p1_x_send[2] = {0x58, '\n'};
                            // send the x byte
                            remote_uart_tx_string_polling(p1_x_send);
                            printf("sent 0x58\n\r");
                            uint8_t check_x_ack[1] = {0};
                            // look for ack
                            while (check_x_ack[0] == 0){
                                remote_uart_rx_char_polling(check_x_ack);
                                // printf("we received %d\n\r", check_x_ack[0]);
                            }
                            our_player_character[0] = 0x58;
                            other_player_character[0] = 0x4F;
                            state = X_PLAYER_TURN;
                        }
                        button = BUTTON_INACTIVE;
                        break;
                    }
                }   
                // if we're not the starting board
                else if (p1_state == IS_NOT_PLAYER_1){
                    static uint8_t not_p1_count = 0;
                    if (not_p1_count < 1){
                        lcd_clear_screen(LCD_COLOR_BLACK);
                        not_p1_count++;
                    }
                    tic_tac_toe_draw_grid();
                    lcd_wait_for_other_player();
                    uint8_t other_char[1] = {0};
                    // look for the other board's starting character
                    while (other_char[0] == 0){
                        remote_uart_rx_char_polling(other_char);
                    }
                    // we start as the character that the other board is not
                    // then we start the game
                    other_player_character[0] = other_char[0];
                    uint8_t other_player_ack[2] = {0xF0, '\n'}; 
                    if (other_player_character[0] == 0x4F){
                        printf("p1 char is 0x4F\n\r");
                        our_player_character[0] = 0x58;
                        remote_uart_tx_string_polling(other_player_ack);
                        if (our_player_character[0] = 0x58){
                            state = X_PLAYER_TURN;
                            break;
                        }
                    }
                    else if (other_player_character[0] == 0x58){
                        printf("p1 char is 0x58\n\r");
                        our_player_character[0] = 0x4F;
                        remote_uart_tx_string_polling(other_player_ack);
                        if (our_player_character[0] = 0x4F){
                            state = O_PLAYER_TURN;
                            break;
                        }
                    }
                }
            break;

            case O_PLAYER_TURN:
                if (our_turn == OUR_TURN){
                    lcd_clear_other_player();
                    tic_tac_toe_draw_grid();
                    draw_squares();
                    // These two if statements set the square we were just on back to what they were before (only really changes anything if we move off)
                    if (squares[o_loc-1] != X_SQUARE_PERM && squares[o_loc-1] != O_SQUARE_PERM){
                        squares[o_loc-1] = SQUARE_OPEN;
                        temp_squares[o_loc-1] = SQUARE_OPEN;
                    }
                    if (squares[o_loc-1] == X_SQUARE_PERM || squares[o_loc-1] == O_SQUARE_PERM){
                        if (temp_squares[o_loc-1] == O_SQUARE_TEMP){
                        }
                        temp_squares[o_loc-1] = SQUARE_OPEN;
                    }

                    o_loc = current_square;
                    // Places a permanent o, changes turns, and checks the win/tie condition
                    if (button == BUTTON_SW1_PRESSED){
                        button = BUTTON_INACTIVE;
                        if (squares[o_loc-1] != X_SQUARE_PERM && squares[o_loc-1] != O_SQUARE_PERM){
                            squares[o_loc-1] = O_SQUARE_PERM;
                            printf("squares[%d] gets O_SQUARE_PERM\n\r", (o_loc));
                            uint8_t move_to_transmit[2] = {0xFF, '\n'};
                            // block to decide which move byte to transmit
                            if (o_loc == 1){
                                move_to_transmit[0] = 0x00;
                            }
                            else if (o_loc == 2){
                                move_to_transmit[0] = 0x01;
                            }
                            else if (o_loc == 3){
                                move_to_transmit[0] = 0x02;
                            }
                            else if (o_loc == 4){
                                move_to_transmit[0] = 0x10;
                            }
                            else if (o_loc == 5){
                                move_to_transmit[0] = 0x11;
                            }
                            else if (o_loc == 6){
                                move_to_transmit[0] = 0x12;
                            }
                            else if (o_loc == 7){
                                move_to_transmit[0] = 0x20;
                            }
                            else if (o_loc == 8){
                                move_to_transmit[0] = 0x21;
                            }
                            else if (o_loc == 9){
                                move_to_transmit[0] = 0x22;
                            }
                            // transmits the move byte
                            remote_uart_tx_string_polling(move_to_transmit);
                            printf("transmitted: %d\n\r", move_to_transmit[0]);
                            uint8_t transmit_ack[1] = {0};

                            // looks for the ack from the other board
                            while(transmit_ack[0] == 0){
                                remote_uart_rx_char_polling(transmit_ack);
                            }
                            // current_square = 5;
                            o_loc = current_square;
                            // state = X_PLAYER_TURN;
                            our_turn = THEIR_TURN;
                            check_win_and_tie();
                            break;
                        }
                    }
                    // Places a temporary o
                    else{
                        temp_squares[o_loc-1] = O_SQUARE_TEMP;
                        break;
                    }
                }
                else{
                    lcd_wait_for_other_player();
                    tic_tac_toe_draw_grid();
                    uint8_t other_player_placement_o[1] = {0xFF};
                    // while we have not received a valid move, poll for what the other board sends
                    while (other_player_placement_o[0] == 0xFF){
                        remote_uart_rx_char_polling(other_player_placement_o);
                    }
                    // if we've received a valid move, send the ACK
                    if (other_player_placement_o[0] != 0xFF){
                        uint8_t other_player_o_ack[2] = {0xF0, '\n'};
                        remote_uart_tx_string_polling(other_player_o_ack);
                    }
                    // This big block basically just tells us where to place the other player's square on our board so we have parity
                    // it also checks to see if someone has won or if there's a tie
                    if (other_player_placement_o[0] == 0x00){
                        squares[0] = X_SQUARE_PERM;
                        current_square = 1;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                    else if (other_player_placement_o[0] == 0x01){
                        squares[1] = X_SQUARE_PERM;
                        current_square = 2;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                    else if (other_player_placement_o[0] == 0x02){
                        squares[2] = X_SQUARE_PERM;
                        current_square = 3;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                    else if (other_player_placement_o[0] == 0x10){
                        squares[3] = X_SQUARE_PERM;
                        current_square = 4;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                    else if (other_player_placement_o[0] == 0x11){
                        squares[4] = X_SQUARE_PERM;
                        current_square = 5;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                    else if (other_player_placement_o[0] == 0x12){
                        squares[5] = X_SQUARE_PERM;
                        current_square = 6;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                    else if (other_player_placement_o[0] == 0x20){
                        squares[6] = X_SQUARE_PERM;
                        current_square = 7;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                    else if (other_player_placement_o[0] == 0x21){
                        squares[7] = X_SQUARE_PERM;
                        current_square = 8;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                    else if (other_player_placement_o[0] == 0x22){
                        squares[8] = X_SQUARE_PERM;
                        current_square = 9;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                }
            break;


            case X_PLAYER_TURN:
                if (our_turn == OUR_TURN){
                    lcd_clear_other_player();
                    tic_tac_toe_draw_grid();
                    draw_squares();
                    // These two if statements set the square we were just on back to what they were before (only really changes anything if we move off)
                    if (squares[x_loc-1] != X_SQUARE_PERM && squares[x_loc-1] != O_SQUARE_PERM){
                        squares[x_loc-1] = SQUARE_OPEN;
                        temp_squares[x_loc-1] = SQUARE_OPEN;
                    }
                    if (squares[x_loc-1] == X_SQUARE_PERM || squares[x_loc-1] == O_SQUARE_PERM){
                        if (temp_squares[x_loc-1] == X_SQUARE_TEMP){
                        }
                        temp_squares[x_loc-1] = SQUARE_OPEN;
                    }
                    
                    // Places a permanent x into the current square, changes turns, and checks the win/tie condition
                    x_loc = current_square;
                    if (button == BUTTON_SW1_PRESSED){
                        button = BUTTON_INACTIVE;
                        if (squares[x_loc-1] != X_SQUARE_PERM && squares[x_loc-1] != O_SQUARE_PERM){
                            squares[x_loc-1] = X_SQUARE_PERM;
                            printf("squares[%d] gets X_SQUARE_PERM\n\r", (x_loc));
                            uint8_t move_to_transmit[2] = {0xFF, '\n'};
                            // Decides what the move to transmit it
                            if (x_loc == 1){
                                move_to_transmit[0] = 0x00;
                            }
                            else if (x_loc == 2){
                                move_to_transmit[0] = 0x01;
                            }
                            else if (x_loc == 3){
                                move_to_transmit[0] = 0x02;
                            }
                            else if (x_loc == 4){
                                move_to_transmit[0] = 0x10;
                            }
                            else if (x_loc == 5){
                                move_to_transmit[0] = 0x11;
                            }
                            else if (x_loc == 6){
                                move_to_transmit[0] = 0x12;
                            }
                            else if (x_loc == 7){
                                move_to_transmit[0] = 0x20;
                            }
                            else if (x_loc == 8){
                                move_to_transmit[0] = 0x21;
                            }
                            else if (x_loc == 9){
                                move_to_transmit[0] = 0x22;
                            }
                            // sends the move
                            remote_uart_tx_string_polling(move_to_transmit);
                            printf("transmitted: %d\n\r", move_to_transmit[0]);
                            uint8_t transmit_ack[1] = {0};
                            // checks for the ACK
                            while(transmit_ack[0] == 0){
                                remote_uart_rx_char_polling(transmit_ack);
                            }
                            // current_square = 5;
                            x_loc = current_square;
                            // state = O_PLAYER_TURN;
                            our_turn = THEIR_TURN;
                            check_win_and_tie();
                            break;
                        }
                    }
                    // Places a temporary x 
                    else{
                        temp_squares[x_loc-1] = X_SQUARE_TEMP;
                        break;
                    }
                }
                else{
                    lcd_wait_for_other_player();
                    tic_tac_toe_draw_grid();
                    uint8_t other_player_placement_x[1] = {0xFF};
                    // while we have not received a valid move, poll for their move
                    while (other_player_placement_x[0] == 0xFF){
                        remote_uart_rx_char_polling(other_player_placement_x);
                    }
                    // if we've received a valid move, send the ACK
                    if (other_player_placement_x[0] != 0xFF){
                        uint8_t other_player_x_ack[2] = {0xF0, '\n'};
                        remote_uart_tx_string_polling(other_player_x_ack);
                    }

                    // This big block basically just tells us where to place the other player's square on our board so we have parity
                    // it also checks to see if someone has won or if there's a tie
                    if (other_player_placement_x[0] == 0x00){
                        squares[0] = O_SQUARE_PERM;
                        current_square = 1;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                    else if (other_player_placement_x[0] == 0x01){
                        squares[1] = O_SQUARE_PERM;
                        current_square = 2;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                    else if (other_player_placement_x[0] == 0x02){
                        squares[2] = O_SQUARE_PERM;
                        current_square = 3;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                    else if (other_player_placement_x[0] == 0x10){
                        squares[3] = O_SQUARE_PERM;
                        current_square = 4;
                        our_turn = OUR_TURN;
                        check_win_and_tie();
                        break;
                    }
                    else if (other_player_placement_x[0] == 0x11){
                        squares[4] = O_SQUARE_PERM;
                        current_square = 5;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                    else if (other_player_placement_x[0] == 0x12){
                        squares[5] = O_SQUARE_PERM;
                        current_square = 6;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                    else if (other_player_placement_x[0] == 0x20){
                        squares[6] = O_SQUARE_PERM;
                        current_square = 7;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                    else if (other_player_placement_x[0] == 0x21){
                        squares[7] = O_SQUARE_PERM;
                        current_square = 8;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                    else if (other_player_placement_x[0] == 0x22){
                        squares[8] = O_SQUARE_PERM;
                        current_square = 9;
                        our_turn = OUR_TURN;
                        draw_squares();
                        check_win_and_tie();
                        break;
                    }
                }
            break;

            case O_PLAYER_WINS:
                // Displays the o player wins splash screen and resets the game when button 2 is released
                lcd_O_wins();
                uint8_t char_start_o_received[1] = {0};
                uint8_t receive_count_o = 0;
                // Poll for the starting byte
                while(receive_count_o < 200){
                    remote_uart_rx_char_polling(char_start_o_received);
                    receive_count_o++;
                }
                printf("");
                // if we press the button and we have not received the byte, we start
                if (button == BUTTON_SW2_PRESSED && char_start_o_received[0] == 0){
                    uint8_t char_start_o_send[2] = {0x5A, '\n'};
                    // send the start byte
                    remote_uart_tx_string_polling(char_start_o_send);
                    printf("send 5A hopefully\n\r");
                    uint8_t char_start_o_ack[1] = {0};
                    // poll for the ACK
                    while(char_start_o_ack[0] != 0xF0){
                        remote_uart_rx_char_polling(char_start_o_ack);
                        printf("");
                    }
                    printf("received F0\n\r");
                    button = BUTTON_INACTIVE;
                    // Reset the game
                    for (int i = 0; i < 9; i++){
                        squares[i] = SQUARE_OPEN;
                        temp_squares[i] = SQUARE_OPEN;
                    }
                    lcd_clear_screen(LCD_COLOR_BLACK);
                    current_square = 5;
                    x_loc = 5;
                    o_loc = 5;
                    our_turn = OUR_TURN;
                    if (our_player_character[0] == 0x4F){ // if we're O
                        state = O_PLAYER_TURN;
                    }
                    else if (our_player_character[0] == 0x58){ // if we're X
                        state = X_PLAYER_TURN;
                    }
                    break;
                }
                // if we're not the starting board
                else if (char_start_o_received[0] == 0x5A){
                    uint8_t char_start_o_ack_send[2] = {0xF0, '\n'};
                    // send the ACK byte
                    remote_uart_tx_string_polling(char_start_o_ack_send);
                    printf("send F0 hopefully\n\r");
                    button = BUTTON_INACTIVE;
                    // reset the game
                    for (int i = 0; i < 9; i++){
                        squares[i] = SQUARE_OPEN;
                        temp_squares[i] = SQUARE_OPEN;
                    }
                    lcd_clear_screen(LCD_COLOR_BLACK);
                    current_square = 5;
                    x_loc = 5;
                    o_loc = 5;
                    our_turn = THEIR_TURN;
                    if (our_player_character[0] == 0x4F){ // if we're O
                        state = O_PLAYER_TURN;
                    }
                    else if (our_player_character[0] == 0x58){ // if we're X
                        state = X_PLAYER_TURN;
                    }
                }   
            break;

            case X_PLAYER_WINS:
                // Displays the x player wins splash screen and resets the game when button 2 is released
                lcd_X_wins();
                uint8_t char_start_x_received[1] = {0};
                uint8_t receive_count_x = 0;
                // Polls for the byte that tells us we're not the starting board
                while(receive_count_x < 200){
                    remote_uart_rx_char_polling(char_start_x_received);
                    receive_count_x++;
                }
                printf("");
                // if we press the button and we have not received the byte, we start
                if (button == BUTTON_SW2_PRESSED && char_start_x_received[0] == 0){
                    uint8_t char_start_x_send[2] = {0x5A, '\n'};
                    // send 0x5A
                    remote_uart_tx_string_polling(char_start_x_send);
                    printf("send 5A hopefully\n\r");
                    uint8_t char_start_x_ack[1] = {0};
                    // looks for the ACK
                    while(char_start_x_ack[0] != 0xF0){
                        remote_uart_rx_char_polling(char_start_x_ack);
                        printf("");
                    }
                    printf("received F0\n\r");
                    button = BUTTON_INACTIVE;

                    // Reset the game
                    for (int i = 0; i < 9; i++){
                        squares[i] = SQUARE_OPEN;
                        temp_squares[i] = SQUARE_OPEN;
                    }
                    lcd_clear_screen(LCD_COLOR_BLACK);
                    current_square = 5;
                    x_loc = 5;
                    o_loc = 5;
                    our_turn = OUR_TURN;
                    if (our_player_character[0] == 0x4F){ // if we're O
                        state = O_PLAYER_TURN;
                    }
                    else if (our_player_character[0] == 0x58){ // if we're X
                        state = X_PLAYER_TURN;
                    }
                    break;
                }
                // If we're not the starting board
                else if (char_start_x_received[0] == 0x5A){
                    uint8_t char_start_x_ack_send[2] = {0xF0, '\n'};
                    // send the ACK byte
                    remote_uart_tx_string_polling(char_start_x_ack_send);
                    printf("send F0 hopefully\n\r");
                    button = BUTTON_INACTIVE;
                    // Resets the game
                    for (int i = 0; i < 9; i++){
                        squares[i] = SQUARE_OPEN;
                        temp_squares[i] = SQUARE_OPEN;
                    }
                    lcd_clear_screen(LCD_COLOR_BLACK);
                    current_square = 5;
                    x_loc = 5;
                    o_loc = 5;
                    our_turn = THEIR_TURN;
                    if (our_player_character[0] == 0x4F){ // if we're O
                        state = O_PLAYER_TURN;
                    }
                    else if (our_player_character[0] == 0x58){ // if we're X
                        state = X_PLAYER_TURN;
                    }
                }   
            break;

            case TIE:
                // Displays the tie splash screen and resets the game when button 2 is released
                lcd_tie();
                uint8_t char_start_tie_received[1] = {0};
                uint8_t receive_count_tie = 0;
                // Polls to see if we have received the byte from the other board telling us they are going to start
                while(receive_count_tie < 200){
                    remote_uart_rx_char_polling(char_start_tie_received);
                    receive_count_tie++;
                }
                printf("");
                // if we press the button and we have not received the byte from the other board, we start
                if (button == BUTTON_SW2_PRESSED && char_start_tie_received[0] == 0){
                    uint8_t char_start_tie_send[2] = {0x5A, '\n'};
                    // Send 0x5A
                    remote_uart_tx_string_polling(char_start_tie_send);
                    printf("send 5A hopefully\n\r");
                    uint8_t char_start_tie_ack[1] = {0};
                    // Look for the ACK
                    while(char_start_tie_ack[0] != 0xF0){
                        remote_uart_rx_char_polling(char_start_tie_ack);
                        printf("");
                    }
                    printf("received F0\n\r");

                    // Resets the game
                    button = BUTTON_INACTIVE;
                    for (int i = 0; i < 9; i++){
                        squares[i] = SQUARE_OPEN;
                        temp_squares[i] = SQUARE_OPEN;
                    }
                    lcd_clear_screen(LCD_COLOR_BLACK);
                    current_square = 5;
                    x_loc = 5;
                    o_loc = 5;
                    our_turn = OUR_TURN;
                    if (our_player_character[0] == 0x4F){ // if we're O
                        state = O_PLAYER_TURN;
                    }
                    else if (our_player_character[0] == 0x58){ // if we're x
                        state = X_PLAYER_TURN;
                    }
                    break;
                }
                // if we're not the starting board
                else if (char_start_tie_received[0] == 0x5A){
                    uint8_t char_start_tie_ack_send[2] = {0xF0, '\n'};
                    // send the ACK
                    remote_uart_tx_string_polling(char_start_tie_ack_send);
                    printf("send F0 hopefully\n\r");
                    button = BUTTON_INACTIVE;
                    for (int i = 0; i < 9; i++){
                        squares[i] = SQUARE_OPEN;
                        temp_squares[i] = SQUARE_OPEN;
                    }
                    // Resets the game
                    lcd_clear_screen(LCD_COLOR_BLACK);
                    current_square = 5;
                    x_loc = 5;
                    o_loc = 5;
                    our_turn = THEIR_TURN;
                    if (our_player_character[0] == 0x4F){ // if we're O
                        state = O_PLAYER_TURN;
                    }
                    else if (our_player_character[0] == 0x58){ // if we're X
                        state = X_PLAYER_TURN;
                    }
                }   
            break;

        }
        
    }
}