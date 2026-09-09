#!/bin/zsh
# Verify a MacBook's hardware against what was purchased.
# Run:  zsh verify-mac.sh        (no sudo needed)

hr() { printf '\n\e[1m== %s ==\e[0m\n' "$1"; }

hr "MODEL / CHIP / MEMORY / SERIAL"
system_profiler SPHardwareDataType 2>/dev/null | grep -E "Model Name|Model Identifier|Model Number|Chip|Total Number of Cores|Memory|Serial Number|Provisioning UDID" | sed 's/^ *//'

hr "CPU DETAIL"
echo "Brand:              $(sysctl -n machdep.cpu.brand_string)"
echo "Performance cores:  $(sysctl -n hw.perflevel0.physicalcpu 2>/dev/null)"
echo "Efficiency cores:   $(sysctl -n hw.perflevel1.physicalcpu 2>/dev/null)"
echo "RAM (bytes -> GB):  $(( $(sysctl -n hw.memsize) / 1073741824 )) GB"

hr "MEMORY MODULES"
system_profiler SPMemoryDataType | grep -E "Memory:|Type:|Manufacturer:" | sed 's/^ *//'

hr "GPU"
system_profiler SPDisplaysDataType | grep -E "Chipset Model|Total Number of Cores|Metal|Vendor" | sed 's/^ *//'

hr "DISPLAY(S)"
system_profiler SPDisplaysDataType | sed -n '/Displays:/,$p' | grep -E "^\s{8}[A-Za-z].*:$|Resolution|Display Type|Main Display" | sed 's/^ *//'

hr "SSD"
system_profiler SPNVMeDataType | grep -E "Model:|Capacity:|Serial Number:|Link Width|Link Speed|S.M.A.R.T" | sed 's/^ *//'
echo "--- disk0 ---"
diskutil info disk0 | grep -E "Device / Media Name|Disk Size|Solid State|SMART Status" | sed 's/^ *//'
echo "--- APFS container usage ---"
df -h / | tail -1 | awk '{print "Boot volume: size " $2 ", used " $3 ", free " $4}'

hr "BATTERY (important on a used machine)"
system_profiler SPPowerDataType | grep -E "Cycle Count|Condition|Maximum Capacity|Full Charge Capacity|Manufacture Date|Serial Number" | sed 's/^ *//'

hr "OS / FIRMWARE"
sw_vers
system_profiler SPHardwareDataType 2>/dev/null | grep -E "System Firmware|OS Loader" | sed 's/^ *//'

hr "OWNERSHIP / LOCKS (should all be clean for a machine you own)"
echo "MDM / DEP enrollment:"
profiles status -type enrollment 2>&1 | sed 's/^/  /'
echo "Find My token in NVRAM (should print nothing):"
nvram -p 2>/dev/null | grep -i fmm | sed 's/^/  /'
echo "Local admin users:"
dscl . -list /Users UniqueID | awk '$2 >= 500 {print "  " $1}'
echo "FileVault:"
fdesetup status | sed 's/^/  /'

hr "WIRELESS / PERIPHERALS PRESENT"
system_profiler SPBluetoothDataType 2>/dev/null | grep -E "Chipset|Firmware|State" | head -3 | sed 's/^ *//'
system_profiler SPAirPortDataType 2>/dev/null | grep -E "Card Type|Supported PHY Modes|Firmware" | head -3 | sed 's/^ *//'
system_profiler SPCameraDataType 2>/dev/null | grep -E "Model ID|Unique ID" | head -2 | sed 's/^ *//'
echo "Touch ID: $(bioutil -r 2>/dev/null | grep -c 'Fingerprint' ) fingerprint(s) enrolled (0 expected on a wiped machine)"

hr "NEXT: check the serial above at https://checkcoverage.apple.com"
echo "That page confirms the official model, purchase date validity, and warranty/AppleCare status."
