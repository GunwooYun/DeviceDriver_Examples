cmd_/home/ubuntu/workplace/DeviceDriver_Examples/printk/modules.order := {   echo /home/ubuntu/workplace/DeviceDriver_Examples/printk/hello.ko; :; } | awk '!x[$$0]++' - > /home/ubuntu/workplace/DeviceDriver_Examples/printk/modules.order