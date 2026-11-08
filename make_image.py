#!/usr/bin/env python3

'''
Following instructions will create a disk image with following disk geometry parameters:
* size in megabytes = 64
* amount of cylinders = 130
* amount of headers = 16
* amount of sectors per track = 63

The image is written directly in Python (no dd, parted or fdisk), so it can be
built the same way on Linux, macOS and Windows (WSL).
'''

import os
import struct
import sys

class color:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def info(c, s):
    print(c + s + color.ENDC)


def panic(s):
    print(color.BOLD + color.FAIL + s + color.ENDC)
    sys.exit(1)

SECTOR = 512
TOTAL_SECTORS = 130 * 16 * 63
PART_START = 2048                       # first sector of partition 1 (the kernel)
PART_SECTORS = TOTAL_SECTORS - PART_START
DISK_SIGNATURE = 0x07a591aa

def read(path, limit=None):
    try:
        with open(path, 'rb') as f:
            data = f.read()
    except OSError as e:
        panic('cannot read %s: %s' % (path, e))
    if limit is not None and len(data) > limit:
        panic('%s is too large: %d bytes (max %d)' % (path, len(data), limit))
    return data

def partition_entry():
    # Bootable, type 0x83, CHS fields as written by parted for this geometry,
    # LBA start and size.
    return (bytes([0x80, 0x00, 0x01, 0x10, 0x83, 0x03, 0xe0, 0xff]) +
            struct.pack('<II', PART_START, PART_SECTORS))

info (color.HEADER, 'Building Certikos Image...')

info (color.HEADER, '\ncreating disk...')
boot0 = read('obj/boot/boot0', 446)
boot1 = read('obj/boot/boot1', 62 * SECTOR)
kernel = read('obj/kern/kernel', PART_SECTORS * SECTOR)

mbr = bytearray(SECTOR)
mbr[0:len(boot0)] = boot0
struct.pack_into('<I', mbr, 440, DISK_SIGNATURE)
mbr[446:462] = partition_entry()
mbr[510:512] = b'\x55\xaa'
info (color.OKGREEN, 'done.')

info (color.HEADER,  '\nwriting mbr...')
with open('certikos.img', 'wb') as img:
    img.truncate(TOTAL_SECTORS * SECTOR)
    img.write(mbr)
    img.seek(SECTOR)
    img.write(boot1)
    info (color.OKGREEN, 'done.')

    info (color.HEADER,  '\ncopying kernel files...')
    info (color.OKBLUE, 'kernel starts at sector %d' % PART_START)
    img.seek(PART_START * SECTOR)
    img.write(kernel)

info (color.OKGREEN + color.BOLD, '\nAll done.')
sys.exit(0)
