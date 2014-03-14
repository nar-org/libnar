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
#include "nar.h"
#include "default_reader.h"
#include "zlib_readers.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>

static char short_options[] = "cla:n:e:ht:T:eEC";

static struct option long_options[] = {
  {"create",   no_argument,       NULL, 'c'},
  {"append",   required_argument, NULL, 'a'},
  {"compress", no_argument,       NULL, 'C'},
  {"encrypt",  no_argument,       NULL, 'E'},
  {"list",     no_argument,       NULL, 'l'},
  {"narfile",  required_argument, NULL, 'n'},
  {"help",     no_argument,       NULL, 'h'},
  {"extract",  required_argument, NULL, 'e'},

  {"compression-type", required_argument, NULL, 't'},
  {"cipher-type",      required_argument, NULL, 'T'},
  {NULL, 0, NULL, 0}
};

static struct compression_driver {
  char const* name;
  void* opaque;
  get_computed_content callback;
  int   (*size) (void*  opaque, uint64_t* size);
  void* (*init) (struct nar_options const* nar);
  void  (*close)(void*  opaque);
} compression_drivers[COMPRESSION_TYPE_LENGTH + 1] = {
  { .name = "none"
  , .opaque = NULL
  , .callback = default_reader
  , .size = default_size
  , .init = init_default_reader
  , .close = close_default_reader },
  { .name = "deflate"
  , .opaque = NULL
  , .callback = zlib_reader
  , .size = zlib_size
  , .init = init_zlib_reader
  , .close = close_zlib_reader },

  { .name = "help"
  , .opaque = NULL, .callback = NULL, .init = NULL, .close = NULL}
};

# define IS_COMPRESSION_SUPPORTED(type)         \
  (  type < COMPRESSION_TYPE_LENGTH             \
  && compression_drivers[type].callback != NULL \
  && compression_drivers[type].init     != NULL \
  && compression_drivers[type].close    != NULL \
  && compression_drivers[type].size     != NULL )

static nar_compression_type to_compression_type(char const* type)
{
  nar_compression_type ret = COMPRESSION_NONE;

  if (type != NULL) {
    for (ret = 0; ret <= COMPRESSION_TYPE_LENGTH; ret++) {
      if (!strncmp(type,
                   compression_drivers[ret].name,
                   strlen(type))) {
        break;
      }
    }
  }

  return ret;
}

# define LICENCE_MESSAGE                                       \
"Copyright (c) 2014, Nicolas DI PRIMA <nicolas@di-prima.fr>\n" \
"this implementation of nar comes without any warranty\n"

static void show_help_message(char const* name)
{
  PRINTF("%s: a C version of the NAR archiver\n"
         "Usage: %s --narfile|-n <filename> [option]\n"
         "\n"
	 LICENCE_MESSAGE
         "\n"
         "Options:\n"
         "    --help|-h\n"
         "                        show this help message\n"
         "    --narfile=<file>|-n <file>\n"
         "                        specify a narfile\n"
         "    --create|-c create\n"
         "                        create the file specified in the option --narfile\n"
         "    --append=<path>|-a <path>\n"
         "                        append the file or directory pointed by the <path> in\n"
         "                        the narfile specified in the option --narfile\n"
         "    --list|-l\n"
         "                        list the content of the archive given by option\n"
         "                        --narfile\n"
         "    --extract=<path>|-e <path>\n"
         "                        extract the item file named (path) from the given\n"
         "                        narfile specified in the option --narfile",
         name, name);
}

static int main_append_file(struct nar_options const* opts)
{
  nar_writer nw;
  nar_reader nr;
  nar_header nh;
  struct compression_driver* cd = compression_drivers; /* Set to default */
  int ofd;
  uint64_t length;
  uint64_t flags = 0;
  int ret = 0;

  if (opts == NULL || opts->output == NULL || opts->input == NULL) {
    DPRINTF("opts(%p) opts->output(%p) opts->input(%p)",
            opts, (opts) ? opts->output : NULL, (opts) ? opts->output : NULL);
    return -1;
  }

  ofd = open(opts->output, O_RDWR);
  if (ofd == -1) {
    ERROR("open(%s) errno(%d): %s",
          opts->output, errno, strerror(errno));
    return -1;
  }

  memset(&nh, 0, sizeof(nar_header));

  if (opts->compress) {
    libnar_init_reader(&nr, ofd);
    libnar_read_nar_header(&nr, &nh);
    libnar_close_reader(&nr);

    if (!IS_COMPRESSION_SUPPORTED(nh.compression_type)) {
      ERROR("compression type not supported %llu", nh.compression_type);
      goto exit_close_output;
    }

    cd = &compression_drivers[nh.compression_type];
    flags |= FILE_COMPRESSED;
  }

  ret = libnar_init_writer(&nw, ofd);
  if (ret) {
    ERROR("init_nar_writer(%s) errno(%d): %s",
          opts->output, -ret, strerror(-ret));
    goto exit_close_output;
  }

  cd->opaque = cd->init(opts);
  if (cd->opaque == NULL) {
    ERROR("can't initialize the compression_driver: %s", cd->name);
    ret = -1;
    goto exit_close_output;
  }

  ret = cd->size(cd->opaque, &length);
  DPRINTF("size: ret(%d) length(%llu)", ret, length);
  ret = libnar_append_file(&nw, flags, opts->input, strlen(opts->input),
                           length, cd->callback, cd->opaque);
  if (ret != 0) {
    ERROR("append(%s) errno(%d): %s",
          opts->input, -ret, strerror(-ret));
    goto exit_close_input;
  }

exit_close_input:
  cd->close(cd->opaque);
exit_close_output:
  libnar_close_writer(&nw);
  close(ofd);
  return ret;
}

