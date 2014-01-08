# Copyright 2013 Elliott Brossard and James Youngquist.
#
# This file is part of cse451-undelete.
#
# cse451-undelete is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# cse451-undelete is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with cse451-undelete.  If not, see <http:#www.gnu.org/licenses/>.

obj-m := ext2undelete.o
ext2undelete-y := undelete.o tar_utils.o 

# Set this path to wherever the kernel source is on your machine.
KDIR := ~/ta_cse451/linux-3.8.3-201.cse451custom.fc18.x86_64
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

