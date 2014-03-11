/*
** libnar.c for libnar
**
** Made by Nicolas DI PRIMA
** Login   nicolas <nicolas@di-prima.fr>
**
** Started on  Mon 10 Mar 2014 11:19:39 GMT Nicolas DI PRIMA
** Last update Tue 11 Mar 2014 19:10:40 GMT Nicolas DI PRIMA
*/

#include "libnar.h"

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int init_nar_writer(nar_writer* nar, int fd)
{
  if (nar == NULL) {
    DPRINTF("nar_write* nar == NULL");
    return -1;
  }

  memset(nar, 0, sizeof(nar_writer));

  nar->fd = fd;

  return 0;
}

void close_nar_writer(nar_writer* nar)
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

int write_nar_header(nar_writer* nar,
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

  /* TODO: use lseek64 instead */
  if (-1 == lseek(nar->fd, 0, SEEK_SET)) {
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


int append_file(nar_writer* nar, uint64_t const flags,
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

  /* TODO: use lseek64 instead */
  if (-1 == lseek(nar->fd, 0, SEEK_END)) {
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

int init_nar_reader(nar_reader* nar, int fd)
{
  if (nar == NULL) {
    DPRINTF("nar(%p)", nar);
    return -1;
  }

  memset(nar, 0, sizeof(nar_reader));
  nar->fd = fd;

  return 0;
}

void close_nar_reader(nar_reader* nar)
{
  if (nar != NULL) {
    memset(nar, 0, sizeof(nar_reader));
  }
}

int read_nar_header(nar_reader* nar, nar_header* nh)
{
  uint8_t buf[64];
  int ret;
  int i, l;

  if (nar == NULL || nh == NULL || nar->fd == -1) {
    DPRINTF("nar_reader(%p) nar_header(%p) fd(%d)",
            nar, nh, (nar) ? nar->fd : -1);
    return -1;
  }

  /* TODO: use lseek64 instead */
  if (-1 == lseek(nar->fd, 0, SEEK_SET)) {
    switch (errno) {
    case ESPIPE:
      /* The user is probably using a pipe or a socked or a FIFO,
      ** then do not consider it as an error */
      break;
    default:
      DPRINTF("lseek errno(%d): %s", errno, strerror(errno));
      return -1;
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

  return 0;
}

int read_item_header(nar_reader* nar, item_header* ih)
{
  uint8_t buf[32];
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

int read_content1(nar_reader* nar, item_header const* ih,
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

  length = ih->length1 - nar->item_offset_content1;

  length = (max > length) ? length : max;
  DPRINTF("length: %llu", length);
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

int read_content2(nar_reader* nar, item_header const* ih,
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

  if (ALIGN64(ih->length1) > nar->item_offset_content1) {
    lseek(nar->fd, ALIGN64(ih->length1) - nar->item_offset_content1, SEEK_CUR);
    nar->item_offset_content1 = ALIGN64(ih->length1);
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

int jump_to_next_item_header(nar_reader* nar, item_header* ih)
{
  uint64_t offset;

  if (nar == NULL || ih == NULL || nar->fd == -1) {
    DPRINTF("nar_reader(%p) item_header(%p) fd(%d)",
            nar, ih, (nar) ? nar->fd : -1);
    return -1;
  }

  offset = sizeof(item_header)
         + (ALIGN64(ih->length1)) + (ALIGN64(ih->length2))
         - nar->item_offset;

  /* TODO: use lseek64 instead */
  if (-1 == lseek(nar->fd, offset, SEEK_CUR)) {
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
