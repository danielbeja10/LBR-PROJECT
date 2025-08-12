#!/bin/sh

echo "[VM_INIT] Mounting host share..."
mkdir -p /mnt/host
mount -t 9p -o trans=virtio hostshare /mnt/host

echo "[VM_INIT] Inserting driver..."
insmod /mnt/host/lbr_driver.ko

echo "[VM_INIT] Done. Last kernel logs:"
dmesg | tail -n 30
