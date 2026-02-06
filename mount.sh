echo "Creating rootfs.img"
rm rootfs.img
sudo rm -rf ./mnt/*
dd if=/dev/zero of=rootfs.img bs=1M count=1024
mkfs.ext4 rootfs.img
echo echo "Mounting rootfs.img on /mnt"
sudo mount rootfs.img ./mnt
echo "Copying all files from /rootfs to /mnt"
sudo cp -r rootfs/* ./mnt
echo "Done! Umounting /mnt."
sudo umount ./mnt
