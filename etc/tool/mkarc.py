#!/usr/bin/env python
import sys,os
from PIL import Image

image=Image.open(sys.argv[1])
if image.width&7: raise Exception("%s: Width must be multiple of 8 (have %d)"%(sys.argv[1],image.width))
data=image.getdata()

frames=[] # each is an array of bytes, byte is one 8-pixel row

def read_row(x,y):
  """Read 8 pixels starting at (x,y) and proceeding rightward. Returns as one byte."""
  row=0
  mask=0x80
  for i in xrange(8):
    pixel=data.getpixel((x+i,y))
    if len(pixel)==4: a=pixel[3]
    elif len(pixel)==3: a=(pixel[0]+pixel[1]+pixel[2]>=384)
    elif len(pixel)==2: a=pixel[1]
    elif type(pixel)==int: a=pixel
    else: raise Exception("%s: Unexpected pixel format, eg %r"%(sys.argv[1],pixel))
    if a: row|=mask
    mask>>=1
  return row

for srcx in xrange(0,image.width,8):
  frame=[]
  for srcy in xrange(image.height):
    row=read_row(srcx,srcy)
    # skip leading empty rows -- there will be a lot of these
    if row or len(frame): frame.append(row)
  frames.append(frame)
  
# Result must have yielded exactly one frame of each height (1..framec) inclusive.
# Sort by length ascending (note this is reverse of the order I drew them).
def cmp_frames(a,b):
  if len(a)<len(b): return -1
  if len(a)>len(b): return 1
  return 0
frames.sort(cmp_frames)
for i in xrange(len(frames)):
  if len(frames[i])!=i+1: raise Exception("%s: Expected %d-pixel tall frame"%(sys.argv[1],i+1))
  
sys.stdout.write("#define ARC_HEIGHT_LIMIT %d\n"%(len(frames),))
  
sys.stdout.write("static uint8_t arc_raw_data[]={\n")
for frame in frames: sys.stdout.write("  %s,\n"%(','.join(map(str,frame))))
sys.stdout.write("};\n")

# The starting points could be calculated at runtime, but let's be clear.
# We'll add a dummy at the beginning, so it can be indexed by exact desired height instead of h-1.
sys.stdout.write("static uint8_t arc_starts[1+ARC_HEIGHT_LIMIT]={\n  0,")
p=0
for frame in frames:
  sys.stdout.write("%d,"%(p,))
  p+=len(frame)
sys.stdout.write("\n};\n")

