#define DISPLAY_WIDTH 20;
#define DISPLAY_HEIGHT 8;


// Display the msg
void gui_display_msg_on_line(uint8_t a_line_number, char* a_msg);

// Displays the given message.
void gui_display_status_msg(char* a_msg);

// Clear the line of chara
void gui_clear_line(uint8_t a_line_number);

// Clear the entire screen.
void gui_clear_screen(void);

// Displays brand name.
void gui_display_brand(void);

// Displays current depth.
void gui_display_current_depth(void);

// display current depth.
void gui_display_depth_rate(void);

// display elapsed time;
void gui_display_elapsed_time(void);

// display alarm status
void gui_alarm_status(void);

// Clears the screen and retrieves all values again.
// Good for when there is a measurement change.
void gui_update_screen(void);