#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 [filename] [size-in-1kB-blocks]"
    exit 1
fi

dd if=/dev/zero bs=1024 count=$2 of=$1 && mkfs.ext2 -F -b 1024 -c $1 && \
    tune2fs -c0 -i0 $1 && mkdir -p mnt && sudo mount -o loop $1 mnt

echo

if [ $? -eq 0 ]; then
    echo "Mount completed successfully! Use 'sudo umount mnt' to unmount"
else
    echo "Mount failed...please try again."
fi
