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

#include "nar.h"
#include "zlib_readers.h"

#include <stdlib.h>
#include <string.h>
#include <zlib.h>

typedef struct {
  FILE* input;

  int flush;
  z_stream strm;
} zlib_reader_state;

void* init_zlib_reader(struct nar_options const* opts)
{
  zlib_reader_state* zrs = NULL;
  int ret;

  if (opts->input != NULL) {
    zrs = malloc(sizeof(zlib_reader_state));
    if (zrs != NULL) {
      memset(zrs, 0, sizeof(zlib_reader_state));
      zrs->input = fopen(opts->input, "r");
      zrs->strm.zalloc = Z_NULL;
      zrs->strm.zfree = Z_NULL;
      zrs->strm.opaque = Z_NULL;

      zrs->flush = feof(zrs->input) ? Z_FINISH : Z_NO_FLUSH;

      ret = deflateInit(&zrs->strm, Z_DEFAULT_COMPRESSION);
      if (ret != Z_OK) {
        fclose(zrs->input);
        free(zrs);
        zrs = NULL;
        ERROR("unable to initialize deflate");
      }
      DPRINTF("initialization done");
    }
  }

  return zrs;
}

void close_zlib_reader(void* opaque)
{
  zlib_reader_state* zrs = NULL;
  zrs = opaque;

  if (zrs != NULL) {
    fclose(zrs->input);
    deflateEnd(&zrs->strm);
    free(zrs);
  }

  DPRINTF("reset done");
}

int zlib_reader(void* opaque, uint8_t* buf, uint32_t const max)
{
  zlib_reader_state* zrs = NULL;
  zrs = opaque;
  int have = 0;
  int ret;
  uint8_t* in = NULL;

  if (zrs == NULL) {
    DPRINTF("opaque(%p)", zrs);
    return -1;
  }

  in = malloc(max);
  if (in == NULL) {
    DPRINTF("in(%p)", in);
    return -1;
  }
  memset(in, 0, max);

  if (zrs->flush == Z_FINISH) {
    DPRINTF("finish");
    goto zlib_read_finish;
  }

  zrs->strm.avail_in = fread(in, sizeof(uint8_t), max, zrs->input);
  if (ferror(zrs->input)) {
    have = -1;
    DPRINTF("error on read");
    goto zlib_read_finish;
  }

  zrs->flush = feof(zrs->input) ? Z_FINISH : Z_NO_FLUSH;
  zrs->strm.next_in = in;

  zrs->strm.avail_out = max;
  zrs->strm.next_out = buf;
  ret = deflate(&zrs->strm, zrs->flush);

  switch (ret) {
  case Z_STREAM_ERROR:
    have = -1;
    break;
  case Z_STREAM_END:
  default:
    have = (max - zrs->strm.avail_out);
    break;
  }

zlib_read_finish:
  DPRINTF("buf(%s) have(%d)", buf, have);
  free(in);
  return have;
}

int zlib_size(void* opaque, uint64_t* size)
{
  zlib_reader_state* zrs = NULL;
  zrs = opaque;

  if (zrs == NULL || size == NULL) {
    DPRINTF("opaque(%p) size(%p)", zrs, size);
    return -1;
  }

  fseek(zrs->input, 0, SEEK_SET);
  *size = fseek(zrs->input, 0, SEEK_END);
  *size = ftell(zrs->input);
  fseek(zrs->input, 0, SEEK_SET);

  return 0;
}
