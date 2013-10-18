
#ifndef _edm_h
#define _edm_h

/* variables defined in edm.c and set in boardfile */

#define EDM_N_EXTERNAL_GPIO 10

extern int edm_external_gpio[EDM_N_EXTERNAL_GPIO];
extern int edm_i2c[3];
extern int edm_ddc;

extern void *edm_analog_audio_platform_data;

#endif
