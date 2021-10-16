#!/bin/sh

if [ "$#" -gt 2 ] ; then
  echo "Usage: $0 [INPUT] [OUTPUT]"
  echo "  INPUT is an existing directory."
  echo "  OUTPUT is a new regular file."
  exit 1
fi

INPATH="$1"
if [ -z "$INPATH" ] ; then
  INPATH=games
fi

OUTPATH="$2"
if [ -z "$OUTPATH" ] ; then
  OUTPATH="img/$(date +%Y%m%d%H%M%S).fat"
fi

if [ ! -d "$INPATH" ] ; then
  echo "$INPATH: Not a directory"
  exit 1
fi

IMAGESIZE="$(du -s -BM "$INPATH" | sed -E 's/^([0-9]+)M.*$/\1/')"
IMAGESIZE="$(((IMAGESIZE+1)<<20))"
if [ "$IMAGESIZE" -lt 1000 ] || [ "$IMAGESIZE" -gt 1000000000 ] ; then
  echo "$INPATH: Improbable image size $IMAGESIZE"
  exit 1
fi

dd if=/dev/zero of="$OUTPATH" bs=1M count="$((IMAGESIZE>>20))" || exit 1

sudo mkfs.fat "$OUTPATH" || exit 1

mkdir -p tmpmnt || exit 1
sudo mount -oloop -tvfat "$OUTPATH" tmpmnt || exit 1
sudo cp -r "$INPATH"/* tmpmnt || exit 1
sudo umount tmpmnt
rmdir tmpmnt

echo "$OUTPATH: Produced $((IMAGESIZE>>20)) MB FAT image."
