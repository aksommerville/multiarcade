# multiarcade

Interfaces for the TinyCircuits TinyArcade/PocketArcade,
and workalikes for more common platforms.
Tiny: https://tinycircuits.com/

## Make commands

Start a new project (basically just copies this one): `make project`

Build everything, for the default targets: `make`

Build for the default host and run it: `make run`

Build for alternate host: `MA_HOST=config-name make run`

config-name: `generic`, `linuxdefault`, `linuxdrm`, `raspi`

Build for Tiny and launch via USB: `make launch`

Build for Tiny and interactively prepare SD card: `make sdcard`

Run tests: `make test`

Run one test: `make test-NAME`

## TODO

- [x] tinyarcade
- [x] linuxdefault
- [x] raspi
- [x] linuxdrm
- [x] generic
- [x] Validate color conversions
- [x] Soft lib (image, font, etc)
- [x] Test framework
- [ ] Example game
- [x] Coordinate SD card image from Makefile
- [ ] Auto-re-deploy menu via Makefile
- [ ] raspi bcm: Need a blotter layer to cover console
- [ ] Synthesizer? Or some layer of audio support.
