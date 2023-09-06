# Build Toolchain

Maybe you could use the [xpack binary](https://xpack.github.io/tags/riscv/) (NOT TESTED) if you do not want to compile the riscv gcc manually.

# Build Linux Kernel
## Configure
Run `ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- make defconfig` to generate a default configuration.

Run`ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- make menuconfig`, enable the following configurations: 
(Note: Some entity would only appear after some other entities are selected. Please follow the operation sequence below) 
- `Kernel features -> SBI v0.1 support`
- `Device drivers -> Character devices -> RISC-V SBI console support`
- `Boot options -> Built-in kernel command line`, enter `earlycon=sbi`
- `Boot options -> Built-in command line usage -> Extend bootloader kernel arguments`
## Compile
Run `ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- make` to compile the kernel. 


# Build Opensbi

Make sure that you have added the path to the gcc executables to your `PATH` env.

Use the following command to build OpenSBI:
```bash
make CROSS_COMPILE=riscv64-unknown-linux-gnu- PLATFORM=generic PLATFORM_RISCV_ISA=rv64ima_zicsr_zifencei FW_FDT_PATH=cemu.dtb FW_PAYLOAD_PATH=<src_path_to_linux_kernel>/arch/riscv/boot/Image
```
You could omit the `FW_PAYLOAD_PATH` env if you just want to build a pure opensbi.



# Build Busybox

# Build rootfs

# Run with QEMU