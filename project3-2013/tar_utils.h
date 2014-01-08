// Copyright 2013 Elliott Brossard and James Youngquist.
//
// This file is part of cse451-undelete.
//
// cse451-undelete is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// cse451-undelete is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with cse451-undelete.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _TAR_UTILS_H
#define _TAR_UTILS_H

#include <linux/types.h>

#define TAR_BLOCKSIZE 512

struct tar_header {
                                /* byte offset */
  char name[100];               /*   0 */
  char mode[8];                 /* 100 */
  char uid[8];                  /* 108 */
  char gid[8];                  /* 116 */
  char size[12];                /* 124 */
  char mtime[12];               /* 136 */
  char chksum[8];               /* 148 */
  char typeflag;                /* 156 */
  char linkname[100];           /* 157 */
  char magic[6];                /* 257 */
  char version[2];              /* 263 */
  char uname[32];               /* 265 */
  char gname[32];               /* 297 */
  char devmajor[8];             /* 329 */
  char devminor[8];             /* 337 */
  char prefix[155];             /* 345 */
                                /* 500 */
};

struct file_properties {
  char name[100];
  int mode;
  int uid;
  int gid;
  size_t size;
  int mtime;
  char uname[32];
  char gname[32];
};

// Write a tar header for the file with the given properties
// the given offset within the given buffer. Returns 0 on
// success, non-zero otherwise.
extern int tar_write_header(
    const struct file_properties* file_properties,
    char* dst_buffer, size_t dst_buffer_len,
    size_t dst_buffer_offset);

#endif
