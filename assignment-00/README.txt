# Clone linux kernel
git clone git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
cd linux-X.XX

# Change CONFIG_LOCALVERSION_AUTO
## Option 1:
1. make defconfig # Generate config for current architecture
2. make menuconfig # Edit config file
3. On kernel config menu, go to general setup, enable "Automatically append version information to the version string"

## Option2:
1. make defconfig # Generate config for current architecture
2. vim .config # To edit config
3. append CONFIG_LOCALVERSION_AUTO=y

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
3. add a new entry in /etc/grub.d/01_ft_linux # Check 01_ft_linux
4. reboot and boot to the new kernel

# Turn in files
sudo dmesg > kern.log
cp linux-X.XX/.config ./config
