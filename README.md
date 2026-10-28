# CrossCrawler / CrossBlood

CrossCrawler is a personal fork of CrossInk, built for ESP32-powered e-readers and optimized around a reading-first experience. It keeps the core CrossInk reader firmware, adds reading statistics and customization, and layers in a lightweight Dungeon Crawler Carl-inspired gameplay loop where reading powers dungeon progression.

This firmware is designed for e-ink readers and reads books from SD cards, file browser/library flows, and wireless transfer paths while keeping the interface simple, low-power, and button-friendly.

## Why this project exists

This fork aims to preserve the best parts of the CrossInk firmware while extending it in a very specific direction:

- keep the reading experience as the core loop
- maintain good e-reader performance on ESP32 hardware
- add dungeon progression powered by reading stats
- make the app feel like a reading-powered dungeon crawler rather than a generic game shell
- remain compatible with the device profiles and firmware build system used by the upstream project

## Core features

### Reading-focused firmware

- EPUB, TXT, and XTC reading support
- SD-card based book library
- file browser and library navigation
- bookmarks, clippings, and reading progress
- reading stats dashboards and session tracking
- configurable front buttons and shortcut actions
- sleep screen customization and reading-focused themes

### Dungeon mode

The project includes a lightweight dungeon activity integrated into the app flow.

- home screen includes a Dungeon entry
- dungeon runs are persisted across app sessions
- player stats include HP, attack, defense, energy, gold, and relics
- dungeon enemies scale with floor progression
- reading directly rewards the dungeon loop with gold, healing, and progression boosts
- the game is intentionally lightweight and fits the constraints of an ESP32-based e-reader

This is a Carl-inspired reading-game layer, not a full standalone 3D roguelike engine, and it is meant to fit the device architecture instead of replacing the reading experience.

## Supported devices

This firmware targets the same reader hardware families as the upstream CrossInk project:

- Xteink X3
- Xteink X4
- Xteink X4 Pro
- Xteink X4 Classic
- Seeed Studio Sticky

## Repository layout

```text
.
├── .claude/                  # local repo guidance and task-specific notes
├── .github/                  # GitHub automation and workflow files
├── assets/                  # static assets and app resources
├── bin/                     # helper scripts or build artifacts
├── docs/                    # project docs and development guides
├── freeink-sdk/             # pinned FreeInk SDK submodule / dependency layer
├── include/                 # project headers
├── lib/                     # libraries used by the firmware
├── scripts/                 # build, patch, and generation scripts
├── site/                    # website content (not the reader UI)
├── src/                     # application source code
│   ├── activities/          # app screens and navigation activities
│   ├── components/          # reusable UI helpers and widgets
│   ├── util/                # utility code
│   ├── main.cpp             # app entry point
│   ├── CrossPointState.h    # persisted global app state
│   ├── CrossPointState.cpp
│   ├── DungeonGame.h        # dungeon logic and combat model
│   ├── DungeonGame.cpp
│   └── ...
├── test/                    # tests and fixtures
├── web/                     # web app / server-related sources
├── .clang-format            # formatting rules
├── .clangd                  # Editor config for clangd
├── AGENTS.md                # repository operational guidance
├── CLAUDE.md                # AI coding instructions
├── LICENSE                  # project license
├── README.md                # project overview
├── platformio.ini           # PlatformIO build matrix and device targets
├── partitions.csv           # flash partition map
├── requirements.txt         # Python tooling requirements
├── SCOPE.md                 # project scope / goals
└── CHANGELOG.md             # release notes and update history
```

## Hardware and software requirements

### Required tooling

- Git
- Python 3
- PlatformIO
- USB serial driver for the connected ESP32 target

### Recommended development setup

- Linux/macOS or WSL is recommended for firmware development
- use the project’s PlatformIO configuration
- make sure submodules are initialized before building

## Quick start

Clone the repo and initialize submodules:

```bash
git clone https://github.com/ariintheam-coder/CrossBlood.git
cd CrossBlood
git submodule update --init --recursive
```

Then build the firmware for your device target.

## Build targets

The project uses PlatformIO with multiple targets defined in `platformio.ini`.

### Default X3/X4 build

```bash
pio run -e default
```

### Sticky build

```bash
pio run -e sticky
```

### X4 Pro build

```bash
pio run -e x4-pro
```

### X4 Classic build

