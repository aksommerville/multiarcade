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

/* Render scene.
 */
 
static void fill_row(ma_pixel_t *dst,ma_pixel_t src) {
  int8_t i=96;
  for (;i-->0;dst++) *dst=src;
}
 
static void render_scene(ma_pixel_t *v,uint16_t input) {

  /* Draw a test pattern, four rows each of:
   *  - Pure red
   *  - Pure green
   *  - Pure blue
   *  - White
   *  - Black
   *  - 50% gray.
   * Fills the first 24 rows.
   */
  ma_pixel_t *dstrow=v;
  uint8_t y=0;
  #if MA_PIXELSIZE==16
    for (;y< 4;y++,dstrow+=96) fill_row(dstrow,0x1f00);
    for (;y< 8;y++,dstrow+=96) fill_row(dstrow,0xe007);
    for (;y<12;y++,dstrow+=96) fill_row(dstrow,0x00f8);
    for (;y<16;y++,dstrow+=96) fill_row(dstrow,0xffff);
    for (;y<20;y++,dstrow+=96) fill_row(dstrow,0x0000);
    for (;y<24;y++,dstrow+=96) fill_row(dstrow,0x1084);
  #else
    for (;y< 4;y++,dstrow+=96) fill_row(dstrow,0x03);
    for (;y< 8;y++,dstrow+=96) fill_row(dstrow,0x1c);
    for (;y<12;y++,dstrow+=96) fill_row(dstrow,0xe0);
    for (;y<16;y++,dstrow+=96) fill_row(dstrow,0xff);
    for (;y<20;y++,dstrow+=96) fill_row(dstrow,0x00);
    for (;y<24;y++,dstrow+=96) fill_row(dstrow,0x92);
  #endif
  
  /* Black background for the remainder.
   */
  for (;y<64;y++,dstrow+=96) fill_row(dstrow,0);
  
  /* Show us the pixel size.
   */
  #if MA_PIXELSIZE==16
    v[96*25+1]=0xffff;
    v[96*26+1]=0xffff;
    v[96*27+1]=0xffff;
    v[96*28+1]=0xffff;
    v[96*29+1]=0xffff;
    v[96*25+4]=0xffff;
    v[96*25+5]=0xffff;
    v[96*26+3]=0xffff;
    v[96*27+3]=0xffff;
    v[96*27+4]=0xffff;
    v[96*28+3]=0xffff;
    v[96*28+5]=0xffff;
    v[96*29+4]=0xffff;
  #else
    v[96*25+2]=0xff;
    v[96*26+1]=0xff;
    v[96*26+3]=0xff;
    v[96*27+2]=0xff;
    v[96*28+1]=0xff;
    v[96*28+3]=0xff;
    v[96*29+2]=0xff;
  #endif
  
  /* Show us the input.
   * 9x5 pixels
   */
  ma_pixel_t *inpic=v+96*25+7;
  #if MA_PIXELSIZE==16
    ma_pixel_t bgcolor=0x1084;
    ma_pixel_t fgcolor=0x0002;
  #else
    ma_pixel_t bgcolor=0x92;
    ma_pixel_t fgcolor=0x08;
  #endif
  uint8_t _y=0;
  for (;_y<5;_y++) {
    uint8_t _x=0;
    for (;_x<9;_x++) {
      inpic[96*_y+_x]=bgcolor;
    }
  }
  if (input&MA_BUTTON_UP)    inpic[96*1+2]=fgcolor;
  if (input&MA_BUTTON_DOWN)  inpic[96*3+2]=fgcolor;
  if (input&MA_BUTTON_LEFT)  inpic[96*2+1]=fgcolor;
  if (input&MA_BUTTON_RIGHT) inpic[96*2+3]=fgcolor;
  if (input&MA_BUTTON_A)     inpic[96*2+7]=fgcolor;
  if (input&MA_BUTTON_B)     inpic[96*2+5]=fgcolor;
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
  
  render_scene(image_fb.v,input);
  
  #if 0
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
  #endif
  
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
