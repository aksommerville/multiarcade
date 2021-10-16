/* multiarcade.h
 */
 
#ifndef MULTIARCADE_H
#define MULTIARCADE_H

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
  extern "C" {
#endif

/* Pixel size for all images must be established at compile time,
 * in the interest of reducing code size.
 * May be 8, 16, or 32. TinyArcade only supports 8 or 16.
 */
#ifndef MA_PIXELSIZE
  #define MA_PIXELSIZE 16
#endif
#if MA_PIXELSIZE==8
  typedef uint8_t ma_pixel_t;
#elif MA_PIXELSIZE==16
  typedef uint16_t ma_pixel_t;
#elif MA_PIXELSIZE==32
  typedef uint32_t ma_pixel_t;
#else
  #error "Illegal value for MA_PIXELSIZE"
#endif

/* You must implement these.
 * Declared here as commentary.
 ***************************************************************/
 
void setup();
void loop();
int16_t audio_next();

/* Call from setup() to bring hardware online.
 ***************************************************************/
 
struct ma_init_params {

  int16_t videow,videoh; // pixels; 0=>96,64
  uint8_t rate; // Hz, 0=>max, unregulated

  uint32_t audio_rate; // Hz, 0=>disable
};

/* Bring everything online.
 * Null or zeroed (params) uses sensible defaults.
 * If present, we fill (params) with the actual config.
 * Nonzero on success.
 */
uint8_t ma_init(struct ma_init_params *params);

/* Routine maintenance, things you should do during loop().
 **************************************************************/
 
#define MA_BUTTON_LEFT       0x0001 /* dpad... */
#define MA_BUTTON_RIGHT      0x0002
#define MA_BUTTON_UP         0x0004
#define MA_BUTTON_DOWN       0x0008
#define MA_BUTTON_A          0x0010 /* Thumb buttons in reachability order (S,W,E,N)... */
#define MA_BUTTON_B          0x0020
// Buttons defined but not present in TinyArcade: XXX why am i defining them then?
#define MA_BUTTON_C          0x0040
#define MA_BUTTON_D          0x0080
#define MA_BUTTON_L1         0x0100 /* Left trigger, frontmost */
#define MA_BUTTON_L2         0x0200 /* Left back */
#define MA_BUTTON_R1         0x0400 /* Right front */
#define MA_BUTTON_R2         0x0800 /* Right back */
#define MA_BUTTON_AUX1       0x1000 /* eg "start" */
#define MA_BUTTON_AUX2       0x2000 /* eg "select" */
#define MA_BUTTON_RSV1       0x4000
#define MA_BUTTON_RSV2       0x8000

/* Gather input, poll systems that need it, sleep if we're configured to regulate timing.
 * Returns input state (bitfields MA_BUTTON_*).
 */
uint16_t ma_update();

/* Dimensions of (fb) must match init params with no padding.
 */
void ma_send_framebuffer(const void *fb);

/* Incidental ops.
 *************************************************************/

/* Read or write a regular file in one shot.
 * When reading, you provide a preallocated buffer.
 * We return the length filled. If the file is longer, you won't know.
 * (path) is from the root of TinyArcade's SD card, or 
 * from some externally-configured sandbox for native builds.
 */
int32_t ma_file_read(void *dst,int32_t dsta,const char *path,int32_t seek);
int32_t ma_file_write(const char *path,const void *src,int32_t srcc);

/* Arduino functions that we imitate if needed.
 **************************************************************/
 
uint32_t millis();
uint32_t micros();
void delay(uint32_t ms);

/* Software rendering.
 * Shared implementation; not platform-specific.
 ************************************************************/
 
struct ma_image {
  int16_t w,h;
  int16_t stride; // pixels (not bytes)
  ma_pixel_t colorkey; // if nonzero, this pixel is transparent
  ma_pixel_t *v;
};

#define MA_IMAGE_DECLARE(name,_w,_h,_colorkey,...) \
  static ma_pixel_t image_##name##_storage[_w*_h]={__VA_ARGS__}; \
  struct ma_image image_##name={ \
    .w=_w, \
    .h=_h, \
    .stride=_w, \
    .colorkey=_colorkey, \
    .v=image_##name##_storage, \
  };

