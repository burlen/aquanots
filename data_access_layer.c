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

    int16_t depth_rate;
    OS_MUTEX depth_rate_mut;
    OS_SEM depth_rate_sem;

    uint32_t air_volume;
    OS_MUTEX air_volume_mut;
    OS_SEM air_volume_sem;

    char *brand_name;
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
    dal_set_air_volume(50);
    dal_set_brand_name("aquanots");
    dal_set_debug(0);
}

// --------------------------------------------------------------------------
void dal_reset()
{ dal_initialize(); }

// --------------------------------------------------------------------------

#define dal_set_get_impl(var_name, var_type)                        \
dal_set_impl(var_name, var_type)                                    \
dal_get_impol(var_name, var_type)

#define  dal_set_impl(var_name, var_type)                           \
void dal_set_ ## var_name (var_type val)                            \
{                                                                   \
    data_access_layer_t *dal = g_dal;                               \
                                                                    \
    /* aquire the mutex */                                          \
    dal_mutex_aquire(&dal-> ## var_name ## _mut);                   \
                                                                    \
    /* set the value  */                                            \
    dal-> ## var_name = val;                                        \
    /*printf("dal_set_" # var_name "(%d)\n", (int)val); */              \
                                                                    \
    /* signal the update */                                         \
    dal_signal_update(&dal->any_sem);                               \
    dal_signal_update(&dal-> ## var_name ## _sem);                  \
                                                                    \
    /* release the mutex */                                         \
    dal_mutex_release(&dal-> ## var_name ## _mut);                  \
}

#define  dal_get_impl(var_name, var_type)                           \
var_type dal_get_ ## var_name ()                                    \
{                                                                   \
    data_access_layer_t *dal = g_dal;                               \
                                                                    \
    /* aquire the mutex */                                          \
    dal_mutex_aquire(&dal-> ## var_name ## _mut);                   \
                                                                    \
    /* get the value */                                             \
    var_type val = dal-> ## var_name;                               \
    /*printf("dal_get_" # var_name "(%d)\n", (int)val); */              \
                                                                    \
    /* release the mutex */                                         \
    dal_mutex_release(&dal-> ## var_name ## _mut);                  \
                                                                    \
    return val;                                                     \
}

dal_set_impl(tick, uint32_t);
dal_set_impl(alarm, uint8_t);
dal_set_impl(units_toggle, uint8_t);
dal_set_impl(depth_rate, int16_t);
dal_set_impl(depth, uint32_t);
dal_set_impl(debug, uint8_t);
dal_set_impl(air_volume, uint32_t);

dal_get_impl(tick, uint32_t);
dal_get_impl(alarm, uint8_t);
dal_get_impl(units_toggle, uint8_t);
dal_get_impl(depth_rate, int16_t);
dal_get_impl(depth, uint32_t);
dal_get_impl(air_volume, uint32_t);
dal_get_impl(brand_name, char *);
dal_get_impl(debug, uint8_t);

// --------------------------------------------------------------------------
void dal_set_brand_name(char *val)
{
    data_access_layer_t *dal = g_dal;

    // aquire the mutex
    dal_mutex_aquire(&dal->brand_name_mut);

    // if there was already something free it
    // no leaks
    if (dal->brand_name)
        free(dal->brand_name);

    // set the value
    dal->brand_name = strdup(val);

    // signal the update
    dal_signal_update(&dal->any_sem);
    dal_signal_update(&dal->brand_name_sem);

    // release the mutex
    dal_mutex_release(&dal->brand_name_mut);
}

// --------------------------------------------------------------------------
void dal_add_air_volume(uint32_t val)
{
    data_access_layer_t *dal = g_dal;

    /* aquire the mutex */
    dal_mutex_aquire(&dal->air_volume_mut);

    /* set the value  */
    dal->air_volume += val;

    /* signal the update */
    dal_signal_update(&dal->any_sem);
    dal_signal_update(&dal->air_volume_sem);

    /* release the mutex */
    dal_mutex_release(&dal->air_volume_mut);
}
