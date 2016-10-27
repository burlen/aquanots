#include "data_access_layer.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

// internal singleton
data_access_layer_t *g_dal = NULL;


// --------------------------------------------------------------------------
data_access_layer_t *new_data_access_layer()
{
    data_access_layer_t *dal =
        (data_access_layer_t*)malloc(sizeof(data_access_layer_t));

    memset(dal, 0, sizeof(data_access_layer_t));

    OS_ERR err;
    OSSemCreate(&dal->any_sem, "any sem", 0, &err);
    assert(OS_ERR_NONE == err);
    OSMutexCreate(&dal->all_mut, "all mutex", &err);
    assert(OS_ERR_NONE == err);

    OSSemCreate(&dal->tick_sem, "tick_sem", 0, &err);
    assert(OS_ERR_NONE == err);
    OSMutexCreate(&dal->tick_mut, "tick_mutex", &err);
    assert(OS_ERR_NONE == err);

    OSSemCreate(&dal->alarm_sem, "alarm_sem", 0, &err);
    assert(OS_ERR_NONE == err);
    OSMutexCreate(&dal->alarm_mut, "alarm_mutex", &err);
    assert(OS_ERR_NONE == err);

    OSSemCreate(&dal->units_toggle_sem, "units_toggle_sem", 0, &err);
    assert(OS_ERR_NONE == err);
    OSMutexCreate(&dal->units_toggle_mut, "units_toggle_mutex", &err);
    assert(OS_ERR_NONE == err);

    OSSemCreate(&dal->depth_sem, "depth_sem", 0, &err);
    assert(OS_ERR_NONE == err);
    OSMutexCreate(&dal->depth_mut, "depth_mutex", &err);
    assert(OS_ERR_NONE == err);

    OSSemCreate(&dal->depth_rate_sem, "depth_rate_sem", 0, &err);
    assert(OS_ERR_NONE == err);
    OSMutexCreate(&dal->depth_rate_mut, "depth_rate_mutex", &err);
    assert(OS_ERR_NONE == err);

    OSSemCreate(&dal->air_volume_sem, "air_volume_sem", 0, &err);
    assert(OS_ERR_NONE == err);
    OSMutexCreate(&dal->air_volume_mut, "air_volume_mutex", &err);
    assert(OS_ERR_NONE == err);

    OSSemCreate(&dal->brand_name_sem, "brand_name_sem", 0, &err);
    assert(OS_ERR_NONE == err);
    OSMutexCreate(&dal->brand_name_mut, "brand_name_mutex", &err);
    assert(OS_ERR_NONE == err);

    OSSemCreate(&dal->debug_sem, "debug_sem", 0, &err);
    assert(OS_ERR_NONE == err);
    OSMutexCreate(&dal->debug_mut, "debug_mutex", &err);
    assert(OS_ERR_NONE == err);

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
{
    data_access_layer_t *dal = g_dal;
    // TODO -- call setters

}

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
{
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


