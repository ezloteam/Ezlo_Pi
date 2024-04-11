
<picture>
  <img src="ezlopi-doc/ezlopi_logo.png" >
</picture>

# RELEASE NOTES

## New:
1. Implementation of 6-function-APIs (for less, for at-least, repeat, follow, pulse, latch): [EZPI-351](https://jira.mios.com/browse/EZPI-351)
2. Implementation of heap-memory guard when 'scene_create' triggered: [EZPI-358](https://jira.mios.com/browse/EZPI-358)
3. Implementation of 'toggle-value' method; [testing remained] [EZPI-349](https://jira.mios.com/browse/EZPI-349)
4. Implementation fake Token based security on local API [EZPI-341](https://jira.mios.com/browse/EZPI-341)
5. Implementation of WiFi Change API [EZPI-302](https://jira.mios.com/browse/EZPI-302)
6. Implementation of Battery status on Firmware Core [EZPI-332](https://jira.mios.com/browse/EZPI-332)
7. Implementation of Status indicator based on LED and BLE for EzloPi [EZPI-344](https://jira.mios.com/browse/EZPI-344)
8. Implementation of UART (serial) configuration from serial and BLE [EZPI-347](https://jira.mios.com/browse/EZPI-347)

## Fixes:
1. Fix device tree free-issue when initialization: [EZPI-333](https://jira.mios.com/browse/EZPI-333)
2. Fix 'NOT' scene method parsing failure: [EZPI-363](https://jira.mios.com/browse/EZPI-363)
3. Fix 'Scene_State' enable/disable not update in NVS: [EZPI-364](https://jira.mios.com/browse/EZPI-364)
4. mDNS Service type modification: [EZPI-356](https://jira.mios.com/browse/EZPI-356)
5. Replacenemt of dynamic buffer to static buffer for minimizing heap fragmentation [EZPI-352](https://jira.mios.com/browse/EZPI-352)
6. WiFi SSID and PSW to be matched len with factory info [EZPI-316](https://jira.mios.com/browse/EZPI-316)