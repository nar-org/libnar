/*
** Copyright (c) 2014, Nicolas DI PRIMA <nicolas@di-prima.fr>
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
** this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice,
** this list of conditions and the following disclaimer in the documentation
** and/or other materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its
** contributors may be used to endorse or promote products derived from this
** software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
*/

/* In order to use lseek64 (see man 3 lseek64) */
#define _LARGEFILE64_SOURCE
#include "libnar.h"

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

# if defined(DEBUG)
#  include <stdio.h>

#  define DPRINTF(fmt, ...)                                 \
   do {                                                     \
     fprintf(stderr, "[%s:%u][%s] " fmt "\n",               \
             __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
   } while (0)
# else
#  define DPRINTF(fmt, ...) \
   do {                     \
   } while (0)
# endif

#if defined(__APPLE__)
# define lseek64 lseek
#endif

#if defined(__BYTE_ORDER__)
# if   __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    /* LITTLE ENDIAN */
# elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#  error "this actual version of LIBNAR does not support BIG ENDIAN"
# elif __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#  error "this actual version of LIBNAR does not support PDP ENDIAN"
# else
#  error "the actual compiler may not support this implementation of LIBNAR"
# endif
#else /* !defined(__BYTE_ODER__) */
# if   defined(__i386__)    || defined(__alpha__)  \
       || defined(__x86_64) || defined(__x86_64__) \
       || defined(__ia64)   || defined(__ia64__)   \
       || defined(_M_IX86)  || defined(_M_IA64)    \
       || defined(_M_ALPHA) || defined(_WIN64)
    /* LITTLE ENDIAN */
# elif defined(__sparc)     || defined(__sparc__)   \
       || defined(_POWER)   || defined(__powerpc__) \
       || defined(__ppc__)  || defined(__hppa)      \
       || defined(_MIPSEB)  || defined(__s390__)
#  error "this actual version of LIBNAR does not support BIG ENDIAN"
# else /* no information provided by the compiler */
#  error "the actual compiler may not support this implementation of LIBNAR"
# endif
#endif

int libnar_init_writer(nar_writer* nar, int fd)
{
  if (nar == NULL) {
    DPRINTF("nar_write* nar == NULL");
    return -1;
  }

  memset(nar, 0, sizeof(nar_writer));

  nar->fd = fd;

  return 0;
}

void libnar_close_writer(nar_writer* nar)
{
  if (nar != NULL) {
    memset(nar, 0, sizeof(nar_writer));
  }
}

static int write_buffer(int fd, uint8_t const* buf, uint64_t const size)
{
  uint64_t offset;
  int ret;

  for (offset = 0; offset < size; offset += ret) {
    ret = write(fd, &buf[offset], size - offset);
    if (ret == -1) {
      DPRINTF("write errno(%d): %s", errno, strerror(errno));
      return -1;
    }
  }

  return offset;
}

int libnar_write_nar_header(nar_writer* nar,
                            uint64_t const cipher_type,
                            uint64_t const compression_type)
{
  nar_header nh;
  uint8_t* buf;
  uint32_t length;
  int ret;

  if (nar == NULL) {
    DPRINTF("nar_write* nar == NULL");
    return -1;
  }

  length = sizeof(nar_header);

  memset(&nh, 0, length);
  memcpy(&(nh.magic), NAR_HEADER_MAGIC, sizeof(uint64_t));
  nh.version.major = NAR_HEADER_VERSION_MAJOR;
  nh.version.minor = NAR_HEADER_VERSION_MINOR;
  nh.cipher_type = cipher_type;
  nh.compression_type = compression_type;
  nh.signature_position = nar->signature_position;
  nh.index_position = nar->index_position;

  if (-1 == lseek64(nar->fd, 0, SEEK_SET)) {
    switch (errno) {
    case ESPIPE:
      /* The user is probably using a pipe or a socked or a FIFO,
      ** then do not consider it as an error */
      break;
    default:
      DPRINTF("lseek/ error: %s", strerror(errno));
      return -errno;
      break;
    }
  }

  buf = (uint8_t*)&nh;
  ret = write_buffer(nar->fd, buf, length);
  if (ret == -1) {
    DPRINTF("write errno(%d): %s", errno, strerror(errno));
    return -errno;
  }

  return 0;
}

static int write_missing_0(int fd, uint64_t size)
{
  uint8_t tmp[64];

  memset(tmp, 0, sizeof(tmp));

  return write_buffer(fd, tmp, size % sizeof(uint64_t));
}


int libnar_append_file(nar_writer* nar, uint64_t const flags,
                       char const* filepath, uint64_t const length_filepath,
                       uint64_t const length_content,
                       get_computed_content callback, void* opaque)
{
  item_header pfh;
  uint8_t* buf;
  uint32_t length;
  uint32_t offset;
  int ret;

  if (nar == NULL || filepath == NULL) {
    DPRINTF("nar(%p) filepath(%p)", nar, filepath);
    return -1;
  }

  if (-1 == lseek64(nar->fd, 0, SEEK_END)) {
    switch (errno) {
    case ESPIPE:
      /* The user is probably using a pipe or a socked or a FIFO,
      ** then do not consider it as an error */
      break;
    default:
      DPRINTF("lseek error: %s", strerror(errno));
      return -errno;
      break;
    }
  }

  length = sizeof(item_header);
  memset(&pfh, 0, length);
  memcpy(&pfh.magic, FILE_HEADER_MAGIC, sizeof(uint64_t));
  pfh.flags = flags;
  pfh.length1 = length_filepath;
  pfh.length2 = length_content;

  buf = (uint8_t*)&pfh;
  ret = write_buffer(nar->fd, buf, length);
  if (ret == -1) {
    DPRINTF("write errno(%d): %s", errno, strerror(errno));
    return -errno;
  }

  buf = (uint8_t*)filepath;
  length = length_filepath;
  ret = write_buffer(nar->fd, buf, length);
  if (ret == -1) {
    DPRINTF("write errno(%d): %s", errno, strerror(errno));
    return -errno;
  }

  if (length % sizeof(uint64_t)) {
    ret = write_missing_0(nar->fd, sizeof(uint64_t) - (length % sizeof(uint64_t)));
    if (ret == -1) {
      DPRINTF("write errno(%d): %s", errno, strerror(errno));
      return -errno;
    }
  }

  for (offset = 0;
       offset != length_content;
       offset += ret) {
    uint8_t tmp[256];

    ret = callback(opaque, tmp, sizeof(tmp));
    if (ret == -1) {
      DPRINTF("callback errno(%d): %s", errno, strerror(errno));
      return -errno;
    }
    if (ret == 0) {
      break;
    }

    length = ret;
    ret = write_buffer(nar->fd, tmp, length);
    if (ret == -1) {
      DPRINTF("write errno(%d): %s", errno, strerror(errno));
      return -errno;
    }
  }

  if (offset % sizeof(uint64_t)) {
    ret = write_missing_0(nar->fd, sizeof(uint64_t) - (offset % sizeof(uint64_t)));
    if (ret < 0) {
      DPRINTF("write errno(%d): %s", errno, strerror(errno));
      return ret;
    }
  }

  return 0;
}

int libnar_init_reader(nar_reader* nar, int fd)
{
  if (nar == NULL) {
    DPRINTF("nar(%p)", nar);
    return -1;
  }

  memset(nar, 0, sizeof(nar_reader));
  nar->fd = fd;

  return 0;
}

void libnar_close_reader(nar_reader* nar)
{
  if (nar != NULL) {
    memset(nar, 0, sizeof(nar_reader));
  }
}

int libnar_read_nar_header(nar_reader* nar, nar_header* nh)
{
  uint8_t buf[64];
  int ret;
  int i, l;

  if (nar == NULL || nh == NULL || nar->fd == -1) {
    DPRINTF("nar_reader(%p) nar_header(%p) fd(%d)",
            nar, nh, (nar) ? nar->fd : -1);
    return -1;
  }

  if (-1 == lseek64(nar->fd, 0, SEEK_SET)) {
    switch (errno) {
    case ESPIPE:
      /* The user is probably using a pipe or a socked or a FIFO,
      ** then do not consider it as an error */
      break;
    default:
      DPRINTF("lseek errno(%d): %s", errno, strerror(errno));
      return -errno;
      break;
    }
  }

  l = sizeof(buf);
  for (i = 0; i < l; i+=ret) {
    ret = read(nar->fd, &buf[i], l - i);
    if (ret == -1) {
      DPRINTF("read errno(%d): %s", errno, strerror(errno));
      return -errno;
    }
  }

  memcpy(nh, buf, sizeof(nar_header));
  nar->item_offset = 0;
  nar->item_offset_content1 = 0;
  nar->item_offset_content2 = 0;

  return 0;
}

int libnar_read_item_header(nar_reader* nar, item_header* ih)
{
  uint8_t buf[sizeof(item_header)];
  int ret;
  int i, l;

  if (nar == NULL || ih == NULL || nar->fd == -1) {
    DPRINTF("nar_reader(%p) item_header(%p) fd(%d)",
            nar, ih, (nar) ? nar->fd : -1);
    return -1;
  }

  nar->item_offset = 0;
  nar->item_offset_content1 = 0;
  nar->item_offset_content2 = 0;

  l = sizeof(buf);
  for (i = 0; i < l; i+=ret) {
    ret = read(nar->fd, &buf[i], l - i);
    if (ret == -1) {
      DPRINTF("read errno(%d): %s", errno, strerror(errno));
      return -errno;
    }

    nar->item_offset += ret;
    if (ret == 0) {
      return -1;
    }
  }

  memcpy(ih, buf, sizeof(item_header));

  return 0;
}

int libnar_read_content1(nar_reader* nar, item_header const* ih,
                         char* buf, uint32_t const max)
{
  int ret = 0;
  uint64_t i;
  uint64_t length;

  if (nar == NULL || ih == NULL || nar->fd == -1 || buf == NULL) {
    DPRINTF("nar_reader(%p) item_header(%p) fd(%d) buf(%p)",
            nar, ih, (nar) ? nar->fd : -1, buf);
    return -1;
  }

  if (!ih->length1) {
    return 0;
  }

  length = ih->length1 - nar->item_offset_content1;

  length = (max > length) ? length : max;
  for (i = 0; i < length; i+=ret) {
    ret = read(nar->fd, &buf[i], length - i);
    if (ret == -1) {
      DPRINTF("read errno(%d): %s", errno, strerror(errno));
      return -errno;
    }

    nar->item_offset += ret;
    nar->item_offset_content1 += ret;
    if (ret == 0) {
      break;
    }
  }

  return i;
}

int libnar_read_content2(nar_reader* nar, item_header const* ih,
                         char* buf, uint32_t const max)
{
  int ret = 0;
  uint64_t i;
  uint64_t length;

  if (nar == NULL || ih == NULL || nar->fd == -1 || buf == NULL) {
    DPRINTF("nar_reader(%p) item_header(%p) fd(%d) buf(%p)",
            nar, ih, (nar) ? nar->fd : -1, buf);
    return -1;
  }

  if (ROUNDUP64(ih->length1) > nar->item_offset_content1) {
    lseek64(nar->fd,
            ROUNDUP64(ih->length1) - nar->item_offset_content1,
            SEEK_CUR);
    nar->item_offset_content1 = ROUNDUP64(ih->length1);
  }

  length = ih->length2 - nar->item_offset_content2;

  length = (max > length) ? length : max;
  for (i = 0; i < length; i+=ret) {
    ret = read(nar->fd, &buf[i], length - i);
    if (ret == -1) {
      DPRINTF("read errno(%d): %s", errno, strerror(errno));
      return -errno;
    }

    nar->item_offset += ret;
    nar->item_offset_content2 += ret;
    if (ret == 0) {
      break;
    }
  }

  return i;
}

int libnar_jump_to_next_item_header(nar_reader* nar, item_header const* ih)
{
  uint64_t offset = 0;

  if (nar == NULL || nar->fd == -1) {
    DPRINTF("nar_reader(%p) fd(%d)",
            nar, (nar) ? nar->fd : -1);
    return -1;
  }

  if (ih == NULL) {
    return 0;
  }

  offset = sizeof(item_header)
         + (ROUNDUP64(ih->length1)) + (ROUNDUP64(ih->length2))
         - nar->item_offset;

  if (-1 == lseek64(nar->fd, offset, SEEK_CUR)) {
    switch (errno) {
    case ESPIPE:
      /* in this case, we shouldn't want to jump, because if we drop the
      ** packets received from the socked (or pipe...) we lost the information */
      DPRINTF("TODO: lseek errno(%d): %s", errno, strerror(errno));
      return -1;
      break;
    default:
      DPRINTF("lsekk errno(%d): %s", errno, strerror(errno));
      return -1;
      break;
    }
  }

  return 0;
}
