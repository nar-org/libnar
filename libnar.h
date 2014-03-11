/*
** libnar.h for libnar
**
** Made by Nicolas DI PRIMA
** Login   nicolas <nicolas@di-prima.fr>
**
** Started on  Thu 06 Mar 2014 17:56:12 GMT Nicolas DI PRIMA
** Last update Tue 11 Mar 2014 22:24:54 GMT Nicolas DI PRIMA
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
  FILE_FLAG_CIPHERED   = 0x02
} file_flags_index;

# define IS_EXECUTABLE(flags) (flags && (1 << FILE_FLAG_EXECUTABLE))
# define IS_COMPRESSED(flags) (flags && (1 << FILE_FLAG_COMPRESSED))
# define IS_CIPHERED(flags)   (flags && (1 << FILE_FLAG_CIPHERED))

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
int  init_nar_writer(nar_writer* nar, int fd);

/**
** Close the nar_writer state
**
** @param nar the nar_writer state to reset
*/
void close_nar_writer(nar_writer* nar);

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
int  write_nar_header(nar_writer* nar,
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
int  append_file(nar_writer* nar, uint64_t const flags,
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
int init_nar_reader(nar_reader* nar, int fd);

/**
** close the nar_reader state
**
** @param nar the reader state to reset
*/
void close_nar_reader(nar_reader* nar);

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
int read_nar_header(nar_reader* nar, nar_header* nh);

/**
** read the item header.
**
** @param nar the reader state.
** @param ih a pointer the to return value. If must not be null. It will be use
** to store the result.
** @return 0 on success and *ih is filled with correct value. -1 or -errno on
** error.
*/
int read_item_header(nar_reader* nar, item_header* ih);

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
int read_content1(nar_reader* nar, item_header const* ih,
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
int read_content2(nar_reader* nar, item_header const* ih,
                  char* buf, uint32_t const max);

/**
** @param nar the reader state
** @param ih the previous item_state (if NULL, do nothing and return 0)
**
** @return 0 on success, -1 on error.
*/
int jump_to_next_item_header(nar_reader* nar, item_header const* ih);

#endif /* !LIBNAR_H_ */
