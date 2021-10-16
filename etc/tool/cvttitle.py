#!/usr/bin/env python
import os,sys
from PIL import Image # sudo apt install python-pil

if len(sys.argv)!=3:
  raise Exception("\nUsage: %s INPUT OUTPUT\n  INPUT is PNG\n  OUTPUT is TinyArcade '.tsv'"%(sys.argv[0],))
  
srcpath=sys.argv[1]
dstpath=sys.argv[2]
  
image=Image.open(srcpath)
srcdata=image.getdata()
w,h=image.size
if w!=96 or h!=64:
  raise Exception("%s: Size must be (96,64). Found (%d,%d)"%(srcpath,w,h))
  
dst="" # BGR565 big-endian
for y in xrange(64):
  for x in xrange(96):
    pixel=srcdata.getpixel((x,y))
    if len(pixel)==3: r,g,b=pixel
    elif len(pixel)==4: r,g,b,a=pixel
    else: raise Exception("%s: Must be RGB or RGBA (found pixel %r)"%(srcpath,pixel))
    bgr565=((b<<8)&0xf800)|((g<<3)&0x07e0)|(r>>3)
    dst+=chr(bgr565>>8)
    dst+=chr(bgr565&0xff)
    
open(dstpath,"wb").write(dst)
