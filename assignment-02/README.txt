# Clean old config
cd linux-X.XX
make mrproper # Delete config files

# Change EXTRAVERSION field
## Option 1:
1. make menuconfig
2. On kernel config menu, go to general config, local version and set config to: '-thor_kernel'

## Option 2:
1. make menuconfig
2. vim Makefile
3. change EXTRAVERSION = -thor_kernel

# Generate the patch
git format-patch HEAD~

# Compile modules and kernel
make modules_install # Install enabled modules
make # Compile the kernel

# Install the new kernel
## Option 1: Replace the kernel files
1. cp -iv arch/x86/boot/bzImage /boot/vmlinuz-4.15.3-lumenthi # Kernel img
2. cp -iv System.map /boot/System.map-4.15.3 # Kernel symbol table
3. reboot

## Option 2: Directly boot to the new kernel via grub
1. cp -iv arch/x86/boot/bzImage /boot/vmlinuz-X.XX # Kernel img
2. cp -iv System.map /boot/System.map-X.XX # Kernel symbol table
3. add a new entry in /etc/grub.d/40_custom # Check 40_custom
4. reboot and boot to the new kernel

# Turn in boot log
sudo dmesg > kern.log
