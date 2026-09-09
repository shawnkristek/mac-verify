# mac-verify

Tools for confirming a MacBook matches what you paid for.

## verify-mac.sh

Prints everything macOS knows about the hardware: model and chip, CPU core
split, RAM size/type/manufacturer, GPU core count, displays, SSD model and
capacity, battery cycle count and health, firmware, MDM/DEP enrollment,
Find My tokens, local users, FileVault, and wireless/camera hardware.

```
zsh verify-mac.sh | tee verify-$(date +%F).txt
```

Take the serial number to <https://checkcoverage.apple.com> to confirm the
official model and warranty status.

Model number prefix: `M` retail new, `Z` configure-to-order, `F` Apple
refurbished, `5`/`N` service replacement.

## membw

STREAM-style memory bandwidth test. Runs read, write, and copy passes over
two 1 GB buffers using work-stealing chunks so efficiency cores don't gate
the result. Sweeps P-cores and all cores.

```
make
./membw                 # defaults
./membw 12 2048         # also try 12 threads, with 2 GB buffers
```

CPU-side tests typically reach 55-75% of Apple's rated bandwidth. For a
clean number, restart, open only Terminal, and confirm swap is near zero:

```
memory_pressure | tail -1 && sysctl vm.swapusage
```

| Chip | Rated |
|---|---|
| M1 Max, M2 Max | 400 GB/s |
| M3 Max 14c CPU / 30c GPU | 300 GB/s |
| M3 Max 16c CPU / 40c GPU | 400 GB/s |
| M4 Max 14c CPU / 32c GPU | 410 GB/s |
| M4 Max 16c CPU / 40c GPU | 546 GB/s |
| Pro variants | 150-273 GB/s |

## Manual checks

- Machine should boot to Setup Assistant, not an Activation Lock prompt.
- Serial on box, underside, and `verify-mac.sh` output should agree.
- System Settings > General > About > Parts and Service History.
- Apple Diagnostics: hold power at boot, then Command-D. `ADP000` = clean.
- Erase All Content and Settings before trusting a used machine.
- Test every port, speakers, keyboard, trackpad, Touch ID, camera, mic.
- Inspect the display at full brightness on white and black.
