=========================INSTALLATION============================
# Clone linux-next kernel
git clone git://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git

# Generate config file
make defconfig
make menuconfig

# Install the new kernel
## Boot to the new kernel via grub
1. cp -iv arch/x86/boot/bzImage /boot/vmlinuz-X.XX # Kernel img
2. cp -iv System.map /boot/System.map-X.XX # Kernel symbol table
3. add a new entry in /etc/grub.d/01_ft_linux # Check 01_ft_linux
4. reboot and boot to the new kernel

# Turn in boot log
sudo dmesg > kern.log
===========================LINUX_NEXT============================
Linux next is a linux tree that merge all changes that will land in the next
kernel version.
The branch is automatically updated by a cron job.