static int main_create_nar_file(struct nar_options const* opts)
{
  nar_writer nw;
  int fd;
  int ret = 0;

  if (opts == NULL || opts->output == NULL) {
    DPRINTF("opts(%p) opts->output(%p)", opts, (opts) ? opts->output : NULL);
    return -1;
  }

  fd = creat(opts->output, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    ERROR("create(%s) errno(%d): %s",
          opts->output, errno, strerror(errno));
    return -1;
  }

  ret = libnar_init_writer(&nw, fd);
  if (ret) {
    ERROR("init_nar_writer(%s) errno(%d): %s",
          opts->output, -ret, strerror(-ret));
    goto exit_function;
  }

  /* TODO: handle options */
  ret = libnar_write_nar_header(&nw, 0, opts->compression_type);
  if (ret != 0) {
    ERROR("write_nar_header(%s) errno(%d): %s",
          opts->output, -ret, strerror(-ret));
    goto exit_function;
  }

exit_function:
  libnar_close_writer(&nw);
  close(fd);
  return ret;
}

static void dump_nar_header(nar_header* nh)
{
  if (nh != NULL) {
    char magic[9];

    memcpy(magic, &nh->magic, sizeof(uint64_t));
    magic[8] = '\0';

    PRINTF("magic: %s", magic);
    PRINTF("version: %u.%u", nh->version.major, nh->version.minor);
    PRINTF("cipher_type(0x%016llx) compression_type(0x%016llx)",
           nh->cipher_type, nh->compression_type);
    PRINTF("signature_position(0x%016llx) index_position(0x%016llx)",
           nh->signature_position, nh->index_position);
    PRINTF("unused[0](0x%016llx) unused[1](0x%016llx)",
           nh->unused[0], nh->unused[1]);
  }
}

static void dump_item_header(item_header* ih)
{
  if (ih != NULL) {
    char magic[9];

    memcpy(magic, &ih->magic, sizeof(uint64_t));
    magic[8] = '\0';

    PRINTF("magic: %s", magic);
    PRINTF("flags: 0x%016llx", ih->flags);
    if (IS_EXECUTABLE(ih->flags)) {
      PRINTF("  executable");
    }
    if (IS_COMPRESSED(ih->flags)) {
      PRINTF("  compressed");
    }
    if (IS_ENCRYPTED(ih->flags)) {
      PRINTF("  encrypted");
    }
    PRINTF("length1: 0x%016llx", ih->length1);
    PRINTF("length2: 0x%016llx", ih->length2);
  }
}

static int main_list_nar_file(struct nar_options const* opts)
{
  char magic[9];
  int ret = 0;
  nar_header nh;
  item_header ih;
  nar_reader nr;
  int fd;

  if (opts == NULL || opts->output == NULL) {
    DPRINTF("opts(%p) opts->output(%p)", opts, (opts) ? opts->output : NULL);
    return -1;
  }

  fd = open(opts->output, O_RDONLY);
  if (fd == -1) {
    DPRINTF("open errno(%d): %s", errno, strerror(errno));
    return -1;
  }

  ret = libnar_init_reader(&nr, fd);

  libnar_read_nar_header(&nr, &nh);
  dump_nar_header(&nh);

  while(libnar_read_item_header(&nr, &ih) == 0) {
    memcpy(magic, &ih.magic, sizeof(uint64_t));
    magic[8] = '\0';

    dump_item_header(&ih);
    if (!strncmp(magic, FILE_HEADER_MAGIC, sizeof(uint64_t))) {
      char filename[256];
      int size;

      memset(filename, 0, sizeof(filename));
      size = libnar_read_content1(&nr, &ih, filename, sizeof(filename));
      if (size >= 0) {
        PRINTF("filename(%d): %s", size, filename);
      } else {
        ERROR("can't read the filename: errno(%d): %s", -size, strerror(-size));
      }
    }
    libnar_jump_to_next_item_header(&nr, &ih);
  }

  libnar_close_reader(&nr);

  close(fd);

  return ret;
}

