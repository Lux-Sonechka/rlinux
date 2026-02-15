echo "Running system using Qemu..."
qemu-system-x86_64 -kernel kernel/vmlinuz-tty2 -append "root=/dev/sda console=tty1 init=/init video=1440x900" -hda rootfs.img -vga std -display sdl -m 512 -serial file:serial.log -device virtio-vga

