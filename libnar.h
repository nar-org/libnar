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

#ifndef LIBNAR_H_
# define LIBNAR_H_

# include "stdint.h"

/**
** @file libnar.h
** @brief This is a C library to read/write NAR file.
*/

/**
** align a value to ROUNDUP a value to 64bits
*/
# if !defined(ROUNDUP64)
#  define ROUNDUP64(value) (value + ((value % 8) ? (8 - (value % 8)) : 0))
# endif

/*
** ------------- NAR HEADER --------------------------------------------------
*/

# define NAR_HEADER_MAGIC "[ NARH ]"
# define NAR_HEADER_VERSION_MAJOR 1
# define NAR_HEADER_VERSION_MINOR 0

typedef struct {
  uint64_t magic;
  struct {
    uint32_t major;
    uint32_t minor;
  } __attribute__((packed)) version;
  uint64_t cipher_type;
  uint64_t compression_type;
  uint64_t signature_position;
  uint64_t index_position;
  uint64_t unused[2];
} __attribute__((packed)) nar_header;

typedef enum {
  COMPRESSION_NONE        = 0,
  COMPRESSION_DEFLATE     = 1,

  COMPRESSION_TYPE_LENGTH = 2
} nar_compression_type;

/*
** ------------- ITEM HEADER -------------------------------------------------
*/

# define FILE_HEADER_MAGIC      "[ FILE ]"
# define SIGNATURE_HEADER_MAGIC "[ SIGN ]"
# define INDEX_HEADER_MAGIC     "[ INDX ]"

typedef struct {
  uint64_t magic;
  uint64_t flags;
  uint64_t length1;
  uint64_t length2;
} __attribute__((packed)) item_header;

/*
** ---- PER FILE HEADER
*/

typedef enum {
  FILE_FLAG_EXECUTABLE = 0x00,
  FILE_FLAG_COMPRESSED = 0x01,
  FILE_FLAG_ENCRYPTED  = 0x02
} file_flags_index;

# define FILE_EXECUTABLE (1 << FILE_FLAG_EXECUTABLE)
# define FILE_COMPRESSED (1 << FILE_FLAG_COMPRESSED)
# define FILE_ENCRYPTED  (1 << FILE_FLAG_ENCRYPTED)

# define IS_EXECUTABLE(flags) (flags & FILE_EXECUTABLE)
# define IS_COMPRESSED(flags) (flags & FILE_COMPRESSED)
# define IS_ENCRYPTED(flags)  (flags & FILE_ENCRYPTED)

/*
** ------------- LIBNAR ------------------------------------------------------
*/

/*
** ---- WRITER
*/

/**
** This is a callback used to get the content to store in the nar archive file.
**
** @param opaque whatever you may need to get the content to store
** @param buf is the buffer which will be write in the nar archive file
** @param max is the buffer size
**
** @return returns the number of bytes to write in buf. 0 if nothing more to
** write and -1 in error case (and errno may contains an error code).
*/
typedef int (*get_computed_content)(void* opaque,
                                    uint8_t* buf, uint32_t const max);

/**
** This is the structure to use for the writing commands.
*/
typedef struct {
  int fd;

  uint64_t signature_position;
  uint64_t index_position;
} nar_writer;

/**
** Initialize the nar_writer structure.
**
** @param nar is the nar_writer state to initialize
** @param fd is the file descriptor where all the data will be stored
**
** @return returns 0 on success else -1.
*/
int libnar_init_writer(nar_writer* nar, int fd);

/**
** Close the nar_writer state
**
** @param nar the nar_writer state to reset
*/
void libnar_close_writer(nar_writer* nar);

/**
** write the NAR HEADER in the given state.
** The header will be stored at the begin of the file descriptor given in the
** nar_writer state if possible (i.e. if it can seek to the begin of the file).
**
** @param nar the nar_writer state
** @param cipher_type is not defined yet TODO
** @param compression_type is not defined yet TODO
**
** @return 0 on success. -1 or -errno on error.
*/
int libnar_write_nar_header(nar_writer* nar,
                            uint64_t const cipher_type,
                            uint64_t const compression_type);

/**
** append a file in a NAR ARCHIVE (the file descriptor in the nar_writer state)
**
** @param nar the nar_writer state
** @param flags the item file flags
** @param filepath the filepath (ciphered or not)
** @param length_filepath the filepath size not necesserly ROUNDUP64
** @param callback the method to get the content to stor (ciphered or not)
** @param opaque the userdata to give to the callback function
**
** @return 0 on success. -1 or -errno on error.
*/
int libnar_append_file(nar_writer* nar, uint64_t const flags,
                       char const* filepath, uint64_t const length_filepath,
                       uint64_t const length_content,
                       get_computed_content callback, void* opaque);

/*
** ---- READER
*/

/**
** nar_reader state
*/
typedef struct {
  int fd;

  uint64_t item_offset;
  uint64_t item_offset_content1;
  uint64_t item_offset_content2;
} nar_reader;

/**
** initialize the nar_reader state
**
** @param nar the nar_reader state to initialize
** @param fd the the NAR file descriptor
**
** @return 0 on success. -1 on error.
*/
int libnar_init_reader(nar_reader* nar, int fd);

/**
** close the nar_reader state
**
** @param nar the reader state to reset
*/
void libnar_close_reader(nar_reader* nar);

/**
** read the NAR HEADER. This method SEEK to the begin of the file if possible
** (i.e. it is not a socked or a pipe) and reset
**
** @param nar the nar_reader state
** @param nh a pointer to the return value. It sould not be null. It will be
** filled with the result.
** @return 0 on success and *nh is the filled with correct value. -1 or -errno
** on error
*/
int libnar_read_nar_header(nar_reader* nar, nar_header* nh);

/**
** read the item header.
**
** @param nar the reader state.
** @param ih a pointer the to return value. If must not be null. It will be use
** to store the result.
** @return 0 on success and *ih is filled with correct value. -1 or -errno on
** error.
*/
int libnar_read_item_header(nar_reader* nar, item_header* ih);

/**
** read the content1 (filename in the case of a file)
**
** @param nar the reader state
** @param ih the current item header (use read_nar_header to get the item
** header).
** @param buf it will be filled with the readed content. It is up to you to
** uncrypt or uncompress it if needed.
** @param max the buf size
**
** @return returns the readed size (i.e. less than max) or 0 (if nothing more
** to read). -1 or -errno on error.
*/
int libnar_read_content1(nar_reader* nar, item_header const* ih,
                         char* buf, uint32_t const max);

/**
** read the content2 (the file content in the case of a file)
**
** @param nar the reader state
** @param ih the current item header (use read_nar_header to get the item
** header).
** @param buf it will be filled with the readed content. It is up to you to
** uncrypt or uncompress it if needed.
** @param max the buf size
**
** @return returns the readed size (i.e. less than max) or 0 (if nothing more
** to read). -1 or -errno on error.
*/
int libnar_read_content2(nar_reader* nar, item_header const* ih,
                         char* buf, uint32_t const max);

/**
** @param nar the reader state
** @param ih the previous item_state (if NULL, do nothing and return 0)
**
** @return 0 on success, -1 on error.
*/
int libnar_jump_to_next_item_header(nar_reader* nar, item_header const* ih);

#endif /* !LIBNAR_H_ */