```bash
pio run -e x4-classic
```

### Simulator build

```bash
pio run -e simulator
```

The build script automatically renames the output firmware to a friendlier `.bin` name in the build directory.

## Output firmware artifacts

The project’s `scripts/rename_firmware.py` post-build hook creates files such as:

```text
.pio/build/default/firmware-x3-x4.bin
.pio/build/sticky/firmware-sticky.bin
.pio/build/x4-pro/firmware-x4-pro.bin
.pio/build/x4-classic/firmware-x4-classic.bin
```

These are the generated files you flash onto the device.

## Flashing firmware

The project documentation describes both web-based and command-line installation flows.

### Web installation

Use the project’s web installer or CrossInk’s companion web tool:

- https://inky.crossink.dev/#flash-tools

### Command line flash example

```bash
esptool.py --chip esp32c3 --port /dev/ttyACM0 --baud 921600 write_flash 0x10000 /path/to/firmware-x3-x4.bin
```

For ESP32-S3 boards, use the correct chip type and serial port for that device.

## Dungeon gameplay overview

The dungeon mode is designed to fit the CrossInk firmware architecture rather than replacing it.

### Reading as the power source

The game loop links real reading activity to progression:

- pages read give reading-based rewards
- reading time improves dungeon-state resources
- book progress and session stats feed the dungeon economy
- dungeon combat rewards the player with gold, relics, and healing

### Player loop

The intended progression loop is:

1. enter the home screen
2. open the Dungeon activity
3. fight enemies on each floor
4. use attacks, defense, rest, and read-powered boosts
5. gain gold and relics
6. continue deeper into the dungeon
7. use reading activity as the main engine for progression

## Important project notes

### This is an embedded e-reader firmware

This is not a standard desktop game project. It targets e-ink reader hardware and is built for constrained ESP32 environments.

Expect:

- flash-based build constraints
- limited RAM on ESP32-C3 devices
- optimization requirements for UI and memory usage
- hardware-specific board profiles and build flags

### This project keeps the reader first

The app remains primarily a reader firmware. The dungeon system is layered onto that experience rather than replacing it.

This is intentional:

- the firmware still feels like a reading device
- the dungeon is powered by actual reading activity
- the reader remains the foundation of the experience

## Development workflow

For normal development:

```bash
git status --short
pio run -e default
pio run -e simulator
```

The project uses PlatformIO as the canonical build path. Local overrides can be placed in a `platformio.local.ini` file if needed.

## Testing and validation

The repo includes simulator and native test paths, and the docs describe validation and debugging flow.

Examples:

```bash
pio run -e simulator
pio check -e default --fail-on-defect low --fail-on-defect medium --fail-on-defect high
```

See the docs folder for detailed guidance on development, debugging, and architecture.

## Troubleshooting

### Build fails because submodules are missing

```bash
git submodule update --init --recursive
```

### Firmware output not found

Check the build directory under `.pio/build/<env>/` and ensure the correct environment was built.

### Wrong board target

Use the correct environment for the device you actually own:

- default = X3/X4 builds
- sticky = Sticky builds
- x4-pro = X4 Pro build
- x4-classic = X4 Classic build

### Flashing fails

- check the serial port
- ensure the device is in the proper programming state
- use the correct chip target for ESP32-C3 vs ESP32-S3

## Documentation and design notes

The docs folder contains the architecture and development guidance for this project, including:

- installation notes
- architecture overview
- testing and debugging guidance
- device-specific and platform-specific development info

## License

This project is licensed under the MIT License.

See the `LICENSE` file for the full text.

## Contribution status

This repository is primarily maintained as a personal firmware fork and may not follow the standard pull-request flow used by larger open-source projects. Always check the repository guidance files before contributing.

## Summary

CrossCrawler / CrossBlood is a reading-first e-reader firmware for ESP32 devices with a dungeon-mode layer inspired by Dungeon Crawler Carl. It keeps the reading experience central while turning reading activity into the power source for progression, progression, loot, and dungeon runs.

It is a practical embedded firmware project built for real hardware and designed to feel like a reading-powered dungeon crawler on an e-ink device.

---

If you want to build and flash a custom firmware image from this repo, run the target-specific PlatformIO command and flash the generated `.bin` file for your exact hardware.

For example:

```bash
pio run -e default
# then flash the generated firmware-x3-x4.bin onto the device
```



