# Makefile for Multiarcade data files.

DATA_MIDDIR:=mid/data

# Find the input files and sort by type (must be in a typed directory).
DATA_SRCFILES:=$(shell find src/data -type f)
DATA_SRCFILES_IMAGE:=$(filter src/data/image/%,$(DATA_SRCFILES))
DATA_SRCFILES_FONT:=$(filter src/data/font/%,$(DATA_SRCFILES))
DATA_SRCFILES_SONG:=$(filter src/data/song/%,$(DATA_SRCFILES))
DATA_SRCFILES_SOUND:=$(filter src/data/sound/%,$(DATA_SRCFILES))
DATA_SRCFILES_RAW:=$(filter src/data/raw/%,$(DATA_SRCFILES))
DATA_SRCFILES:=$(DATA_SRCFILES_IMAGE) $(DATA_SRCFILES_FONT) $(DATA_SRCFILES_SONG) $(DATA_SRCFILES_SOUND) $(DATA_SRCFILES_RAW)

DATA_DSTFILES:=$(patsubst src/data/%,$(DATA_MIDDIR)/%.c,$(basename $(DATA_SRCFILES)))

$(DATA_MIDDIR)/image/%.c:src/data/image/%.png $(EXE_mkimage);$(PRECMD) $(EXE_mkimage) -o$@ $<
$(DATA_MIDDIR)/font/%.c:src/data/font/%.png $(EXE_mkfont);$(PRECMD) $(EXE_mkfont) -o$@ $<

DATA_TSV:=out/$(PROJECT_NAME).tsv
$(DATA_TSV):src/data/title.png $(EXE_mktsv);$(PRECMD) $(EXE_mktsv) -o$@ $<
