#include <assert.h>
#include <stdint.h>				
#include <stdio.h>

#include "os.h"
#include "iorx63n.h"

#include "adc.h"
#include  <bsp_glcd.h>
#include "data_access_layer.h"


#define DISPLAY_WIDTH 19
#define DISPLAY_HEIGHT 7

#define LINE_BRAND 0

#define LINE_DEPTH 2
#define LINE_RATE 3
#define LINE_AIR_VOL 4
#define LINE_ELAPSED_DIVE_TIME 5
#define LINE_HIGH_ALARM 7


static char BLANK_LINE[20] = "                    ";


// Display the msg
void gui_display_msg_on_line(uint8_t a_line_number, char* a_msg)
{
  BSP_GraphLCD_String(a_line_number, a_msg);
}


// Displays the given message.
void gui_display_status_msg(char* a_msg)
{
  int h = DISPLAY_HEIGHT;
  gui_display_msg_on_line(h, a_msg);
}


// Clear the line of chara
void gui_clear_line(uint8_t a_line_number)
{
  int n = DISPLAY_WIDTH;
  
  for(int i = 0; i < n; i++)
  {
     gui_display_msg_on_line(i, BLANK_LINE);
  }
}

// Clear the entire screen.
void gui_clear_screen()
{
  int n = DISPLAY_HEIGHT;
  
  for( int i = 0; i < n; i++)
  {
    gui_clear_line(i);
  }
}

// Displays brand name.
void gui_display_brand(void)
{
  gui_display_msg_on_line(0, "AQUA NOTS!");
}

// Displays current depth.
void gui_display_current_depth(void)
{
  float depth = dal_get_depth(); // in millimeters
  uint8_t units = dal_get_units_toggle();
  char msg[DISPLAY_WIDTH];
  if(units == DAL_UNITS_METRIC)
  {
    depth = depth/1000.0; // to meters
  }
  else
  {
    depth = depth*0.00328084; // feet
  }
  
  sprintf(msg, "DEPTH: %4.2f %s", depth, (units==DAL_UNITS_METRIC)?"M":"FT");
  gui_display_msg_on_line(LINE_DEPTH, msg);
}

// display current depth.
void gui_display_depth_rate(void)
{
  float rate = dal_get_depth_rate();
  uint8_t units = dal_get_units_toggle();
  
  char msg[DISPLAY_WIDTH];
  if(units == DAL_UNITS_METRIC)
  {
    rate = rate; // to meters
  }
  else
  {
    rate = rate*3.28084; // feet
  }
  
  sprintf(msg, "RATE: %4.2f %s", rate, (units==DAL_UNITS_METRIC)?"M":"FT");
  gui_display_msg_on_line(LINE_RATE, msg);
  
}


// display elapsed time;
void gui_display_elapsed_time(void)
{
    // this is in seconds
    uint32_t tick = dal_get_tick();

    uint32_t hours = tick/3600;
    uint32_t min = (tick - hours*3600)/60;
    uint32_t sec = tick - hours*3600 - min*60;

    char buf[32] = {'\0'};
    snprintf(buf, 32, "EDT : %2u:%02u:%02u", hours, min, sec);

    BSP_GraphLCD_ClrLine(6);
    BSP_GraphLCD_String(6, buf);
}

// display alarm status
void gui_alarm_status(void)
  {}

// Clears the screen and retrieves all values again.
// Good for when there is a measurement change.
void gui_update_screen(void)
  {}
  



