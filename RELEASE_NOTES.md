<picture>
  <img src="ezlopi-doc/ezlopi_logo.png" >
</picture>

# RELEASE NOTES

# Version 4.4.13 Build 203

## New

1. Implement : ADC De-Init function with memory cleanup [EZPI-574](https://jira.mios.com/browse/EZPI-574)
2. Implement : delete_service context functions [EZPI-575](https://jira.mios.com/browse/EZPI-575)
3. Implement : Error Returns on mDNS Module [EZPI-576](https://jira.mios.com/browse/EZPI-576)

## Fixes

1. Fix: Annotate file and function headers [EZPI-539](https://jira.mios.com/browse/EZPI-539)
2. Fix: Null pointer dereference : Scenes Populate [EZPI-595](https://jira.mios.com/browse/EZPI-595)
3. Fix : Possible null pointer dereference: house_mode_id_array [EZPI-596](https://jira.mios.com/browse/EZPI-596)
4. Fix : invalidFunctionArg : MQ Sensors [EZPI-597](https://jira.mios.com/browse/EZPI-597)
5. Fix : hub.nma.register.repeat implementation missing / not working [EZPI-455](https://jira.mios.com/browse/EZPI-455)
6. Fix : Change the author and details position from files [EZPI-483](https://jira.mios.com/browse/EZPI-483)
7. Fix : Object does not has a parent [EZPI-589](https://jira.mios.com/browse/EZPI-589)
8. Fix : Shifting 32-bit value by 56 bits is undefined behaviour [shiftTooManyBits] [EZPI-592](https://jira.mios.com/browse/EZPI-592)
9. Fix : Core Modes -> Null pointer dereference: curr_source [nullPointer] [EZPI-593](https://jira.mios.com/browse/EZPI-593)
10. Fix : arrayIndexOutOfBounds on ezlopi_core_processes [EZPI-594](https://jira.mios.com/browse/EZPI-594)
11. Fix : Null pointer dereference : Scenes Populate [EZPI-595](https://jira.mios.com/browse/EZPI-595)
12. Fix : Possible null pointer dereference: house_mode_id_array [EZPI-596](https://jira.mios.com/browse/EZPI-596)
13. Fix : Possible null pointer dereference: house_mode_id_array [EZPI-596](https://jira.mios.com/browse/EZPI-596)

## What's Changed

- Revert "Feature kconfig ot" by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/217
- Development by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/224
- Development by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/225
- EZPI-537 Apply templates to all source files by @ezlo-gary in https://github.com/ezloteam/Ezlo_Pi/pull/208
- Dev lastest merge code clean by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/226
- Ezpi for device test by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/227
- Ezpi combined fix 593 595 596 597 by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/228
- Espi 583 change author and details positions by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/232
- Fixed the bug of IndexOutOfBound in \_\_check_ezlopi_task function by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/231
- Ezpi 576 error return on functions by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/230
- Development by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/233

# Version 4.3.12 Build 202

## New

1. Implement : Header source with the updated templates on EzloPi Local Scenes [EZPI-557](https://jira.mios.com/browse/EZPI-557)
2. Implement : Header source with the updated templates on EzloPi Cores [EZPI-558](https://jira.mios.com/browse/EZPI-558)
3. Implement : Add Config Chipset check support for ESP32S2 [EZPI-580](https://jira.mios.com/browse/EZPI-580)
4. Implement : Add K-Config for Otel Enable Disable [EZPI-553](https://jira.mios.com/browse/EZPI-553)
5. Implement : header & source templates on Ezlopi_hal components [EZPI-567](https://jira.mios.com/browse/EZPI-567)

## Fixes

1. Fix: ESP32-C3 Device is not responding [EZPI-519](https://jira.mios.com/browse/EZPI-519)
2. Fix: EzloPi Core OTA Testing and fixing [EZPI-566](https://jira.mios.com/browse/EZPI-566)
3. Fix: Decrease delay while broadcasting WSS messages [EZPI-552](https://jira.mios.com/browse/EZPI-552)
4. Fix: Stack overflow on MQ Calibration Tasks [EZPI-568](https://jira.mios.com/browse/EZPI-568)
5. Fix: Remove global/static variables from sensor-devices [EZPI-572](https://jira.mios.com/browse/EZPI-572)
6. Fix: Potential array bounds issues [EZPI-554](https://jira.mios.com/browse/EZPI-554)
7. Fix: Potential memory leak issues [EZPI-556](https://jira.mios.com/browse/EZPI-556)
8. Fix: Moving from tasks to loop for memory optimization [EZPI-578](https://jira.mios.com/browse/EZPI-578)
9. Fix: Debugging, Fixing errors and memory leaks [EZPI-579](https://jira.mios.com/browse/EZPI-579)

## What's Changed

- Ezpi 568 remove sensor tasks by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/214
- Feature kconfig ot by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/215
- Removing global statics in sensor devices by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/219
- Current config freeing by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/218
- Ezpi 566 ota test and fix by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/220
- fix memoryleak tendencies https://jira.mios.com/browse/EZPI-556. by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/221
- fix array out of bounds issue : https://jira.mios.com/browse/EZPI-554 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/222

# Version 4.3.11 Build 201

## New

1. Implement Feature Open Telemetry [EZPI-477](https://jira.mios.com/browse/EZPI-477)

## What's Changed

- Feature otel trace by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/211
- Development by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/212

# Version 4.2.10 Build 200

## New

1. Implement API: `hub.mode.entry_delay.skip` - House Modes [EZPI-529](https://jira.mios.com/browse/EZPI-529)
2. Added house-mode alarm functionality in `house_modes_loop` [EZPI-548](https://jira.mios.com/browse/EZPI-548)
3. Implement API: `hub.modes.protect.buttons.set` [EZPI-549](https://jira.mios.com/browse/EZPI-549)
4. Implement API: `hub.modes.protect.devices.add` [EZPI-550](https://jira.mios.com/browse/EZPI-550)
5. Implement API: `hub.modes.protect.devices.remove` [EZPI-551](https://jira.mios.com/browse/EZPI-551)

## Fixes

1.  Digital LDR causing device to reboot [EZPI-546](https://jira.mios.com/browse/EZPI-546)
2.  Memory assessment on WSS and optimization [EZPI-543](https://jira.mios.com/browse/EZPI-543)
3.  Update source and header templates[EZPI-536](https://jira.mios.com/browse/EZPI-536)

## What's Changed

- Inspecting otel by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/205
- EZPI-536 Update template files by @ezlo-gary in https://github.com/ezloteam/Ezlo_Pi/pull/204
- Combined ezpi 529 548 549 550 551 by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/206
- Development by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/207

## New Contributors

- @ezlo-gary made their first contribution in https://github.com/ezloteam/Ezlo_Pi/pull/204

# Version 4.2.9 Build 199

## New

1. Implement API: `hub.modes.notifications.set` - House Modes [EZPI-525](https://jira.mios.com/browse/EZPI-525)
2. Implement API: `hub.modes.disarmed_default.set` [EZPI-526](https://jira.mios.com/browse/EZPI-526)
3. Implement API: `hub.modes.notifications.remove` [EZPI-528](https://jira.mios.com/browse/EZPI-528)
4. Implement API: `hub.modes.bypass_devices.add` - when-method [EZPI-530](https://jira.mios.com/browse/EZPI-530)
5. Implement API: `hub.modes.bypass_devices.remove` [EZPI-531](https://jira.mios.com/browse/EZPI-531)
6. Implement API: `hub.modes.disarmed_devices.add` [EZPI-527](https://jira.mios.com/browse/EZPI-527)
7. Local Auth Token Addition on factory / Provisioning data on BLE [EZPI-533](https://jira.mios.com/browse/EZPI-533)
8. Added USB CDC Support for S2 and S3 [EZPI-535](https://jira.mios.com/browse/EZPI-535)

## Fixes

1.  Avoid reading non-readable chars from the string stored in flash. [EZPI-534](https://jira.mios.com/browse/EZPI-534)
2.  EzloPi CMD 4 in different chunks [EZPI-532](https://jira.mios.com/browse/EZPI-532)
3.  Sunrise/Sunset time generator API (using longitude and latitude parameter) [EZPI-310](https://jira.mios.com/browse/EZPI-310)

# Version 4.2.8 Build 198

## New

1. Implement API: `isItemStateChanged` - when-method [EZPI-518](https://jira.mios.com/browse/EZPI-518)
2. Implement API: `hub.scenes.clone` [EZPI-515](https://jira.mios.com/browse/EZPI-515)
3. Implement API: `hub.scenes.trigger.devices.list` [EZPI-516](https://jira.mios.com/browse/EZPI-516)
4. Implement API: `isGroupState` - when-method [EZPI-517](https://jira.mios.com/browse/EZPI-517)
5. Implement API: `hub.scenes.time.list` [EZPI-509](https://jira.mios.com/browse/EZPI-509)
6. Token Addition on factory / Provisioning data [EZPI-523](https://jira.mios.com/browse/EZPI-523)
7. Validate tokens for incoming connections over local Web Socket [EZPI-503](https://jira.mios.com/browse/EZPI-503)

## Fixes

1. When creating or deleting scenes created using the switchHouseMode method side effects occur - the scene is launched, which is confirmed by the corresponding broadcast. [EZPI-510](https://jira.mios.com/browse/EZPI-510)
2. EzloPI mdns instance name must include serial number [EZPI-502](https://jira.mios.com/browse/EZPI-502)
3. DHT11 Reading value 0 [EZPI-511](https://jira.mios.com/browse/EZPI-511)
4. Multiple scale on broadcast json [EZPI-512](https://jira.mios.com/browse/EZPI-512)
5. Multiple I2C sensors [513](https://jira.mios.com/browse/EZPI-513)
6. Remove hard coding of I2C Device address [514](https://jira.mios.com/browse/EZPI-514)

## What's Changed

- Ezpi 509 510 issue fixed by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/190
- Ezlopi multiple i2c by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/191
- impementation of provisioning UART API completed by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/192
- Ezpi 515 516 517 combined by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/193
- fix cmd 3 : https://jira.mios.com/browse/EZPI-521 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/194
- minor fixes and updated the size. by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/195
- Ezpi 503 validating tikens by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/197
- Ezpi 518 is item state changed by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/198
- Preventing API calls from being processed if not logged in WSS by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/199
- Development by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/200

**Full Changelog**: https://github.com/ezloteam/Ezlo_Pi/compare/v4.2.7...v4.2.8

# Version 4.2.7 Build 197

## New

1. Implement API: `hub.scenes.meta.set` [EZPI-479](https://jira.mios.com/browse/EZPI-479)
2. Implement API : `hub.scenes.block.meta.set` [EZPI-501](https://jira.mios.com/browse/EZPI-501)
3. Implement API : `hub.scenes.expression.list` [EZPI-493](https://jira.mios.com/browse/EZPI-493)
4. Implement API: `isHouseModeAlarmPhaseRange` [EZPI-372](https://jira.mios.com/browse/EZPI-372)
5. Implement API: `isHouseModeSwitchToRange` [EZPI-373](https://jira.mios.com/browse/EZPI-373)
6. Implement API : `hub.scenes.room.set` [EZPI-505](https://jira.mios.com/browse/EZPI-505)
7. Implement API : `hub.data.list` [EZPI-487](https://jira.mios.com/browse/EZPI-487)
8. Implement API : `hub.settings` [EZPI-253](https://jira.mios.com/browse/EZPI-253)
9. Implement Error Codes [EZPI-414](https://jira.mios.com/browse/EZPI-414)
10. Implement Provisioning API on Serial interface [EZPI-478](https://jira.mios.com/browse/EZPI-478)
11. Implement driver for Internal temperature sensor [EZPI-500](https://jira.mios.com/browse/EZPI-500)

## Fixes

1. Missing "value" field in Expressions (variable: false). [EZPI-494](https://jira.mios.com/browse/EZPI-494)
2. One Wire not working on certain pins [EZPI-432](https://jira.mios.com/browse/EZPI-432)
3. Empty / Blanks Rooms issues [EZPI-224](https://jira.mios.com/browse/EZPI-224)
4. Issue with PWM Dimmer [EZPI-494](https://jira.mios.com/browse/EZPI-494)
5. Check sensor readings and make the value 0 if no valid data [EZPI-498](https://jira.mios.com/browse/EZPI-498)
6. Incorrect Parent-device initialized [EZPI-506](https://jira.mios.com/browse/EZPI-506)

## What's Changed

- Ezpi 253 hub settings by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/175
- Fix: decoding buffer length issue by @AbeereSpark in https://github.com/ezloteam/Ezlo_Pi/pull/176
- Ezpi 414 error codes by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/177
- Fix PWM dimmer issue : https://jira.mios.com/browse/EZPI-496 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/178
- Ezpi 493 fix missing expression value in broadcast by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/179
- Ezpi 479 501 hub scenes meta set by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/180
- Dev merge test 372 373 by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/181
- Ezpi 500 internal temperature sensor by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/182
- Implemented setting set command broadcast by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/183
- Added EZPI-505 and EZPI-506 by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/184
- code-refactored for parent-id assignation in child-devices by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/185
- Pre release by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/186
- Development by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/187
- Setting commands changes by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/188
- Development by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/189

**Full Changelog**: https://github.com/ezloteam/Ezlo_Pi/compare/v4.1.6...v4.2.7

# Version 4.1.6 Build 196

## New

1. Add remaining 'device/item_group' - comparison features in 'when' & 'then' [EZPI-437](https://jira.mios.com/browse/EZPI-437)
2. Implement isGroupItem_group : when-method [EZPI-464](https://jira.mios.com/browse/EZPI-464)
3. add expression-update feature when using 'hub.scenes.expressions.set'. [EZPI-465](https://jira.mios.com/browse/EZPI-465)
4. Implement Group_set_Item_value then-method [EZPI-456](https://jira.mios.com/browse/EZPI-456)
5. Implement Group_set_Device_armed then-method [EZPI-457](https://jira.mios.com/browse/EZPI-457)
6. Implement Group_toggle_value then-method [EZPI-458](https://jira.mios.com/browse/EZPI-458)

## Fixes

1. Avoid HTTP Requests if there is no valid HTTP/S URL [EZPI-474](https://jira.mios.com/browse/EZPI-474)
2. Issues when distinguishing between 'expression' and 'variable'. [EZPI-476](https://jira.mios.com/browse/EZPI-476)
3. Duplication of Expression/Variable with same name. [EZPI-475](https://jira.mios.com/browse/EZPI-475)
4. BLE Onboarding, cJSON incorrect cert length [EZPI-491](https://jira.mios.com/browse/EZPI-491)
5. Scenes with three or more triggers are not created - broadcasts about the successful creation of the scene are not received. [EZPI-320](https://jira.mios.com/browse/EZPI-320)
6. Bug found when triggering 'hub.device.room.set' [EZPI-473](https://jira.mios.com/browse/EZPI-473)

# Version 4.1.5 Build 195

## New

1. Added 'itemGroup' and 'DeviceGroups' [EZPI-427](https://jira.mios.com/browse/EZPI-427)
2. Add support for ESP32S2 - Beta [EZPI-471](https://jira.mios.com/browse/EZPI-471)

## Fixes

1.  Fix Timer getting error when there is no device. [EZPI-393](https://jira.mios.com/browse/EZPI-393)
2.  Store and optimize string constants to Flash memory [EZPI-396](https://jira.mios.com/browse/EZPI-396)
3.  Firmware RAM and Flash optimization [EZPI-424](https://jira.mios.com/browse/EZPI-424)
4.  EzloPi WS connection ON and OFF [EZPI-439](https://jira.mios.com/browse/EZPI-439)
5.  RAM optimization for Cloud Constants [EZPI-449](https://jira.mios.com/browse/EZPI-449)
6.  CRLF improper setting for ESP32 and C3 [EZPI-451](https://jira.mios.com/browse/EZPI-451)
7.  BLE Stack Optimization [EZPI-452](https://jira.mios.com/browse/EZPI-452)
8.  EzloPi Heap Shrink debug [EZPI-462](https://jira.mios.com/browse/EZPI-462)
9.  Fix EzloPi Buffer Rlease Fail [EZPI-463](https://jira.mios.com/browse/EZPI-463)
10. Fix Renaming devices never sticks and they get renamed back to their default names [EZPI-468](https://jira.mios.com/browse/EZPI-468)
11. Disable captive portal [EZPI-469](https://jira.mios.com/browse/EZPI-469)

# Version 4.1.4 Build 194

## New

1. Added 'setVariable' - Meshbot then_method [EZPI-426](https://jira.mios.com/browse/EZPI-426)
2. Added 'setExpression' - Meshbot then_method [EZPI-425](https://jira.mios.com/browse/EZPI-425)
3. Added 'itemGroup' and 'DeviceGroups' [EZPI-427](https://jira.mios.com/browse/EZPI-427)

## Fixes

1. Fix issue about device apperaing offline when WiFi network changed with changing device IP [EZPI-443](https://jira.mios.com/browse/EZPI-443)
2. Fix EzloPi WS connection ON and OFF [EZPI-439](https://jira.mios.com/browse/EZPI-439)
3. Fix issue of appearing `ezlopi_generic_0` on BLE client devices
4. Fix timer issue with custom loop implementation within the core

## What's Changed

- Development by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/147
- Pre release test meshbot with funtion fix by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/149
- Ezpi 441 complete room integration by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/148
- fixed meshbot-crashing issue on enable-true/false by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/150
- scenes_enable/disable_testing_complete by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/151
- Ezpi 443 keep tcp conn on ip change by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/154
- Ezpi 44 x ws integration by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/155
- Dev merge ezpi 375 422 425 426 427 by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/156
- Task Completed : https://jira.mios.com/browse/EZPI-445 by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/157
- Development revert wss by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/159
- Development by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/160

**Full Changelog**: https://github.com/ezloteam/Ezlo_Pi/compare/v4.1.3...v4.1.4

# Version 4.1.3 Build 191

## New

1. Enrollment of first-provisioning-api
2. Implemented \_\_loop to minimize the heap consumption. Followings has been added to loop:
   2.1: ping
   2.2: device/item routine call
   2.3: removed timer-events
   2.4: Cloud registration
   2.5: broadcast service
   2.6: LED indicator
   2.7: meshbot
   2.8: ota
   2.9: uart
3. Fixed touch-switch input
4. Cloud syncing for provisioning
5. Reset_scene_latches [https://jira.mios.com/browse/EZPI-371]
6. Added nvs update when activating : 'hub.scenes.block.status.reset' . [https://jira.mios.com/browse/EZPI-411]
7. Added 'hub.scenes.block.enabled.set' method . [https://jira.mios.com/browse/EZPI-413]
8. Added 'hub.scenes.house_modes.set' method . [https://jira.mios.com/browse/EZPI-415]
9. Added 'hub.scenes.action.block.test' method . [https://jira.mios.com/browse/EZPI-417]
10. Added 2-broadcast methods for :- 'hub.scenes.expressions.set' . [https://jira.mios.com/browse/EZPI-419]
11. Added broadcast method for :- 'hub.expression.deleted' . [https://jira.mios.com/browse/EZPI-420]

## What's Changed

- Update README.md by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/140
- Combined ezpi 381 400 401 402 403 404 by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/139
- 1. bug_fixed in 'EZPI_firmware_info_get' function by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/142
- Dev merge ezpi 371 411 413 415 417 419 420 by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/141
- Development by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/143
- Ezpi wifi scanner issue by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/144
- added device/item to loop in every 1-seconds by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/145
- Pre release ezlopi core 4.x.x by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/146

# Version 3.5.9 Build 189

## New

1. Added `cmd 99` as an heartbeat on UART
2. Configurable cloud log and trace [EZPI-405](https://jira.mios.com/browse/EZPI-405)

## Fixes

1. Fix ping issue with expanding the idle task stack [EZPI-392](https://jira.mios.com/browse/EZPI-392)
2. EzloPi Distros cleanup [EZPI-395](https://jira.mios.com/browse/EZPI-395)
3. Fixed broadcast issues when activating hub.scenes.delete
4. Found fixed a possible bug case in 'scenes.changed' broadcast method
5. fixed an error in ` cjext : 'isCJSON_NULL'`--> always returns 'null'.
6. Replaced `CJSON_GET_VALUE_DOUBLE` with `CJSON_GET_VALUE_GPIO` for all gpios
7. Fixed DHT22 pin issue
8. Fixed `EZPI_core_factory_info_v3_set_ezlopi_config`
9. Cleaned multiple occurrence of `\r\n`

## What's Changed

- Soved the issue of 0 memory usage in status get API by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/130
- Ezpi 405 cofigurable cloud log and trace by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/131
- Fixed incorrect broadcast problem : https://jira.mios.com/browse/EZPI… by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/132
- Ezpi 392 ping issue by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/133
- Ezpi 405 cofigurable cloud log and trace by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/134
- Ezpi 395 distro clean by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/135
- Ezpi 395 distro clean by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/137
- Dev old compare string by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/136
- Development by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/138

**Full Changelog**: https://github.com/ezloteam/Ezlo_Pi/compare/v3.5.8...v3.5.9

# Version 3.5.8 Build 187

## New

1. Implementation of 6-function-APIs (for less, for at-least, repeat, follow, pulse, latch): [EZPI-351](https://jira.mios.com/browse/EZPI-351)
2. Implement EzloPi Device Statuses API [EZPI-254](https://jira.mios.com/browse/EZPI-254)
3. Change Serial Config, Update NVS and Reboot EzloPi Device if only new config [EZPI-365](https://jira.mios.com/browse/EZPI-365)
4. Implement hub.device.armed.set API [EZPI-379](https://jira.mios.com/browse/EZPI-379)
5. EzloPi Distro Design and implementation [EZPI-385](https://jira.mios.com/browse/EZPI-385)
6. EzloPi - Framework for house-modes [EZPI-280](https://jira.mios.com/browse/EZPI-280)
7. 'hub.modes.current.get' api integration [EZPI-292](https://jira.mios.com/browse/EZPI-292)
8. Implementation of then-method 'toggle-value' [EZPI-349](https://jira.mios.com/browse/EZPI-349)
9. Implementation of isHouseMode_changed_from_method [EZPI-366](https://jira.mios.com/browse/EZPI-366)
10. Implementation of switchHouseMode_then_method [EZPI-366](https://jira.mios.com/browse/EZPI-366)
11. Implementation of Log Control from Serial [EZPI-388](https://jira.mios.com/browse/EZPI-388)
12. Add chip compatibility for config [EZPI-389](https://jira.mios.com/browse/EZPI-389)

## Fixes

1. Memory leak fixing causing device to go offline [EZPI-369](https://jira.mios.com/browse/EZPI-369)
2. Add Filter on serial info such that if not provisioned send data 0 [EZPI-353](https://jira.mios.com/browse/EZPI-353)
3. Implement cloud API controlled logs for cloud and serial on EzloPi [EZPI-370](https://jira.mios.com/browse/EZPI-370)
4. Cleanup distro merge [EZPI-378](https://jira.mios.com/browse/EZPI-378)
5. Fix Flash overflow with converting LOG macros into a function based implementaion [EZPI-384](https://jira.mios.com/browse/EZPI-384)
6. Connect to SNTP if only WiFi connected and reduce delay time [EZPI-386](https://jira.mios.com/browse/EZPI-386)
7. Meshbots Branch "review" refactoring [EZPI-261](https://jira.mios.com/browse/EZPI-261)
8. Test and fixing on 'house-modes' [EZPI-301](https://jira.mios.com/browse/EZPI-301)
9. When several scenes (more than 6) are created, the next created scenes stop coming from the controller [EZPI-250](https://jira.mios.com/browse/EZPI-250)
10. Fix invalid '\_item_ID' in toggle_value method [EZPI-390](https://jira.mios.com/browse/EZPI-390)

## What's Changed

- Development by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/109
- release note. by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/110
- added mDNS config disable in K-Config. by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/111
- Ezlopi distros by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/112
- Ezpi 308 hub status get ii by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/113
- Dev function to be merged into 3.5.7 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/114
- Pre release 3.5.7 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/115
- removed unneccessary warnings by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/116
- Ezpi 370 implement hub log by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/117
- Device free bug fix pre release 3.5.7 by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/118
- Pre release 3.5.7 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/119
- Development by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/120
- fixed core-processess when disabled by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/121
- Dev adding toggle value and housemode changed from by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/122
- Ezpi 376 fix memory leak by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/123
- Pre release next by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/124
- Pre release 3.5.8 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/125
- Fixed : invalid_item_id in 'toggle_value_method' ; https://jira.mio.com/browse/EZPI-390 by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/126
- Added UART service for log severity by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/127
- Development by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/128

**Full Changelog**: https://github.com/ezloteam/Ezlo_Pi/compare/v3.4.7...v3.5.8

# Version 3.4.7 Build 186

## New:

1. Implementation of heap-memory guard when 'scene_create' triggered: [EZPI-358](https://jira.mios.com/browse/EZPI-358)
2. Implementation of 'toggle-value' method; [testing remained] [EZPI-349](https://jira.mios.com/browse/EZPI-349)
3. Implementation fake Token based security on local API [EZPI-341](https://jira.mios.com/browse/EZPI-341)
4. Implementation of WiFi Change API [EZPI-302](https://jira.mios.com/browse/EZPI-302)
5. Implementation of Battery status on Firmware Core [EZPI-332](https://jira.mios.com/browse/EZPI-332)
6. Implementation of Status indicator based on LED and BLE for EzloPi [EZPI-344](https://jira.mios.com/browse/EZPI-344)
7. Implementation of UART (serial) configuration from serial and BLE [EZPI-347](https://jira.mios.com/browse/EZPI-347)

## Fixes:

1. Fix device tree free-issue when initialization: [EZPI-333](https://jira.mios.com/browse/EZPI-333)
2. Fix 'NOT' scene method parsing failure: [EZPI-363](https://jira.mios.com/browse/EZPI-363)
3. Fix 'Scene_State' enable/disable not update in NVS: [EZPI-364](https://jira.mios.com/browse/EZPI-364)
4. mDNS Service type modification: [EZPI-356](https://jira.mios.com/browse/EZPI-356)
5. Replacenemt of dynamic buffer to static buffer for minimizing heap fragmentation [EZPI-352](https://jira.mios.com/browse/EZPI-352)
6. WiFi SSID and PSW to be matched len with factory info [EZPI-316](https://jira.mios.com/browse/EZPI-316)

## What's Changed

- Implemented scene state change by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/88
- Meshbot edit ezpi 324 by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/87
- Sensor device init bug fix by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/89
- Hotfix heap fragmentation by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/90
- Development merge add parent by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/92
- refactored parent_child_device naming by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/94
- implementation of https://jira.mios.com/browse/EZPI-341 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/95
- Hotfix heap fragmentation v3 by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/96
- Baud rate config ezpi 347 by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/97
- implemented https://jira.mios.com/browse/EZPI-356 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/99
- Uart info command issue for new device serial parameters solved by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/100
- Ezpi344 ble service addition by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/98
- ESP32 indicator led dimmable by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/101
- finalized serial config https://jira.mios.com/browse/EZPI-347 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/102
- added not_method by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/103
- added fix for setSceneState by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/104
- Development fix scene get status and run scene.EZPI-358 by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/105
- Development new scene 30kb guard by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/106
- Wrap up api for initiative by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/107
- Pre release v3.4.7 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/108

**Full Changelog**: https://github.com/ezloteam/Ezlo_Pi/compare/v3.3.6...v3.4.7

# Version 3.3.6 Build 185

## Supported Ezlo Cloud APIs

1. [Items](https://api.ezlo.com/hub/items_api/index.html)

   - [hub.items.list](https://api.ezlo.com/hub/items_api/index.html#hubitemslist)
   - [hub.item.value.set (single item)](https://api.ezlo.com/hub/items_api/index.html#hubitemvalueset-single-item)

2. [Devices](https://api.ezlo.com/hub/devices_api/index.html)

   - [hub.devices.list](https://api.ezlo.com/hub/devices_api/index.html#hubdeviceslist)
   - [hub.device.name.set](https://api.ezlo.com/hub/devices_api/index.html#hubdevicenameset)
   - [hub.device.room.set](https://api.ezlo.com/hub/devices_api/index.html#hubdeviceroomset)

3. [Device Settings](https://api.ezlo.com/hub/settings_api/index.html)

   - [hub.device.settings.list](https://api.ezlo.com/hub/settings_api/index.html#hubdevicesettingslist)
   - [hub.device.setting.value.set](https://api.ezlo.com/hub/settings_api/index.html#hubdevicesettingvalueset)
   - [hub.device.setting.reset](https://api.ezlo.com/hub/settings_api/index.html#hubdevicesettingreset)

4. [Room](https://api.ezlo.com/hub/room/index.html)

   - [hub.room.list](https://api.ezlo.com/hub/room/index.html#hubroomlist)
   - [hub.room.create](https://api.ezlo.com/hub/room/index.html#hubroomcreate)
   - [hub.room.name.set](https://api.ezlo.com/hub/room/index.html#hubroomnameset)
   - [hub.room.get](https://api.ezlo.com/hub/room/index.html#hubroomget)
   - [hub.room.delete](https://api.ezlo.com/hub/room/index.html#hubroomdelete)

5. [Common commands](https://api.ezlo.com/hub/common/index.html)

   - [hub.info.get](https://api.ezlo.com/hub/common/index.html#hubinfoget)
   - [hub.reboot](https://api.ezlo.com/hub/common/index.html#hubreboot)

6. [Network](https://log.ezlo.com/new/hub/network/)

   - [hub.network.get](https://log.ezlo.com/new/hub/network/#hubnetworkget)
   - [hub.network.wifi.scan.start](https://log.ezlo.com/new/hub/network/#hubnetworkwifiscanstart)
   - [hub.network.wifi.scan.stop](https://log.ezlo.com/new/hub/network/#hubnetworkwifiscanstop)

7. [Time configuration](https://api.ezlo.com/hub/time/index.html)

   - [hub.time.location.set](https://api.ezlo.com/hub/time/index.html#hubtimelocationset)
   - [hub.time.location.get](https://api.ezlo.com/hub/time/index.html#hubtimelocationget)
   - [hub.time.location.list](https://api.ezlo.com/hub/time/index.html#hubtimelocationlist)

8. [House Modes commands](https://api.ezlo.com/hub/house_modes_manager/index.html#house-modes-commands)

   - [hub.modes.get](https://api.ezlo.com/hub/house_modes_manager/index.html#hubmodesget)
   - [hub.modes.current.get](https://api.ezlo.com/hub/house_modes_manager/index.html#hubmodescurrentget)
   - [hub.modes.switch](https://api.ezlo.com/hub/house_modes_manager/index.html#hubmodesswitch)

9. [Broadcasts](https://api.ezlo.com/hub/broadcasts/index.html)

   - [hub.item.updated](https://api.ezlo.com/hub/broadcasts/index.html#hubitemupdated)
   - [hub.room.created](https://api.ezlo.com/hub/broadcasts/index.html#hubroomcreated)
   - [hub.room.deleted](https://api.ezlo.com/hub/broadcasts/index.html#hubroomdeleted)
   - [hub.room.reordered](https://api.ezlo.com/hub/broadcasts/index.html#hubroomreordered)
   - [hub.room.edited](https://api.ezlo.com/hub/broadcasts/index.html#hubroomedited)
   - [hub.scene.added](https://api.ezlo.com/hub/broadcasts/index.html#hubsceneadded)
   - [hub.scene.deleted](https://api.ezlo.com/hub/broadcasts/index.html#hubscenedeleted)
   - [hub.scene.changed](https://api.ezlo.com/hub/broadcasts/index.html#hubscenechanged)
   - [hub.scene.run.progress](https://api.ezlo.com/hub/broadcasts/index.html#hubscenerunprogress)
   - [hub.network.wifi.scan.progress](https://api.ezlo.com/hub/broadcasts/index.html#hubnetworkwifiscanprogress)
   - [hub.device.setting.updated](https://api.ezlo.com/hub/broadcasts/index.html#hubdevicesettingupdated)

10. [Scenes](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html)

    - [hub.scenes.create](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenescreate)
    - [hub.scenes.get](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesget)
    - [hub.scenes.list](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubsceneslist)
    - [hub.scenes.edit](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesedit)
    - [hub.scenes.delete](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesdelete)
    - [hub.scenes.block.data.list](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesblockdatalist)
    - [hub.scenes.enabled.set](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesenabledset)
    - [hub.scenes.notification.add](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesnotificationadd)
    - [hub.scenes.notification.remove](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesnotificationremove)
    - [hub.scenes.run](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesrun)
    - [hub.scenes.name.set](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesnameset)
    - [hub.scenes.expressions.set](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesexpressionsset)
    - [hub.scenes.expressions.delete](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesexpressionsdelete)
    - [hub.scenes.expressions.list](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesexpressionslist)

11. [Scenes : When blocks](https://api.ezlo.com/hub/scenes/when_blocks/index.html)

    - [isItemState](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isitemstate)
    - [isItemStateChanged](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isitemstatechanged)
    - [isInterval](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isinterval)
    - [isSunState](https://api.ezlo.com/hub/scenes/when_blocks/index.html#issunstate)
    - [isOnce](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isonce)
    - [isDateRange](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isdaterange)
    - [isDeviceState](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isdevicestate)
    - [isSceneState](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isscenestate)
    - [isCloudState](https://api.ezlo.com/hub/scenes/when_blocks/index.html#iscloudstate)
    - [compareNumbers](https://api.ezlo.com/hub/scenes/when_blocks/index.html#comparenumbers)
    - [compareNumberRange](https://api.ezlo.com/hub/scenes/when_blocks/index.html#comparenumberrange)
    - [compareStrings](https://api.ezlo.com/hub/scenes/when_blocks/index.html#comparestrings)
    - [stringOperation](https://api.ezlo.com/hub/scenes/when_blocks/index.html#stringoperation)
    - [inArray](https://api.ezlo.com/hub/scenes/when_blocks/index.html#inarray)
    - [compareValues](https://api.ezlo.com/hub/scenes/when_blocks/index.html#comparevalues)
    - [hasAtLeastOneDictionaryValue](https://api.ezlo.com/hub/scenes/when_blocks/index.html#hasatleastonedictionaryvalue)
    - [isFirmwareUpdateState](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isfirmwareupdatestate)
    - [isDictionaryChanged](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isdictionarychanged)
    - [and](https://api.ezlo.com/hub/scenes/when_blocks/index.html#and)
    - [not](https://api.ezlo.com/hub/scenes/when_blocks/index.html#not)
    - [or](https://api.ezlo.com/hub/scenes/when_blocks/index.html#or)
    - [xor](https://api.ezlo.com/hub/scenes/when_blocks/index.html#xor)

12. [Scenes : Action blocks](https://api.ezlo.com/hub/scenes/action_blocks/index.html#action-blocks)
    - [setItemValue](https://api.ezlo.com/hub/scenes/action_blocks/index.html#setitemvalue)
    - [sendHttpRequest](https://api.ezlo.com/hub/scenes/action_blocks/index.html#sendhttprequest)
    - [runCustomScript](https://api.ezlo.com/hub/scenes/action_blocks/index.html#runcustomscript)
    - [rebootHub](https://api.ezlo.com/hub/scenes/action_blocks/index.html#reboothub)
    - [resetHub](https://api.ezlo.com/hub/scenes/action_blocks/index.html#resethub)

## Supported Sensors

1. MPU6050 Accelerometer and Gyroscope
2. ADXL345 3-Axis Accelerometer
3. GY-271 Magnetometer
4. LTR303ALS Luminosity sensor
5. RMT SK6812
6. BMP680 I2C Sensor
7. BMP280 I2C Sensor
8. DHT11 Temperature Humidity Sensor
9. DHT22 Temperature Humidity Sensor
10. Potentiometer
11. Internal Hall Effect Sensor
12. PIR Sensor
13. Parallax 2-Axis Joystick
14. MB1013 Sonar
15. PWM Generic Load
16. Touch Switch TTP223B (Toggle)
17. Ultrasonic Sensor HC SR04
18. LDR Digital and analog Sensor
19. Water Leak Sensor
20. GY61 3-Axis Accelerometer
21. GXHTC3 - RH and T Sensor
22. DS18B20 Temperature Sensor
23. JSN SR04T Water Level Sensor
24. Soil Moisture Sensor
25. Turbidity Sensor
26. Proximity Sensor
27. Touch switch TPP 223B (Momentary)
28. Servo Motor MG996R
29. SK6812 RGB LEDs
30. TCS230 Sensor
31. FC28 Soil Moisture Sensor
32. Shunt voltmeter
33. UV Intensity Sensor
34. TSL2561 Luminosity Sensor
35. CS712TELC 05B Current Sensor
36. HX711 Load Cell
37. MQ4 Methane Gas Sensor
38. MQ2 LPG Gas Sensor
39. MQ3 Alcohol Sensor
40. MQ8 H2 Gas Sensor
41. MQ135 NH3 Gas Sensor
42. GPS GYGPS6MV2
43. YFS201 flowmeter
44. Flex Resistor
45. Force Sensitive Resistor
46. KY026 Flame Sensor
47. MQ6 LPG Detector
48. Vibration Sensor
49. Reed Switch
50. MQ7 CO Detector
51. MQ9 LPG Detector
52. Float Switch
53. R307 Fingerprint Module
54. HLK-LD2410B Presence Sensor
55. ENS160 gas sensor
56. ZE08 ch02 gas sensor

## Supported Controllable Devices

1. PWM Dimmable
2. RGB Dimmable
3. Smart Plug / Relay
4. Servo Mot0r
5. Addressable LEDs

## Firmware Features

### Serial Command

EzloPi can be configured with serial interface as well. Following are common serial interface commands :

#### Action: Device management

This command is for device administrative purpose such as device restart, device factory restart etc

```
Request

Factory Restore
{
    "cmd": 0,
    "sub_cmd": 0 // Factory Restore only
}

Just Reboot
{
    "cmd": 0,
    "sub_cmd": 1 // Reboot only
}

Response :
{
  "cmd": 0,
  "status": 1 // Status of the command execution, 0 = failed, 1 = success
}
```

#### Action: Get Info

```
Request

{
"cmd": 1
}
Just Reboot
{
    "cmd": 0,
    "sub_cmd": 1 // Reboot only
}

Response :

{
    "cmd": 1,
    "firmware_version": "3.3.6",
    "build": 185,
    "chip_info": {
        "chip_type": "ESP32-S3",
        "chip_version": "0.01",
        "firmware_SDK_name": "ESP-IDF",
        "firmware_SDK_version": "v4.4.6-dirty"
    },
    "uptime": "0d 0h 30m 7s",
    "build_date": 1701862983,
    "boot_count": 354,
    "boot_reason": "Power ON",
    "mac": "2A:4B:6F:8E:1D:3C",
    "uuid": "fb3c84b0-534c-11ec-b2d6-8f260f5287fa",
    "uuid_prov": "065320-838d-11ee-8889-0f56091e7639",
    "serial": 102000529,
    "ssid": "ESP",
    "device_name": "__NAME",
    "ezlopi_device_type": "ezlopi_generic",
    "device_total_flash": "8MB",
    "brand": "__BRAND",
    "manufacture": "__MANU",
    "model": "__MODEL",
    "net_info": {
        "wifi_mode": "STA",
        "wifi_connection_status": true,
        "ip_sta": "192.168.100.19",
        "ip_nmask": "255.255.255.0",
        "ip_gw": "192.168.100.2",
        "internet_status": "unknown",
        "cloud_connection_status": false
    }
}
```

#### Action: Set Wi-Fi

```
Request

{
  "cmd": 2,
  "pass": "myWiFiPass",
  "ssid": "my wifi ssid"
}

Response :
{
  "cmd": 2,
  "status_write": 1,
  "status_connect": 1
}
```

#### Action Device Config

```
Request

{
  "cmd": 4
}

Response :

{
    "config_id": "_ID_",
    "config_time": 1696508363,
    "dev_total": 3,
    "config_name": "My moisture sensor",
    "dev_detail": [
        {
            "dev_type": 1,
            "dev_name": "Dining Room Main Lamp",
            "id_room": "",
            "id_item": 2,
            "val_ip": true,
            "val_op": false,
            "gpio_in": 21,
            "gpio_out": 13,
            "is_ip": false,
            "ip_inv": true,
            "pullup_ip": true,
            "pullup_op ": true,
            "op_inv": false
        },
        {
            "dev_type": 2,
            "dev_name": "Switch",
            "id_room": "",
            "id_item": 4,
            "gpio": 21,
            "pull_up": true,
            "logic_inv": true
        },
        {
            "dev_type": 8,
            "dev_name": "Sensor for my Dog",
            "id_room": "",
            "id_item": 5,
            "gpio_sda": 21,
            "gpio_scl": 22,
            "pullup_scl": true,
            "pullup_sda": true,
            "slave_addr": 105
        }
    ]
}
```

### BLE WiFi Onboarding

BLE (Bluetooth Low Energy) WiFi on-boarding for EzloPi Devices simplifies the process of connecting EzloPi devices to a WiFi network. Using BLE technology, the device can initiate a secure and user-friendly on-boarding process without the need for direct user input on the device.

### BLE Provisioning

BLE provisioning for EzloPi devices enhances user convenience by providing a wireless and user-friendly method for initial setup, eliminating the need for complex manual configurations directly on the device.

### BLE Pin Configurations

BLE Configuration simplifies the process of configuring the GPIOs of a hardware microcontroller employed by EzloPi, enhancing user convenience.

### OTA

This feature allows for convenient and remote firmware upgrades, ensuring that EzloPi devices can receive the latest software improvements and enhancements without the need for manual intervention or physical access.

### mDNS Discovery

The EzloPi can be discovered locally with following services. The hostname would be `ezlopi_<last 4 digits of Serial id>`

## Local Meshbot (Scene)

Scenes provide the possibility to make the relations between devices and make some actions with them. A scene consists of "conditions" and "actions" represented by (when blocks and action blocks respectively. All of them, except action groups, have Json Array representation.

In the perfect case when and exceptions ("conditions") block must form Abstract Syntax Tree (AST) from the Json and be executable. Currently, it's represented as a list of possible single-argument function.
then and else (action) blocks contain the list of actions. Each functional element actually is the item. As far as you can see there are read only items such as : temperature, pressure, humidity, etc. and there are read-write items : switch, dimmer, light(switch), doorlock, etc...

## Changes

- TSL256 Lux sensor V3 by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/37
- Dev v3 device config by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/40
- Feature scenes expression by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/42
- Dev v3 scene merge by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/43
- Dev v3 sensor hilink ezpi 268 by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/44
- Dev v3 dir refactor ezpi 271 by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/45
- Refactoring sensor-device driver files. by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/46
- PMS5003 integration to EzloPi(issue: https://jira.mios.com/browse/EZP… by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/47
- sensor-devices dir_name re-structured by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/48
- Dev v3 dir refactor ezpi 265 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/49
- Dev v3 dir refactor by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/50
- Worked on warning : https://jira.mios.com/browse/EZPI-273 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/51
- Dev v3 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/52
- Removed static variables and worked on warning in sensor drivers by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/53
- Feature hub scene edit by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/54
- Development ezpi255 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/55
- fixed mac address. by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/56
- Development ezpi255 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/57
- Development pr template ezpi 286 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/58
- Implement Factory Restore Feature on Serial Command by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/62
- Ble heap consumption ezpi 287 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/63
- complete : https://jira.mios.com/browse/EZPI-299 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/66
- Feature house modes by @krishna-ezlo in https://github.com/ezloteam/Ezlo_Pi/pull/64
- Ezpi 300 integrate test new sensors by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/65
- EZPI issue: https://jira.mios.com/browse/EZPI-303 by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/67
- Added nvs read and write. by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/69
- Ezpi 305 implement network scan command by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/68
- Development fix by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/73
- Ezpi312 run scene implementation by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/72
- added necessary 'item->user_arg' and valueFormatted malloc guards. EZ… by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/70
- https://jira.mios.com/browse/EZPI-316 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/74
- added runCustomScript .EZPI-313: https://jira.mios.com/browse/EZPI-313 by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/76
- Mdns implementation by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/78
- Development gpio not found fix by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/81
- Mdns ws implementation by @NDJ-EZLO in https://github.com/ezloteam/Ezlo_Pi/pull/82
- ble tested : https://jira.mios.com/browse/EZPI-323 by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/83
- Development gpio num fix by @ezlo-rikenm in https://github.com/ezloteam/Ezlo_Pi/pull/84
- modified mdns structure. by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/85
- Development by @lomasSE89 in https://github.com/ezloteam/Ezlo_Pi/pull/86

## New Contributors

- @lomasSE89 made their first contribution in https://github.com/ezloteam/Ezlo_Pi/pull/43
- @ezlo-rikenm made their first contribution in https://github.com/ezloteam/Ezlo_Pi/pull/46

**Full Changelog**: https://github.com/ezloteam/Ezlo_Pi/compare/v3.0.1...v3.3.6
