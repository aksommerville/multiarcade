all:
.SILENT:
PRECMD=echo "  $(@F)" ; mkdir -p $(@D) ;
PROJECT_NAME:=$(notdir $(PWD))

include etc/make/configure.mk

include src/tool/Makefile
all:tools

include src/data/Makefile
data:$(DATA_DSTFILES) $(DATA_TSV)
all:data

#include src/test/Makefile TODO
#test:...
#test-%:...

ifneq (,$(MA_BUILD_TINY))
  include src/platform/tinyarcade/Makefile
  tiny:$(TINY_BIN_SOLO) $(TINY_BIN_HOSTED)
  all:tiny
endif

ifneq (,$(MA_BUILD_NATIVE))
  include src/platform/$(MA_HOST)/Makefile
  native:$($(MA_HOST)_EXE)
  all:native
  run:$($(MA_HOST)_EXE);$(RUNCMD)
endif

clean:;rm -rf mid out

#----------------XXX-----------------------------------------------------------
ifeq (,XXX)
#---------------------------------------------------------
# Establish default platform and allow re-entry for cross-compiling.

ifndef MA_PLATFORM
  UNAMES:=$(shell uname -s)
  ifeq ($(UNAMES),Linux)
    UNAMEN:=$(shell uname -n)
    ifeq ($(UNAMEN),raspberrypi)
      MA_PLATFORM:=raspi
    else
      MA_PLATFORM:=linuxdefault
    endif
  else ifeq ($(UNAMES),Darwin)
    MA_PLATFORM:=macos
  else ifneq (,$(strip $(filter MINGW%,$(UNAMES))))
    MA_PLATFORM:=mswin
  else
    MA_PLATFORM:=tty
  endif
endif

