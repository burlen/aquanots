#ifndef data_access_layer_h
#define data_access_layer_h

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>


// structure for shared data, a singleton instance, and
// set/get api
typedef struct
{
    uint32_t tick;
    uint8_t alarm;
    uint8_t units_toggle;
    uint16_t depth;
    uint16_t air_volume;
    const char *brand_name;
    uint8_t debug;
}
data_access_layer_t;

data_access_layer *g_dal = NULL;

// constructor
data_access_layer_t *new_data_access_layer();

// destructor
void free_data_access_layer();

// reset the data to initial values
void dal_reset();

// set/get the clock
// in units of seconds
void dal_set_tick(uint32_t tick);
uint32_t dal_get_tick(void);

// set/get alarm state
void dal_set_alarm(uint8_t val);
uint8_t dal_get_alarm(void);

// set/get the units
#define UNITS_METRIC 0x01
#define UNITS_US     0x02
void dal_set_units_toggle(uint8_t val);
uint8_t dal_get_units_toggle();

// set/get the brand name strung
void dal_set_brand_name(const char *vakl);
const char *dal_get_brand_name();

// set/get the depth rate
// values are in units of meters/min
void dal_set_depth_rate(int16_t val);
int16_t dal_get_depth_rate();

// set/get the depth rate
// values are milimeters
void dal_set_depth(uint32_t val);
uint32_t dal_get_depth();

// set/get the current air volume
// units of millileters
void dal_set_air_volume(uint16_t val);
uint16_t dal_get_air_volume();

// set/get the debug flag state
void dal_set_debug(uint8_t val);
uint8_t dal_get_debug();

#endif
