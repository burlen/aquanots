#include "data_access_layer.h"
#include  <bsp_glcd.h> // for LCD

#include <stdlib.h>
#include <assert.h>
#include <string.h>


// structure for shared data, a singleton instance, and
// set/get api
typedef struct
{
    OS_SEM any_sem;
    OS_MUTEX all_mut;

    uint32_t tick;
    OS_MUTEX tick_mut;
    OS_SEM tick_sem;

    uint8_t alarm;
    OS_MUTEX alarm_mut;
    OS_SEM alarm_sem;

    uint8_t units_toggle;
    OS_MUTEX units_toggle_mut;
    OS_SEM units_toggle_sem;

    uint32_t depth;
    OS_MUTEX depth_mut;
    OS_SEM depth_sem;

    uint16_t depth_rate;
    OS_MUTEX depth_rate_mut;
    OS_SEM depth_rate_sem;

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

// constructor
data_access_layer_t *new_data_access_layer();

// destructor
void free_data_access_layer();
// internal singleton
data_access_layer_t *g_dal = NULL;

// --------------------------------------------------------------------------
int dal_signal_update(OS_SEM *sem)
{
    OS_ERR err;
    OSSemPost(sem, OS_OPT_POST_NO_SCHED, &err);
    assert(OS_ERR_NONE == err);
    return 0;
}

// --------------------------------------------------------------------------
int dal_mutex_aquire(OS_MUTEX *mut)
{
    OS_ERR err;
    OSMutexPend(mut, 0, OS_OPT_PEND_BLOCKING,  NULL, &err);
    assert(OS_ERR_NONE == err);
    return 0;
}

// --------------------------------------------------------------------------
int dal_mutex_release(OS_MUTEX *mut)
{
    OS_ERR err;
    OSMutexPost(mut, OS_OPT_POST_NONE, &err);
    assert(OS_ERR_NONE == err);
    return 0;
}


// --------------------------------------------------------------------------
int dal_mutex_create(OS_MUTEX *mut, const char *name)
{
    OS_ERR err;
    OSMutexCreate(mut, (char*)name, &err);
    assert(OS_ERR_NONE == err);
    return 0;
}

// --------------------------------------------------------------------------
int dal_sem_create(OS_SEM *sem, const char *name)
{
    OS_ERR err;
    OSSemCreate(sem, (char*)name, 0, &err);
    assert(OS_ERR_NONE == err);
    return 0;
}


// --------------------------------------------------------------------------
data_access_layer_t *new_data_access_layer()
{
    data_access_layer_t *dal =
        (data_access_layer_t*)malloc(sizeof(data_access_layer_t));

    // initialize it all to zero
    memset(dal, 0, sizeof(data_access_layer_t));

    dal_sem_create(&dal->any_sem, "any_sem");
    dal_mutex_create(&dal->all_mut, "all_mut");

    dal_sem_create(&dal->tick_sem, "tick_sem");
    dal_mutex_create(&dal->tick_mut, "tick_mutex");

    dal_sem_create(&dal->alarm_sem, "alarm_sem");
    dal_mutex_create(&dal->alarm_mut, "alarm_mutex");

    dal_sem_create(&dal->units_toggle_sem, "units_toggle_sem");
    dal_mutex_create(&dal->units_toggle_mut, "units_toggle_mutex");

    dal_sem_create(&dal->depth_sem, "depth_sem");
    dal_mutex_create(&dal->depth_mut, "depth_mutex");

    dal_sem_create(&dal->depth_rate_sem, "depth_rate_sem");
    dal_mutex_create(&dal->depth_rate_mut, "depth_rate_mutex");

    dal_sem_create(&dal->air_volume_sem, "air_volume_sem");
    dal_mutex_create(&dal->air_volume_mut, "air_volume_mutex");

    dal_sem_create(&dal->brand_name_sem, "brand_name_sem");
    dal_mutex_create(&dal->brand_name_mut, "brand_name_mutex");

    dal_sem_create(&dal->debug_sem, "debug_sem");
    dal_mutex_create(&dal->debug_mut, "debug_mutex");

    return dal;
}

// --------------------------------------------------------------------------
void free_data_access_layer()
{
    // TODO -- this data structure's life time is infinite
    // so this is alow priority
}

// --------------------------------------------------------------------------
void dal_initialize()
{
    if (!g_dal)
        g_dal = new_data_access_layer();

    dal_set_tick(0);
    dal_set_alarm(0);
    dal_set_units_toggle(0);
    dal_set_depth(0);
    dal_set_depth_rate(0);
    dal_set_air_volume(0);
    dal_set_brand_name("aquanots");
    dal_set_debug(0);
}

// --------------------------------------------------------------------------
void dal_reset()
{ dal_initialize(); }

// --------------------------------------------------------------------------
void dal_set_tick(uint32_t val)
{
    data_access_layer_t *dal = g_dal;

    // aquire the mutex
    dal_mutex_aquire(&dal->tick_mut);

    // set the value
    dal->tick = val;

    // signal the update
    dal_signal_update(&dal->any_sem);
    dal_signal_update(&dal->tick_sem);

    // release the mutex
    dal_mutex_release(&dal->tick_mut);
}

// --------------------------------------------------------------------------
uint32_t dal_get_tick()
{
    data_access_layer_t *dal = g_dal;

    // aquire the mutex
    dal_mutex_aquire(&dal->tick_mut);

    // get the value
    uint32_t val = dal->tick;

    // release the mutex
    dal_mutex_release(&dal->tick_mut);

    return val;
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


