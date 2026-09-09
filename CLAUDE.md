# CLAUDE.md

Context for Claude Code when this repo is opened on a newly acquired Mac.

## Purpose

Shawn bought a used or refurbished MacBook Pro and wants to confirm the
hardware matches the listing before the return window closes. Your job is
to run the checks, compare against the expected specs below, and report
every mismatch plainly. Do not soften a discrepancy.

## Workflow

1. Run the hardware report and save it:
   `zsh verify-mac.sh | tee verify-$(date +%F).txt`
2. Build and run the bandwidth test: `make && ./membw | tee membw-$(date +%F).txt`
   If `make` fails because Xcode Command Line Tools are missing, say so and
   offer `xcode-select --install`. Do not skip the test silently.
3. Before trusting the bandwidth number, check the machine is idle:
   `memory_pressure | tail -1 && sysctl vm.swapusage`
   Free above 80% and swap near zero. If not, ask Shawn to close apps or
   restart, then rerun.
4. Fill in the "Found" column in the table below from the outputs and mark
   each row match or mismatch.
5. Look up the serial at https://checkcoverage.apple.com. Report model,
   warranty state, and whether the serial is recognized.
6. Walk through the manual checks in README.md and ask Shawn to confirm each.

## Expected vs found

Fill "Expected" from the purchase listing or receipt. Ask Shawn for it if
blank. "Found" comes from the script output.

| Item | Expected | Found | Match |
|---|---|---|---|
| Model name / year | | | |
| Model identifier | | | |
| Model number prefix (M/Z/F/5/N) | | | |
| Chip | | | |
| CPU cores (P + E) | | | |
| GPU cores | | | |
| RAM (GB, type) | | | |
| SSD (model, bytes) | | | |
| Display size / resolution | | | |
| Battery cycle count | | | |
| Battery max capacity | | | |
| Memory bandwidth (rated) | ~400 GB/s | | |
| membw peak (COPY) | 55-75% of rated | | |
| MDM / DEP enrollment | No / No | | |
| Find My NVRAM token | none | | |
| Warranty / AppleCare | | | |

## Interpreting membw

- The rated figure is fixed by the chip variant. See the table in README.md.
  If the chip and GPU core count identify a Max bin, bandwidth follows.
- A CPU-only test reaches 55-75% of rated. For a 400 GB/s chip expect
  roughly 220-300 GB/s on COPY. 100-150 suggests a Pro chip. Under 100
  means swapping or a fault.
- Baseline for comparison: Shawn's previous M1 Pro (MacBookPro18,3, 32 GB,
  rated 200 GB/s) measured 135 GB/s COPY while swapping heavily, 2026-09-09.

## Red flags to call out immediately

- Activation Lock prompt at first boot, or a Find My token in NVRAM.
- MDM or DEP enrollment showing Yes.
- Model number starting with F when the listing said new, or 5/N at all.
- Battery cycle count above 500 or max capacity under 80% on a machine
  sold as lightly used.
- Chip, core count, RAM, or SSD smaller than the listing.
- SMART status anything other than Verified.
- Parts and Service History showing unknown or non-genuine parts.

## Conventions

- Keep verify-mac.sh dependency-free: only tools shipped with macOS.
- membw.c is plain C with pthreads, no external libraries.
- Saved outputs (`*.txt`) are gitignored. Do not commit them.
- Do not commit or push unless Shawn asks.
