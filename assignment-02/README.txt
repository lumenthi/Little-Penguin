# Clean old config
cd linux-X.XX
make mrproper # Delete config files

# Change EXTRAVERSION field
## Option 1:
1. make defconfig
2. make menuconfig
3. On kernel config menu, go to general config, local version and set config to: '-thor_kernel'

## Option 2(the one we must choose for this assignment):
1. make defconfig
2. make menuconfig
3. vim Makefile
4. change EXTRAVERSION = -thor_kernel

## Option 3:
1. make defconfig
2. make menuconfig
3. vim .config
4. change CONFIG_LOCALVERSION="-thor_kernel"

# Generate the patch
1. Edit the makefile
2. Commit the changes
3. git format-patch -1 HEAD Makefile

# Install the new kernel
## Boot to the new kernel via grub
1. cp -iv arch/x86/boot/bzImage /boot/vmlinuz-X.XX # Kernel img
2. cp -iv System.map /boot/System.map-X.XX # Kernel symbol table
3. add a new entry in /etc/grub.d/01_ft_linux # Check 01_ft_linux
4. reboot and boot to the new kernel

# Turn in boot log
sudo dmesg > kern.log
