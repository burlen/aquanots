#ifndef data_access_layer_h
#define data_access_layer_h

typedef struct
{



} data_access_layer_t;


data_access_layer _t *new_data_access_layer();
void free_data_access_layer();

void dal_reset();


void dal_set_tick(uint32_t tick);
uint32_t dal_get_tick(void);

void dal_set_alarm(uint8_t val);
uint8_t dal_get_alarm(void);

void dal_set_uits_toggle(uint8_t val);
uint8_t dal_get_units_toggle();

void dal_set_brand_name(const char *vakl);
const char *dal_get_brand_name();

void dal_set_depth_rate(int16_t val);
int16_t dal_get_depth_rate();

void dal_set_depth(uint16_t val);
uint16_t dal_get_depth();


void dal_set_air_volume(uint16_t val);
uint16_t dal_get_air_volume();

void dal_set_debug(uint8_t val);
uint8_t dal_get_debug();

void dal_set_();
dal_get_();

#endif
