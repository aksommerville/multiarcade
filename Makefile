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

include src/test/Makefile
test:$(TEST_EXES);etc/tool/testrunner.sh $(TEST_EXES)
test-%:$(TEST_EXES);MA_TEST_FILTER="$*" etc/tool/testrunner.sh $(TEST_EXES)

ifneq (,$(MA_BUILD_TINY))
  include src/platform/tinyarcade/Makefile
  tiny:$(TINY_BIN_SOLO) $(TINY_BIN_HOSTED) $(DATA_TSV) $(TINY_PACKAGE)
  all:tiny
endif

ifneq (,$(MA_BUILD_NATIVE))
  include src/platform/$(MA_HOST)/Makefile
  native:$($(MA_HOST)_EXE)
  all:native
  run:$($(MA_HOST)_EXE);$(RUNCMD)
endif

clean:;rm -rf mid out

project:;etc/tool/mkproject.sh

TA_MENU_BIN:=etc/ArcadeMenu.ino.bin
ifneq (,$(TA_MENU_BIN))
  deploy-menu:; \
    stty -F /dev/$(MA_PORT) 1200 ; \
    sleep 2 ; \
    $(MA_PKGROOT)/arduino/tools/bossac/1.7.0-arduino3/bossac -i -d --port=$(MA_PORT) -U true -i -e -w $(TA_MENU_BIN) -R
endif
