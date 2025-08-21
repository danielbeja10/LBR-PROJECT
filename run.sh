#!/bin/bash
set -e

# קבצים בתיקייה שלך:
IMG="$PWD/ubuntu_vm.qcow2"
ISO="$PWD/ubuntu-24.04.2-desktop-amd64.iso"   # ודא שזה שם הקובץ המדויק עם .iso
RAM=4096

# אם עוד אין דיסק – ניצור
if [ ! -f "$IMG" ]; then
  echo "[INIT] Creating qcow2 disk: $IMG (20G)"
  qemu-img create -f qcow2 "$IMG" 20G
fi

echo "[QEMU] Booting installer/live (TCG, no KVM)..."
exec qemu-system-x86_64 \
  -accel tcg \
  -cpu max \
  -smp 4 \
  -m "$RAM" \
  -drive file="$IMG",format=qcow2,if=virtio \
  -cdrom "$ISO" -boot order=d \
  -device virtio-net-pci \
  -nic user,model=virtio \
  -virtfs local,path="$PWD",mount_tag=hostshare,security_model=passthrough,id=hostshare \
  -display sdl