static int main_extract_nar_file(struct nar_options const* opts)
{
  char magic[9];
  int ret = 0;
  nar_header nh;
  item_header ih;
  nar_reader nr;
  int fd;

  if (opts == NULL || opts->output == NULL) {
    DPRINTF("opts(%p) opts->output(%p)", opts, (opts) ? opts->output : NULL);
    return -1;
  }

  fd = open(opts->output, O_RDONLY);
  if (fd == -1) {
    DPRINTF("open errno(%d): %s", errno, strerror(errno));
    return -1;
  }

  ret = libnar_init_reader(&nr, fd);

  libnar_read_nar_header(&nr, &nh);

  while(libnar_read_item_header(&nr, &ih) == 0) {
    memcpy(magic, &ih.magic, sizeof(uint64_t));
    magic[8] = '\0';

    if (!strncmp(magic, FILE_HEADER_MAGIC, sizeof(uint64_t))) {
      char filename[256];
      int size;

      memset(filename, 0, sizeof(filename));
      size = libnar_read_content1(&nr, &ih, filename, sizeof(filename));
      if (size >= 0 && !strncmp(filename, opts->target, sizeof(filename))) {
        while ((size = libnar_read_content2(&nr, &ih, filename, sizeof(filename))) > 0) {
            write(STDOUT_FILENO, filename, size);
        }
      }
    }
    libnar_jump_to_next_item_header(&nr, &ih);
  }

  libnar_close_reader(&nr);

  close(fd);

  return ret;
}

int main(int argc, char * const* argv)
{
  int option_index = 0;
  int c;
  int i;
  int help = 0, error = 0;

  struct nar_options opt;
  memset(&opt, 0, sizeof(struct nar_options));

  while (!help && !error) {
    c = getopt_long(argc, argv, short_options,
                    long_options, &option_index);

    if (c == -1) {
      break;
    }

    switch (c) {
    case '?':
    case 'h':
      show_help_message(argv[0]);
      help = 1;
      break;
    case 'n':
      opt.output = optarg;
      break;
    case 'c':
      if (!opt.action) {
        opt.action = CREATE;
      } else {
        ERROR("can't create a file with other action: 0x%03x", opt.action);
        error = 1;
      }
      break;
    case 'a':
      if (!opt.action) {
        opt.action = APPEND;
        opt.input = optarg;
      } else {
        ERROR("can't append a file with other action: 0x%03x", opt.action);
        error = 1;
      }
      break;
    case 'e':
      if (!opt.action) {
        opt.action = EXTRACT;
	opt.target = optarg;
      } else {
        ERROR("can't extract a file with other action: 0x%03x", opt.action);
        error = 1;
      }
      break;
    case 'l':
      if (!opt.action) {
        opt.action = LIST;
      } else {
        ERROR("can't list a file with other action: 0x%03x", opt.action);
        error = 1;
      }
      break;
    case 't':
      opt.compression_type = to_compression_type(optarg);
      switch (opt.compression_type) {
      case COMPRESSION_TYPE_LENGTH:
        PRINTF("list of supported compression type:");
        for (i = COMPRESSION_NONE; i < COMPRESSION_TYPE_LENGTH; i++) {
          PRINTF("  %s", compression_drivers[i].name);
        }
        break;
      case COMPRESSION_TYPE_LENGTH + 1:
        error = 1;
        ERROR("compression type not supported: %s", optarg);
        break;
      default:
        break;
      }
      break;
    case 'T':
      opt.cipher_type = optarg;
      break;
    case 'C':
      if (opt.action == APPEND) {
        opt.compress = 1;
      } else {
        ERROR("option --compress|-C only available with option --append|-a");
        error = 1;
      }
      break;
    case 'E':
      if (opt.action == APPEND) {
        opt.encrypt = 1;
      } else {
        ERROR("option --encrypt|-E only available with option --append|-a");
        error = 1;
      }
      break;
    default:
      ERROR("unknown options %c (0x%08x)", c, c);
      error = 1;
      break;
    }
  }

  if (!IS_COMPRESSION_SUPPORTED(opt.compression_type)) {
    ERROR("compression type not supported %u", opt.compression_type);
    error = 1;
  }

  if (!help && !error && opt.output == NULL) {
    ERROR("output should not be null: use option --narfile:<file>");
    error = 1;
  }

  if (!help && !error) {
    switch (opt.action) {
    case CREATE:
      error = main_create_nar_file(&opt);
      break;
    case APPEND:
      error = main_append_file(&opt);
      break;
    case LIST:
      error = main_list_nar_file(&opt);
      break;
    case EXTRACT:
      error = main_extract_nar_file(&opt);
      break;
    case NOTHING:
    default:
      break;
    }
  }

  return -error;
}
