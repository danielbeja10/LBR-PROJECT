#!/bin/bash


echo "[RUN.SH] Launching QEMU..."

sudo qemu-system-x86_64 \
  -enable-kvm \
  -cpu host \
  -m 4096 \
  -smp 2 \
  -drive file="/mnt/c/Program Files/qemu/ubuntu_vm.qcow2",format=qcow2 \
  -virtfs local,path="/mnt/c/Users/User/Documents/Danielprojects/TPILPROJECT",mount_tag=hostshare,security_model=passthrough,id=hostshare \
  -display default,show-cursor=on \
  -net nic -net user
