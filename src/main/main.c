#include "multiarcade.h"
#include <stdio.h>
#include <string.h>

/* Globals.
 */
 
MA_IMAGE_DECLARE(fb,96,64,0,0)

static ma_pixel_t nextpixel=0;
static uint16_t nextpixelp=0;

static int16_t aulevel=2000; // toggles positive/negative
static uint16_t auhalfperiod=0; // 0=silent
static uint16_t aup=0;

/* Audio.
 */
 
int16_t audio_next() {
  if (auhalfperiod) {
    if (aup++>=auhalfperiod) {
      aup=0;
      aulevel=-aulevel;
    }
    return aulevel;
  } else {
    return 0;
  }
}

/* Update.
 */
 
void loop() {
  uint16_t input=ma_update();
  
       if (input&MA_BUTTON_B) auhalfperiod=20;
  else if (input&MA_BUTTON_A) auhalfperiod=30;
  else if (input&MA_BUTTON_UP) auhalfperiod=40;
  else if (input&MA_BUTTON_DOWN) auhalfperiod=60;
  else if (input&MA_BUTTON_LEFT) auhalfperiod=80;
  else if (input&MA_BUTTON_RIGHT) auhalfperiod=100;
  else auhalfperiod=0;
  
  #if MA_PIXELSIZE==16
    ma_pixel_t checka=0x8410;
    ma_pixel_t checkb=0xc618;
    ma_pixel_t white=0xffff;
  #else
    ma_pixel_t checka=0x92;
    ma_pixel_t checkb=0xdb;
    ma_pixel_t white=0xff;
  #endif
  ma_pixel_t *dst=image_fb.v;
  int yi=image_fb.h;
  while (yi-->0) {
    int xi=image_fb.w;
    for (;xi-->0;dst++) {
      *dst=((xi&1)==(yi&1))?checka:checkb;
    }
  }
  
  #if MA_PIXELSIZE==8
    for (dst=image_fb.v,yi=0;yi<256;yi++,dst++) *dst=yi;
  #endif
  
  if (input&MA_BUTTON_UP) {
    image_fb.v[21*96+32]=white;
    image_fb.v[22*96+31]=white;
    image_fb.v[22*96+33]=white;
  }
  if (input&MA_BUTTON_DOWN) {
    image_fb.v[43*96+32]=white;
    image_fb.v[42*96+31]=white;
    image_fb.v[42*96+33]=white;
  }
  if (input&MA_BUTTON_LEFT) {
    image_fb.v[32*96+20]=white;
    image_fb.v[31*96+21]=white;
    image_fb.v[33*96+21]=white;
  }
  if (input&MA_BUTTON_RIGHT) {
    image_fb.v[32*96+44]=white;
    image_fb.v[31*96+43]=white;
    image_fb.v[33*96+43]=white;
  }
  if (input&MA_BUTTON_A) {
    image_fb.v[32*96+80]=white;
  }
  if (input&MA_BUTTON_B) {
    image_fb.v[32*96+60]=white;
  }
  
  ma_send_framebuffer(image_fb.v);
}

/* Setup.
 */
 
void setup() {
  //fprintf(stderr,"%s\n",__func__);
  struct ma_init_params params={
    .videow=image_fb.w,
    .videoh=image_fb.h,
    .rate=60, // request that loop() be called at 60 Hz
    .audio_rate=22050,
  };
  if (!ma_init(&params)) return;
  if ((params.videow!=image_fb.w)||(params.videoh!=image_fb.h)) return;
}
