# Makefile for "linuxdefault" platform.

# 8 and 16 both good for pixelsize.
_CCDEF:=-DMA_PIXELSIZE=16 -DMA_APP_NAME=\"$(PROJECT_NAME)\"
_CCINC:=-Isrc/platform/linuxdefault -Isrc/common
_CCERR:=-Werror -Wimplicit

linuxdefault_CC:=gcc -c -MMD -O2 $(_CCINC) $(_CCERR) $(_CCDEF)
linuxdefault_LD:=gcc
linuxdefault_LDPOST:=-lX11 -lpulse-simple -lpthread

linuxdefault_MIDDIR:=mid/platform/linuxdefault
linuxdefault_OUTDIR:=out/linuxdefault

linuxdefault_EXE:=$(linuxdefault_OUTDIR)/$(PROJECT_NAME)

linuxdefault_CFILES:=$(shell find src/platform/linuxdefault -name '*.c') $(CFILES_COMMON) $(CFILES_MAIN)
linuxdefault_OFILES:=$(patsubst src/%.c,$(linuxdefault_MIDDIR)/%.o,$(linuxdefault_CFILES))
-include $(linuxdefault_OFILES:.o=.d)

$(linuxdefault_MIDDIR)/%.o:src/%.c;$(PRECMD) $(linuxdefault_CC) -o $@ $<

$(linuxdefault_EXE):$(linuxdefault_OFILES);$(PRECMD) $(linuxdefault_LD) -o $@ $^ $(linuxdefault_LDPOST)

