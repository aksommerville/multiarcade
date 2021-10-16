#include "multiarcade.h"
#include <stdio.h>
#include <string.h>

extern struct ma_image image_tiles;
extern struct ma_font font_basic;

/* Globals.
 */
 
MA_IMAGE_DECLARE(fb,96,64,0,0)

static struct ma_image tile_floor,tile_box,tile_man;

static int16_t aulevel=2000; // toggles positive/negative
static uint16_t auhalfperiod=0; // 0=silent
static uint16_t aup=0;

static int16_t manx=38,many=43;
static int16_t mandx=0,mandy=0;
static uint16_t mandxtime=0,mandytime=0;

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
  
  /* Copy some tiles.
   */
  ma_image_blit_unchecked(&image_fb,20,25,&tile_floor);
  ma_image_blit_unchecked(&image_fb,32,25,&tile_floor);
  ma_image_blit_unchecked(&image_fb,44,25,&tile_box);
  ma_image_blit_unchecked(&image_fb,20,37,&tile_floor);
  ma_image_blit_unchecked(&image_fb,32,37,&tile_floor);
  ma_image_blit_unchecked(&image_fb,44,37,&tile_floor);
  ma_image_blit_unchecked(&image_fb,20,49,&tile_box);
  ma_image_blit_unchecked(&image_fb,32,49,&tile_floor);
  ma_image_blit_unchecked(&image_fb,44,49,&tile_floor);
  
  /* And finally draw the sprites.
   */
  ma_image_blit_unchecked(&image_fb,manx-6,many-6,&tile_man);
  
  /* Oooh and also, some text.
   */
  ma_font_render(&image_fb,57,24,&font_basic,"Text!",5,MA_PIXEL(0xff,0xff,0xff));
  ma_font_render(&image_fb,57,32,&font_basic,"blue?",5,MA_PIXEL(0x00,0x00,0xff));
  ma_font_render(&image_fb,57,40,&font_basic,"GREEN",6,MA_PIXEL(0x00,0xff,0x00));
  ma_font_render(&image_fb,57,48,&font_basic,"Red.",4,MA_PIXEL(0xff,0x00,0x00));
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
  
  manx+=mandx;
  if (manx<26) { manx=26; if (mandx<0) mandx=-mandx; }
  else if (manx>50) { manx=50; if (mandx>0) mandx=-mandx; }
  if (mandxtime) mandxtime--;
  else { mandx=rand()%3-1; mandxtime=30+rand()%100; }
  many+=mandy;
  if (many<31) { many=31; if (mandy<0) mandy=-mandy; }
  else if (many>55) { many=55; if (mandy>0) mandy=-mandy; }
  if (mandytime) mandytime--;
  else { mandy=rand()%3-1; mandytime=30+rand()%100; }
  
  render_scene(image_fb.v,input);
  
  ma_send_framebuffer(image_fb.v);
}

/* Slice tiles from the sheet.
 */
 
static void init_tiles() {
  const uint8_t colc=4;
  const uint8_t rowc=4;
  const uint8_t colw=image_tiles.w/colc;
  const uint8_t rowh=image_tiles.h/rowc;
  #define TILE(name,col,row) ma_image_crop(&tile_##name,&image_tiles,col*colw,row*rowh,colw,rowh);
  TILE(floor,0,0)
  TILE(box,1,0)
  TILE(man,0,1)
  #undef TILE
}

/* Setup.
 */
 
void setup() {
  init_tiles();
  struct ma_init_params params={
    .videow=image_fb.w,
    .videoh=image_fb.h,
    .rate=60,
    .audio_rate=22050,
  };
  if (!ma_init(&params)) return;
  if ((params.videow!=image_fb.w)||(params.videoh!=image_fb.h)) return;
  srand(millis());
}
