#!/bin/sh

if [ "$#" -ne 2 ] ; then
  echo "Usage: $0 IMAGE DEVICE"
  exit 1
fi

INPATH="$1"
OUTPATH="$2"

if [ ! -f "$INPATH" ] ; then
  echo "$INPATH: Not a regular file"
  exit 1
fi

if [ ! -b "$OUTPATH" ] ; then
  echo "$OUTPATH: Not a block device"
  exit 1
fi

sudo dd if="$INPATH" of="$OUTPATH" || exit 1
