#include "multiarcade.h"
#include <string.h>

/* Blit, valid bounds.
 */
 
void ma_image_blit_unchecked(
  struct ma_image *dst,int16_t dstx,int16_t dsty,
  const struct ma_image *src
) {
  int16_t yi=src->h;
  ma_pixel_t *dstrow=dst->v+dsty*dst->stride+dstx;
  const ma_pixel_t *srcrow=src->v;
  if (src->colorkey) {
    for (;yi-->0;dstrow+=dst->stride,srcrow+=src->stride) {
      int16_t xi=src->w;
      ma_pixel_t *dstp=dstrow;
      const ma_pixel_t *srcp=srcrow;
      for (;xi-->0;dstp++,srcp++) {
        if (*srcp==src->colorkey) continue;
        *dstp=*srcp;
      }
    }
  } else {
    for (;yi-->0;dstrow+=dst->stride,srcrow+=src->stride) {
      int16_t xi=src->w;
      ma_pixel_t *dstp=dstrow;
      const ma_pixel_t *srcp=srcrow;
      for (;xi-->0;dstp++,srcp++) {
        *dstp=*srcp;
      }
    }
  }
}

/* Blit, check bounds.
 */

void ma_image_blit(
  struct ma_image *dst,int16_t dstx,int16_t dsty,
  const struct ma_image *src
) {
  int16_t w=src->w,h=src->h,srcx=0,srcy=0;
  if (dstx<0) { srcy+=dsty; if ((w+=dstx)<1) return; }
  if (dsty<0) { srcy+=dsty; if ((h+=dsty)<1) return; }
  if (dstx>dst->w-w) { if ((w=dst->w-dstx)<1) return; }
  if (dsty>dst->h-h) { if ((h=dst->h-dsty)<1) return; }
  ma_pixel_t *dstrow=dst->v+dsty*dst->stride+dstx;
  const ma_pixel_t *srcrow=src->v+srcy*src->stride+srcx;
  if (src->colorkey) {
    for (;h-->0;dstrow+=dst->stride,srcrow+=src->stride) {
      int16_t xi=w;
      ma_pixel_t *dstp=dstrow;
      const ma_pixel_t *srcp=srcrow;
      for (;xi-->0;dstp++,srcp++) {
        if (*srcp==src->colorkey) continue;
        *dstp=*srcp;
      }
    }
  } else {
    for (;h-->0;dstrow+=dst->stride,srcrow+=src->stride) {
      int16_t xi=w;
      ma_pixel_t *dstp=dstrow;
      const ma_pixel_t *srcp=srcrow;
      for (;xi-->0;dstp++,srcp++) {
        *dstp=*srcp;
      }
    }
  }
}

/* Blit with color replacement.
 */
 
void ma_image_blit_replace(
  struct ma_image *dst,int16_t dstx,int16_t dsty,
  const struct ma_image *src,
  ma_pixel_t color
) {
  if (!src->colorkey) {
    ma_image_fill_rect(dst,dstx,dsty,src->w,src->h,color);
    return;
  }
  int16_t w=src->w,h=src->h,srcx=0,srcy=0;
  if (dstx<0) { srcy+=dsty; if ((w+=dstx)<1) return; }
  if (dsty<0) { srcy+=dsty; if ((h+=dsty)<1) return; }
  if (dstx>dst->w-w) { if ((w=dst->w-dstx)<1) return; }
  if (dsty>dst->h-h) { if ((h=dst->h-dsty)<1) return; }
  ma_pixel_t *dstrow=dst->v+dsty*dst->stride+dstx;
  const ma_pixel_t *srcrow=src->v+srcy*src->stride+srcx;
  for (;h-->0;dstrow+=dst->stride,srcrow+=src->stride) {
    int16_t xi=w;
    ma_pixel_t *dstp=dstrow;
    const ma_pixel_t *srcp=srcrow;
    for (;xi-->0;dstp++,srcp++) {
      if (*srcp==src->colorkey) continue;
      *dstp=color;
    }
  }
}

/* Crop.
 */

void ma_image_crop(
  struct ma_image *dst,
  const struct ma_image *src,
  int16_t x,int16_t y,int16_t w,int16_t h
) {
  if (x<0) { w+=x; x=0; }
  if (y<0) { h+=y; y=0; }
  if (x>src->w-w) w=src->w-x;
  if (y>src->h-h) h=src->h-y;
  if ((w<1)||(h<1)) {
    memset(dst,0,sizeof(struct ma_image));
  } else {
    dst->v=src->v+y*src->stride+x;
    dst->w=w;
    dst->h=h;
    dst->stride=src->stride;
    dst->colorkey=src->colorkey;
  }
}

/* Clear image.
 */

void ma_image_clear(
  struct ma_image *image,
  ma_pixel_t pixel
) {
  ma_pixel_t *dst=image->v;
  int16_t i=image->h;
  if ((MA_PIXELSIZE==8)||((pixel>>8)==(pixel&0xff))) {
    if (image->stride==image->w) {
      memset(dst,pixel,image->w*image->h);
    } else {
      for (;i-->0;dst+=image->stride) {
        memset(dst,pixel,image->w);
      }
    }
  } else {
    for (;i-->0;dst+=image->stride) {
      int16_t xi=image->w;
      ma_pixel_t *dstp=dst;
      for (;xi-->0;dstp++) *dstp=pixel;
    }
  }
}

/* Fill rect.
 */

void ma_image_fill_rect(
  struct ma_image *image,
  int16_t x,int16_t y,int16_t w,int16_t h,
  ma_pixel_t pixel
) {
  if (x<0) { w+=x; x=0; }
  if (x>image->w-w) w=image->w-x;
  if (w<1) return;
  if (y<0) { h+=y; y=0; }
  if (y>image->h-h) h=image->h-y;
  if (h<1) return;
  ma_pixel_t *dst=image->v+y*image->stride+x;
  if ((MA_PIXELSIZE==8)||((pixel>>8)==(pixel&0xff))) {
    for (;h-->0;dst+=image->stride) {
      memset(dst,pixel,w);
    }
  } else {
    for (;h-->0;dst+=image->stride) {
      int16_t xi=w;
      ma_pixel_t *dstp=dst;
      for (;xi-->0;dstp++) *dstp=pixel;
    }
  }
}
