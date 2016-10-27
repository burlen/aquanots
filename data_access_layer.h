#ifndef data_access_layer_h
#define data_access_layer_h

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include <os.h>

#define DANGEROUS_ASCENT_RATE_M_MIN (15)
#define VOLUME_PER_CLICK_IN_MILLILITRES (5000)

// structure for shared data, a singleton instance, and
// set/get api
typedef struct
{
    OS_SEM any_sem;
    OS_MUTEX all_mutex;

    uint32_t tick;
    OS_MUTEX tick_mut;
    OS_SEM tick_sem;

    uint8_t alarm;
    OS_MUTEX alarm_mut;
    OS_SEM alarm_sem;

    uint8_t units_toggle;
    OS_MUTEX units_mut;
    OS_SEM units_sem;

    uint16_t depth;
    OS_MUTEX depth_mut;
    OS_SEM depth_sem;

    uint16_t air_volume;
    OS_MUTEX air_volume_mut;
    OS_SEM air_volume_sem;

    const char *brand_name;
    OS_MUTEX brand_name_mut;
    OS_SEM brand_name_sem;

    uint8_t debug;
    OS_MUTEX debug_mut;
    OS_SEM debug_sem;
}
data_access_layer_t;


typedef enum
{
  UNITS_METRIC=0,
  UNITS_US
}
units_toggle_t;


// constructor
data_access_layer_t *new_data_access_layer();

// destructor
void free_data_access_layer();

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
#define UNITS_METRIC 0x01
#define UNITS_US     0x02
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
