#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common/fs.h"
#include "common/png.h"

/* Convert image.
 */
 
static void tsv_from_rgb(uint8_t *dst,const uint8_t *src,int c) {
  for (;c-->0;dst+=2,src+=3) {
    uint16_t bgr565=
      ((src[2]<<8)&0xf800)|
      ((src[1]<<3)&0x07e0)|
      (src[0]>>3)
    ;
    dst[0]=bgr565>>8;
    dst[1]=bgr565;
  }
}
 
static int tsv_from_png_image(uint8_t *dst,const struct png_image *srcimage) {
  struct png_image image={0};
  if (png_image_convert(&image,8,PNG_COLORTYPE_RGB,srcimage)<0) {
    png_image_cleanup(&image);
    return -1;
  }
  tsv_from_rgb(dst,image.pixels,96*64);
  png_image_cleanup(&image);
  return 0;
}

/* Main.
 */

int main(int argc,char **argv) {

  const char *dstpath=0,*srcpath=0;
  int argp=1;
  for (;argp<argc;argp++) {
    if (!memcmp(argv[argp],"-o",2)) {
      if (dstpath) {
        fprintf(stderr,"%s: Multiple output paths\n",argv[0]);
        return 1;
      }
      dstpath=argv[argp]+2;
    } else if (!argv[argp][0]||(argv[argp][0]=='-')) {
      fprintf(stderr,"%s: Unexpected argument '%s'\n",argv[0],argv[argp]);
      return 1;
    } else if (srcpath) {
      fprintf(stderr,"%s: Multiple input paths\n",argv[0]);
      return 1;
    } else {
      srcpath=argv[argp];
    }
  }
  if (!dstpath||!srcpath) {
    fprintf(stderr,"Usage: %s -oOUTPUT INPUT\n",argv[0]);
    fprintf(stderr,"  OUTPUT is a TinyArcade 'tsv' file, INPUT is a PNG file.\n");
    return 1;
  }
  
  void *src=0;
  int srcc=file_read(&src,srcpath);
  if (srcc<0) {
    fprintf(stderr,"%s: Failed to read file\n",srcpath);
    return 1;
  }
  
  struct png_image *image=png_decode(src,srcc);
  free(src);
  if (!image) {
    fprintf(stderr,"%s: Failed to decode %d-byte file as PNG\n",srcpath,srcc);
    return 1;
  }
  
  if ((image->w!=96)||(image->h!=64)) {
    fprintf(stderr,"%s: Dimensions must be 96x64. Found %dx%d\n",srcpath,image->w,image->h);
    png_image_del(image);
    return 1;
  }
  
  uint8_t *dst=malloc(96*64*2);
  if (!dst) {
    png_image_del(image);
    return 1;
  }
  
  if (tsv_from_png_image(dst,image)<0) {
    fprintf(stderr,"%s: Failed to convert image\n",srcpath);
    png_image_del(image);
    free(dst);
    return 1;
  }
  png_image_del(image);
  
  int err=file_write(dstpath,dst,96*64*2);
  free(dst);
  if (err<0) {
    fprintf(stderr,"%s: Failed to write file\n",dstpath);
    return 1;
  }
  
  return 0;
}
