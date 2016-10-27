#include "data_access_layer.h"
#include  <bsp_glcd.h> // for LCD

#include <stdlib.h>

// internal singleton
data_access_layer_t *g_dal = NULL;

// --------------------------------------------------------------------------
data_access_layer_t *new_data_access_layer()
{
    data_access_layer_t *dal =
        (data_access_layer_t*)malloc(sizeof(data_access_layer_t));
    return dal;
}

// --------------------------------------------------------------------------
void free_data_access_layer()
{
    // TODO -- this data structure's life time is infinite
    // so this is alow priority
}

// --------------------------------------------------------------------------
void dal_reset()
{}

// --------------------------------------------------------------------------
void dal_set_tick(uint32_t tick)
{
}

// --------------------------------------------------------------------------
uint32_t dal_get_tick()
{
    return 0;
}

// --------------------------------------------------------------------------
void dal_set_alarm(uint8_t val)
{
}

// --------------------------------------------------------------------------
uint8_t dal_get_alarm(void)
{
    return  0;
}


// --------------------------------------------------------------------------
void dal_set_units_toggle(uint8_t val)
{
}

// --------------------------------------------------------------------------
uint8_t dal_get_units_toggle()
{
    return 0;
}


// --------------------------------------------------------------------------
void dal_set_brand_name(const char *vakl)
{
}

// --------------------------------------------------------------------------
const char *dal_get_brand_name()
{
    return "";
}


// --------------------------------------------------------------------------
void dal_set_depth_rate(int16_t val)
{
#define DISPLAY_DEBUG
#ifdef DISPLAY_DEBUG
  {
    char msg[32];
    sprintf(&msg[0], "depth: %4i", val);
    BSP_GraphLCD_String(0, (const char *)&msg);
  }
#endif
}


// --------------------------------------------------------------------------
int16_t dal_get_depth_rate()
{
    return 0;
}


// --------------------------------------------------------------------------
void dal_set_depth(uint32_t val)
{
}

// --------------------------------------------------------------------------
uint32_t dal_get_depth()
{
    return 0;
}

// --------------------------------------------------------------------------
void dal_set_air_volume(uint16_t val)
{
}

// --------------------------------------------------------------------------
uint16_t dal_get_air_volume()
{
    return 0;
}

// --------------------------------------------------------------------------
void dal_add_air_volume_in_Millilitres(uint32_t vol)
void dal_add_air_volume_in_Millilitres(uint32_t vol_delta)
{
#ifdef DISPLAY_DEBUG
  {
    char msg[32];
    static uint32_t total_vol=0;
    total_vol += vol_delta;
    sprintf(&msg[0], "vol: %4u", total_vol);
    BSP_GraphLCD_String(0, (const char *)&msg);
  }
#endif
}
// --------------------------------------------------------------------------
void dal_set_debug(uint8_t val)
{
}

// --------------------------------------------------------------------------
uint8_t dal_get_debug()
{
    return 0;
}


