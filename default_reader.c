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
#include "nar.h"
#include "default_reader.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <errno.h>

#if defined(__APPLE__)
# define lseek64 lseek
#endif

void* init_default_reader(struct nar_options const* opts)
{
  int* fd = NULL;

  if (opts->input != NULL) {
    fd = malloc(sizeof(int));
    if (fd != NULL) {
      *fd = open(opts->input, O_RDONLY);
    }
  }

  return fd;
}

void close_default_reader(void* fd)
{
  if (fd != NULL) {
    close(*(int*)fd);
    free(fd);
  }
}

int default_reader(void* opaque, uint8_t* buf, uint32_t const max)
{
  int* fd;

  if (opaque == NULL) {
    DPRINTF("opaque(%p)", opaque);
    return -1;
  }

  fd = (int*)opaque;
  return read(*fd, buf, max);
}

int default_size(void* opaque, uint64_t* size)
{
  int* fd;

  if (opaque == NULL || size == NULL) {
    DPRINTF("opaque(%p) size(%p)", opaque, size);
    return -1;
  }

  fd = (int*)opaque;
  lseek64(*fd, 0, SEEK_SET);
  *size = lseek64(*fd, 0, SEEK_END);
  lseek64(*fd, 0, SEEK_SET);

  return 0;
}
