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

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/string.h>

#include "tar_utils.h"

#define TMAGIC   "ustar"        /* ustar and a null */
#define TMAGLEN  6
#define TVERSION "  "           /*    and no null */
#define TVERSLEN 2

#define REGTYPE  '0'            /* regular file */

#define int_to_oct(num, oct)                                    \
  snprintf((oct), (int) sizeof(oct), "%0*lo",                   \
           (int) sizeof(oct) - 1, (unsigned long) (num))

// Each file requires a header plus space for its data blocks.
#define tar_blocks_for_length(length)               \
  (1 + (length) / TAR_BLOCKSIZE +                   \
   ((length) % TAR_BLOCKSIZE != 0 ? 1 : 0))

#define tar_size_for_length(length) \
  (tar_blocks_for_length(length) * TAR_BLOCKSIZE)

int tar_write_header(
    const struct file_properties* file_properties,
    char* dst_buffer, size_t dst_buffer_len,
    size_t dst_buffer_offset) {
  struct tar_header* tar_header;
  int i, sum = 0;

  // Check that the destination buffer is sufficiently large.
  if (dst_buffer_offset + TAR_BLOCKSIZE > dst_buffer_len) {
    return -EINVAL;
  }

  tar_header = (struct tar_header*) &dst_buffer[dst_buffer_offset];
  memset(tar_header, 0, sizeof(*tar_header));

  // Write the file properties to the header.
  strncpy(tar_header->name, file_properties->name, sizeof(tar_header->name));
  int_to_oct(file_properties->mode, tar_header->mode);
  int_to_oct(file_properties->uid, tar_header->uid);
  int_to_oct(file_properties->gid, tar_header->gid);
  int_to_oct(file_properties->size, tar_header->size);
  int_to_oct(file_properties->mtime, tar_header->mtime);
  tar_header->typeflag = REGTYPE;
  strncpy(tar_header->magic, TMAGIC, sizeof(tar_header->magic));
  strncpy(tar_header->version, TVERSION, sizeof(tar_header->version));
  strncpy(tar_header->uname, file_properties->uname, sizeof(tar_header->uname));
  strncpy(tar_header->gname, file_properties->gname, sizeof(tar_header->gname));

  // Compute the checksum.
  for (i = 0; i < sizeof(*tar_header); ++i) {
    sum += ((char*) (tar_header))[i];
  }
  // Do not include the checksum bytes themselves.
  for (i = 0; i < sizeof(tar_header->chksum); ++i) {
    sum += ' ' - tar_header->chksum[i];
  }
  int_to_oct(sum, tar_header->chksum);

  return 0;
}
