
<picture>
  <img src="ezlopi-doc/ezlopi_logo.png" >
</picture>

# RELEASE NOTES

## New:
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
11. Implementation of isHouseModeAlarmPhaseRange [EZPI-367](https://jira.mios.com/browse/EZPI-367)
12. Implementation of isHouseModeSwitchToRange [EZPI-373](https://jira.mios.com/browse/EZPI-373)
13. Implementation of Log Control from Serial [EZPI-388](https://jira.mios.com/browse/EZPI-388)
14. Add chip compatibility for config [EZPI-389](https://jira.mios.com/browse/EZPI-389)

## Fixes:
1. Memory leak fixing causing device to go offline [EZPI-369](https://jira.mios.com/browse/EZPI-369)
2. Add Filter on serial info such that if not provisioned send data 0 [EZPI-353](https://jira.mios.com/browse/EZPI-353)
3. Implement cloud API controlled logs for cloud and serial on EzloPi	 [EZPI-370](https://jira.mios.com/browse/EZPI-370)
4. Cleanup distro merge [EZPI-378](https://jira.mios.com/browse/EZPI-378)
5. Fix Flash overflow with converting LOG macros into a function based implementaion [EZPI-384](https://jira.mios.com/browse/EZPI-384)
6. Connect to SNTP if only WiFi connected and reduce delay time [EZPI-386](https://jira.mios.com/browse/EZPI-386)
7. Meshbots Branch "review" refactoring [EZPI-261](https://jira.mios.com/browse/EZPI-261)
8. Test and fixing on 'house-modes' [EZPI-301](https://jira.mios.com/browse/EZPI-301)
9. When several scenes (more than 6) are created, the next created scenes stop coming from the controller [EZPI-250](https://jira.mios.com/browse/EZPI-250)
10. Fix invalid '_item_ID' in toggle_value method [EZPI-390](https://jira.mios.com/browse/EZPI-390)