void ma_image_blit_unchecked(
  struct ma_image *dst,int16_t dstx,int16_t dsty,
  const struct ma_image *src
);

void ma_image_blit(
  struct ma_image *dst,int16_t dstx,int16_t dsty,
  const struct ma_image *src
);

// Replace opaque pixels in (src) with (color), typically for text.
// If (src) has no colorkey, equivalent to ma_image_fill_rect().
void ma_image_blit_replace(
  struct ma_image *dst,int16_t dstx,int16_t dsty,
  const struct ma_image *src,
  ma_pixel_t color
);

void ma_image_crop(
  struct ma_image *dst,
  const struct ma_image *src,
  int16_t x,int16_t y,int16_t w,int16_t h
);

void ma_image_clear(
  struct ma_image *image,
  ma_pixel_t pixel
);

void ma_image_fill_rect(
  struct ma_image *image,
  int16_t x,int16_t y,int16_t w,int16_t h,
  ma_pixel_t pixel
);

static inline ma_pixel_t ma_pixel_from_rgb(const uint8_t *rgb) {
  #if MA_PIXELSIZE==8
    return (rgb[0]>>6)|((rgb[1]&0xe0)>>3)|(rgb[2]&0xe0);
  #elif MA_PIXELSIZE==16
    // 16-bit BGR565 but always big-endian: It's backward on little-endian machines (like the Tiny).
    #if 0 //TODO big-endian host
      return (rgb[0]>>3)|((rgb[1]&0xfc)>>1)|((rgb[2]&0xf8)<<8);
    #else
      return ((rgb[0]&0xf8)<<5)|((rgb[1]&0x1c)<<7)|(rgb[1]>>5)|(rgb[2]&0xf8);
    #endif
  #endif
}

static inline void ma_rgb_from_pixel(uint8_t *rgb,ma_pixel_t pixel) {
  #if MA_PIXELSIZE==8
    rgb[0]=pixel<<6; if (rgb[0]&0x40) rgb[0]|=0x20; rgb[0]|=rgb[0]>>3; rgb[0]|=rgb[0]>>6;
    rgb[1]=pixel&0x1c; rgb[1]|=rgb[1]<<3; rgb[1]|=rgb[1]>>6;
    rgb[2]=pixel&0xe0; rgb[2]|=rgb[2]>>3; rgb[2]|=rgb[2]>>6;
  #elif MA_PIXELSIZE==16
    rgb[0]=(pixel&0x1f00)>>5; rgb[0]|=rgb[0]>>5;
    rgb[1]=((pixel&0xe000)>>11)|((pixel&0x0007)<<5); rgb[1]|=rgb[1]>>6;
    rgb[2]=pixel&0x00f8; rgb[2]|=rgb[2]>>5;
  #endif
}

/* Font.
 * We only support ASCII G0 (0x20..0x7f), glyphs up to 7x7 pixels.
 * (actually, the total pixel count in a glyph must be <=32, so 5x6 is about it).
 **********************************************************/
 
struct ma_font {
  uint8_t metrics[0x60]; // (w<<5)|(h<<2)|y
  uint32_t bits[0x60]; // LRTB big-endianly
};

/* Render text in one color.
 * No line breaking; do that yourself before calling.
 * (dstx,dsty) is the top-left corner of the first glyph.
 * Returns the total advancement, including 1 blank column after the last glyph.
 */
int16_t ma_font_render(
  struct ma_image *dst,
  int16_t dstx,int16_t dsty,
  const struct ma_font *font,
  const char *src,int16_t srcc,
  ma_pixel_t pixel
);

int16_t ma_font_measure(
  const struct ma_font *font,
  const char *src,int16_t srcc
);

#ifdef __cplusplus
  }
#endif
#endif
