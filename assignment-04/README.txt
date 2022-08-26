# Udev rules location
/usr/lib/udev/rules.d/ system default (lowest priority)
/etc/udev/rules.d/ custom rules (highest priority)

# Priority
Higher number overrule lower numbers

============
# When plugin a keyboard in VM
$ sudo lssub -v
[...]
	iProduct                2 Gaming Keyboard
	[...]
      bInterfaceClass         3 Human Interface Device
      bInterfaceSubClass      1 Boot Interface Subclass
      bInterfaceProtocol      1 Keyboard
[...]

# Put the corresponding attributes in the rule file
SUBSYSTEM=="usb",\
ATTRS{bInterfaceClass}=="03",\
ATTRS{bInterfaceSubClass}=="01",\
ATTRS{bInterfaceProtocol}=="01",\

# Define the actions
ACTION=="add", RUN+="/usr/sbin/insmod /mnt/lfs/home/user/little-penguin/assignment-04/main.ko"

# Copy the file in custom rules folder (so you have higher priority)
sudo cp 01-usb-keyboard.rules /etc/udev/rules.d

# Reload rules
sudo udevadm control --reload-rules

====================
# MONITORING PART
# Now i plug the keyboard again and monitor tools so i get the device path

$ sudo udevadm monitor
[...]
UDEV  [89.871060] bind     /devices/pci0000:00/0000:00:06.0/usb1/1-2/1-2:1.0/0003:0603:00F7.0002 (hid)
UDEV  [89.871687] bind     /devices/pci0000:00/0000:00:06.0/usb1/1-2/1-2:1.0 (usb) # This is our USB device
UDEV  [89.881067] bind     /devices/pci0000:00/0000:00:06.0/usb1/1-2 (usb)

$ sudo udevadm test -a add /devices/pci0000:00/0000:00:06.0/usb1/1-2/1-2:1.0
[...]
Reading rules file: /etc/udev/rules.d/01-usb-keyboard.rules # This is our rule
[...]
run: '/usr/sbin/insmod /mnt/lfs/home/user/little-penguin/assignment-04/main.ko' # Our rule is well executed
run: 'kmod load usb:v0603p00F7d0723dc00dsc00dp00ic03isc01ip01in00'
[...]

$ sudo tail -F /var/log/messages # When plugin the keyboard
[...]
Aug 26 19:00:47 debian kernel: [  115.437615] usb 1-2: New USB device found, idVendor=0603, idProduct=00f7, bcdDevice= 7.23
Aug 26 19:00:47 debian kernel: [  115.437617] usb 1-2: New USB device strings: Mfr=1, Product=2, SerialNumber=0
Aug 26 19:00:47 debian kernel: [  115.437617] usb 1-2: Product: Gaming Keyboard
Aug 26 19:00:47 debian kernel: [  115.437618] usb 1-2: Manufacturer: SINO WEALTH
Aug 26 19:00:47 debian kernel: [  115.457344] input: SINO WEALTH Gaming Keyboard as /devices/pci0000:00/0000:00:06.0/usb1/1-2/1-2:1.0/0003:0603:00F7.0002/input/input9
Aug 26 19:00:47 debian kernel: [  115.514444] hid-generic 0003:0603:00F7.0002: input,hidraw1: USB HID v1.11 Keyboard [SINO WEALTH Gaming Keyboard] on usb-0000:00:06.0-2/input0
Aug 26 19:00:47 debian kernel: [  115.597957] input: SINO WEALTH Gaming Keyboard Keyboard as /devices/pci0000:00/0000:00:06.0/usb1/1-2/1-2:1.1/0003:0603:00F7.0003/input/input12
Aug 26 19:00:47 debian mtp-probe: checking bus 1, device 3: "/sys/devices/pci0000:00/0000:00:06.0/usb1/1-2"
Aug 26 19:00:47 debian mtp-probe: bus: 1, device: 3 was not an MTP device
Aug 26 19:00:47 debian kernel: [  115.652986] main: loading out-of-tree module taints kernel.
Aug 26 19:00:47 debian kernel: [  115.653006] main: module verification failed: signature and/or required key missing - tainting kernel
Aug 26 19:00:47 debian kernel: [  115.653045] Hello keyboard ! # Hello keyboard :)
[...]
