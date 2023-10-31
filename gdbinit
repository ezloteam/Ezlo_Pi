target remote :3333
set remote hardware-watchpoint-limit 2
mon reset halt
flushregs
thb ezlopi_scenes_scripts_nvs_parse
c