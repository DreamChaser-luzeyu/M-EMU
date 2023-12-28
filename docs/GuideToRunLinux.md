# Build Toolchain

```bash
./configure --prefix=/opt/riscv --with-arch=rv64imac --with-abi=lp64
```


# Build Linux Kernel
## Configure
Run `ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- make defconfig` to generate a default configuration.

Run`ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- make menuconfig`, enable the following configurations: 
(Note: Some entity would only appear after some other entities are selected. Please follow the operation sequence below) 
- `Kernel features -> SBI v0.1 support`
- `Device drivers -> Character devices -> RISC-V SBI console support`
- `Boot options -> Built-in kernel command line`, enter `earlycon=sbi`
- `CONFIG_SERIAL_UARTLITE_CONSOLE=y`
- `Boot options -> Built-in command line usage -> Extend bootloader kernel arguments`
- `General setup -> Initial RAM filesystem and RAM disk`, set `../busybox_rootfs.gz`
## Compile
Run `ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- make` to compile the kernel. 


# Build Opensbi

Make sure that you have added the path to the gcc executables to your `PATH` env.

Use the following command to build OpenSBI:
```bash
make \
CROSS_COMPILE=riscv64-unknown-linux-gnu- \
PLATFORM=generic \
PLATFORM_RISCV_ISA=rv64ima_zicsr_zifencei \
FW_FDT_PATH=cemu.dtb \
FW_PAYLOAD_PATH=path_to_linux_kernel_src/arch/riscv/boot/Image
```
You could omit the `FW_PAYLOAD_PATH` env if you just want to build a pure opensbi.

<!-- 
In my case ```bash
make \
CROSS_COMPILE=riscv64-unknown-linux-gnu- \
PLATFORM=generic \
PLATFORM_RISCV_ISA=rv64ima_zicsr_zifencei \
FW_FDT_PATH=../platform_dtb.dtb \
FW_PAYLOAD_PATH=../linux-6.5.8/arch/riscv/boot/Image 
```
 -->

# Build Busybox

```bash
ARCH=riscv64 CROSS_COMPILE=riscv-none-elf- make menuconfig
```

- `Settings -> Support --long options -> Don't use /usr`
- `Settings -> Build Options -> Build static binary`


Note: You should use a compiler with glib supported. An embedded compiler would throw error like header not found.
```bash
CROSS_COMPILE=riscv64-unknown-linux-gnu- ARCH=riscv64 make

CROSS_COMPILE=riscv64-unknown-linux-gnu- ARCH=riscv64 make install
```
# Build rootfs
## Copy busybox files
```
mkdir ../initramfs
cp -r ./_install/* ../initramfs/

cd ../initramfs
```

The structure of initramfs directory should be like this:
```
├── bin
├── linuxrc -> bin/busybox
└── sbin
```
## Write init file
Create a file named `init` at initramfs root directory. Fill your init script.

Example init script:
```bash
#!/bin/bash

echo "Init running..."

export PATH="/bin:/sbin"

# Check and create directory
[ -d /dev  ] || mkdir -m 0755 /dev
[ -d /root ] || mkdir --mode=0700 /root
[ -d /sys  ] || mkdir /sys
[ -d /proc ] || mkdir /proc
[ -d /tmp  ] || mkdir /tmp
[ -d /mnt  ] || mkdir /mnt

# Mount /proc and /sys:
mount -n proc /proc -t proc
mount -n sysfs /sys -t sysfs

# Mount devfs
# Only needed when udev scripts are used
#mount -t tmpfs -o mode=0755 udev /dev

# Create basic character device nodes
[ -e /dev/console ] || mknod /dev/console c 5 1
[ -e /dev/null ] || mknod /dev/null c 1 3

# Specify path to hotplug script
echo /sbin/mdev >/proc/sys/kernel/hotplug

# Run mdev, do device scanning and create dev nodes accordingly
mdev -s

# Run terminal
/bin/sh -i

```
Do not forget to run `chmod +x ./init` to grant execute permission.


Also, for simplicity, you could use `ln -s ./bin/busybox init` to run busybox directly.


# Run with QEMU