PLATFORMS:=$(notdir $(wildcard src/platform/*))
ifeq (,$(strip $(filter $(MA_PLATFORM),$(PLATFORMS))))
  $(error Requested platform '$(MA_PLATFORM)' not found in src/platform/)
endif

$(PLATFORMS):;MA_PLATFORM=$@ make

MIDDIR:=mid/$(MA_PLATFORM)
OUTDIR:=out/$(MA_PLATFORM)

#------------------------------------------------------------

# Discover source files, remove those for other platforms.
SRCFILES:=$(shell find src -type f)
SRCFILES_PLATFORM:=$(filter src/platform/$(MA_PLATFORM)/%,$(SRCFILES))
SRCFILES:=$(filter-out src/platform/%,$(SRCFILES)) $(SRCFILES_PLATFORM)


$(info make $(MAKECMDGOALS) ; MA_PLATFORM=$(MA_PLATFORM))

#----------------XXX-----------------------------------------------------------
ifeq (,XXX)
PORT:=ttyACM0
IDEROOT:=/opt/arduino-1.8.16

# Empty, or a directory immediately under src, to build also for a non-TinyArcade platform.
ifeq ($(shell uname -n),raspberrypi)
  NATIVE_PLATFORM:=raspi
else
  NATIVE_PLATFORM:=linux-glx
endif

# Likely constant, for Linux users.
BUILDER:=$(IDEROOT)/arduino-builder
PKGROOT:=$(wildcard ~/.arduino15/packages)
LIBSROOT:=$(wildcard ~/Arduino/libraries)
SRCFILES:=$(shell find src -type f)
TMPDIR:=mid/build
CACHEDIR:=mid/cache
TMPDIRNORMAL:=mid/buildn
CACHEDIRNORMAL:=mid/cachen
EXECARD:=out/$(PROJECT_NAME).bin
EXENORM:=out/$(PROJECT_NAME)-normal.bin

SRCFILES:=$(filter-out src/dummy.cpp %/config.mk,$(SRCFILES))
SRCFILES_TINY:=$(foreach F,$(SRCFILES),$(if $(filter 2,$(words $(subst /, ,$F))),$F))
ifneq (,$(NATIVE_PLATFORM))
  SRCFILES_NATIVE:=$(filter src/$(NATIVE_PLATFORM)/%,$(SRCFILES)) src/softarcade.c src/main.c
endif

clean:;rm -rf mid out

#---------------------------------------------------------------
# Rules for TinyArcade.

$(TMPDIR):;mkdir -p $@
$(CACHEDIR):;mkdir -p $@
$(TMPDIRNORMAL):;mkdir -p $@
$(CACHEDIRNORMAL):;mkdir -p $@

define BUILD # 1=goal, 2=tmpdir, 3=cachedir, 4=BuildOption
$1:$2 $3; \
  $(BUILDER) \
  -compile \
  -logger=machine \
  -hardware $(IDEROOT)/hardware \
  -hardware $(PKGROOT) \
  -tools $(IDEROOT)/tools-builder \
  -tools $(IDEROOT)/hardware/tools/avr \
  -tools $(PKGROOT) \
  -built-in-libraries $(IDEROOT)/libraries \
  -libraries $(LIBSROOT) \
  -fqbn=TinyCircuits:samd:tinyarcade:BuildOption=$4 \
  -ide-version=10816 \
  -build-path $2 \
  -warnings=none \
  -build-cache $3 \
  -prefs=build.warn_data_percentage=75 \
  -prefs=runtime.tools.openocd.path=$(PKGROOT)/arduino/tools/openocd/0.10.0-arduino7 \
  -prefs=runtime.tools.openocd-0.10.0-arduino7.path=$(PKGROOT)/arduino/tools/openocd/0.10.0-arduino7 \
  -prefs=runtime.tools.arm-none-eabi-gcc.path=$(PKGROOT)/arduino/tools/arm-none-eabi-gcc/7-2017q4 \
  -prefs=runtime.tools.arm-none-eabi-gcc-7-2017q4.path=$(PKGROOT)/arduino/tools/arm-none-eabi-gcc/7-2017q4 \
  -prefs=runtime.tools.bossac.path=$(PKGROOT)/arduino/tools/bossac/1.7.0-arduino3 \
  -prefs=runtime.tools.bossac-1.7.0-arduino3.path=$(PKGROOT)/arduino/tools/bossac/1.7.0-arduino3 \
  -prefs=runtime.tools.CMSIS-Atmel.path=$(PKGROOT)/arduino/tools/CMSIS-Atmel/1.2.0 \
  -prefs=runtime.tools.CMSIS-Atmel-1.2.0.path=$(PKGROOT)/arduino/tools/CMSIS-Atmel/1.2.0 \
  -prefs=runtime.tools.CMSIS.path=$(PKGROOT)/arduino/tools/CMSIS/4.5.0 \
  -prefs=runtime.tools.CMSIS-4.5.0.path=$(PKGROOT)/arduino/tools/CMSIS/4.5.0 \
  -prefs=runtime.tools.arduinoOTA.path=$(PKGROOT)/arduino/tools/arduinoOTA/1.2.1 \
  -prefs=runtime.tools.arduinoOTA-1.2.1.path=$(PKGROOT)/arduino/tools/arduinoOTA/1.2.1 \
  src/dummy.cpp $(SRCFILES_TINY) \
  2>&1 | etc/tool/reportstatus.py
  # Add -verbose to taste
endef

# For inclusion in a TinyArcade SD card.
BINFILE:=$(TMPDIR)/dummy.cpp.bin
all:$(EXECARD)
$(EXECARD):build-card;$(PRECMD) cp $(BINFILE) $@
$(eval $(call BUILD,build-card,$(TMPDIR),$(CACHEDIR),TAgame))

# For upload.
NBINFILE:=$(TMPDIRNORMAL)/dummy.cpp.bin
all:$(EXENORM)
$(EXENORM):build-normal;$(PRECMD) cp $(NBINFILE) $@
$(eval $(call BUILD,build-normal,$(TMPDIRNORMAL),$(CACHEDIRNORMAL),normal))
  
launch:$(EXENORM); \
  stty -F /dev/$(PORT) 1200 ; \
  sleep 2 ; \
  $(PKGROOT)/arduino/tools/bossac/1.7.0-arduino3/bossac -i -d --port=$(PORT) -U true -i -e -w $(EXENORM) -R
  
#----------------------------------------------------------
# Rules for native platform.

ifndef (,$(NATIVE_PLATFORM))

MIDDIR:=mid/$(NATIVE_PLATFORM)
OUTDIR:=out/$(NATIVE_PLATFORM)

EXE:=$(OUTDIR)/$(PROJECT_NAME)

CMD_PRERUN:=
CMD_POSTRUN:=

CC:=gcc -c -MMD -O2 -Isrc -Werror -Wimplicit -DTINYC_NON_TINY=1 -DAPPNAME=\"$(PROJECT_NAME)\"
LD:=gcc
LDPOST:=

include src/$(NATIVE_PLATFORM)/config.mk

CFILES_NATIVE:=$(filter %.c,$(SRCFILES_NATIVE))
OFILES_NATIVE:=$(patsubst src/%,$(MIDDIR)/%.o,$(basename $(CFILES_NATIVE)))
-include $(OFILES_NATIVE:.o=.d)

$(MIDDIR)/%.o:src/%.c;$(PRECMD) $(CC) -o $@ $<

all:$(EXE)
$(EXE):$(OFILES_NATIVE);$(PRECMD) $(LD) -o $@ $^ $(LDPOST)
native:$(EXE)

run:$(EXE);$(CMD_PRERUN) $(EXE) $(CMD_POSTRUN)

else
native:;echo "NATIVE_PLATFORM unset" ; exit 1
endif
endif
endif
