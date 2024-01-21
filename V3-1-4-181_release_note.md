
## Release Notes

Date : 20 Jan 2024, 
Version : 3.2.4
Build : 182

### New Features

1. Added Servo motor support for firmware > V3
2. Added Joystick support for firmware > V3
3. Added Hi-Link Presence sensor support
4. Implemented rooms methods
 - hub.room.list
- hub.room.create
- hub.room.name.set
- hub.room.order.set
- hub.room.get
- hub.room.delete
- hub.room.all.delete
- hub.room.created
- hub.room.edited
- hub.room.deleted
- hub.room.reordered

5. Implemented device methods
 - hub.device.name.set
-  hub.device.updated

6. Added device-id in device-config(i.e. ezlopi-config) and stored it to the factory-info (i.e. id.bin)

7. Implemented scenes methods
- hub.scenes.notifications.add
- hub.scenes.notifications.remove
- hub.scenes.blocks.list
- hub.scene.run
- hub.scene.deleted
- hub.scene.changed
- hub.scene.added
- hub.scenes.expressions.set
- hub.scenes.expressions.list
- hub.scenes.expressions.delete



### Enhancements 

BLE features addition and enhancements
- Added new service for dynamic configuration
-- Write dynamic config to factory info (i.e. id.bin)
-- Read dynamic config from factory info (i.e. id.bin)
- Added reset functions
-- Device soft-reset
-- Device hard-reset [added, implementation remaining]
-- Device factory-reset [added, implementation remaining]
- Added Provisioning
-- Provisioning read
-- Provisioning write

### Fixes

1. issue fixed while storing device-config in factory info (i.e. id.bin)
- issue fixed at freeing devices
- added return at __init functions in devices/sensors
- added action-methods hub.scene.block.data.list
- added expressions components in hub.scene.block.data.list
- added cJSON in registration method
- segregated strings into a file to minimize warnings
- Fixed warnings due data-type mis-match
- added parent-id in device-structre and device-list response
- Optimized memory usages - freed buffers just after its use is complete instead of completing the script