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
