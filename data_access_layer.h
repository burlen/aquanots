#ifndef data_access_layer_h
#define data_access_layer_h

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <os.h>

#define DANGEROUS_ASCENT_RATE_M_MIN (15)
data_access_layer *g_dal = NULL;
#define VOLUME_PER_CLICK_IN_MILLILITRES (5000)
// initialize the dal
void dal_intialize();

// reset the data to initial values
void dal_reset();

// set/get the clock
// in units of seconds
void dal_set_tick(uint32_t tick);
uint32_t dal_get_tick();
// block unit the value has changed, return 0 on success
// -1 on error
int wait_tick_changed();


// set/get alarm state
void dal_set_alarm(uint8_t val);
uint8_t dal_get_alarm(void);
// block unit the value has changed, return 0 on success
// -1 on error
int wait_alarm_changed();

// set/get the units
typedef enum
{
  DAL_UNITS_METRIC=0x1,
  DAL_UNITS_US=0x2
}
units_toggle_t;
void dal_set_units_toggle(uint8_t val);
uint8_t dal_get_units_toggle();
// block unit the value has changed, return 0 on success
// -1 on error
int wait_units_toggle_changed();

// set/get the brand name strung
void dal_set_brand_name(const char *vakl);
const char *dal_get_brand_name();

// set/get the depth rate
// values are in units of meters/min
// values are in units of +/- meters/min
void dal_set_depth_rate(int16_t val);
int16_t dal_get_depth_rate();
// block unit the value has changed, return 0 on success
// -1 on error
int wait_depth_rate_change();

// set/get the depth rate
// values are milimeters
void dal_set_depth(uint32_t val);
uint32_t dal_get_depth();
// block unit the value has changed, return 0 on success
// -1 on error
int wait_depth_change();

// set/get the current air volume
// units of millileters
// units of milliliters
void dal_set_air_volume(uint16_t val);
// block unit the value has changed, return 0 on success
// -1 on error
uint16_t dal_get_air_volume();
void dal_add_air_volume_in_Millilitres(uint32_t volume);
void dal_add_air_volume_in_Millilitres(uint32_t total_vol);

// block unit the value has changed, return 0 on success
// -1 on error
int wait_air_volume_changed();

// set/get the debug flag state
void dal_set_debug(uint8_t val);
uint8_t dal_get_debug();

#endif
