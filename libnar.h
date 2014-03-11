/*
** header.h for libnar
**
** Made by Nicolas DI PRIMA
** Login   nicolas <nicolas@di-prima.fr>
**
** Started on  Thu 06 Mar 2014 17:56:12 GMT Nicolas DI PRIMA
** Last update Tue 11 Mar 2014 21:27:09 GMT Nicolas DI PRIMA
*/

#ifndef LIBNAR_H_
# define LIBNAR_H_

# include "stdint.h"

/**
** align a value to ROUNDUP a value to 64bits
*/
# if !defined(ROUNDUP64)
#  define ROUNDUP64(value) (value + ((value % 8) ? (8 - (value % 8)) : 0))
# endif

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

# define FILE_HEADER_MAGIC      "[ FILE ]"
# define SIGNATURE_HEADER_MAGIC "[ SIGN ]"
# define INDEX_HEADER_MAGIC     "[ INDX ]"

typedef enum {
  FILE_FLAG_EXECUTABLE = 0x00,
  FILE_FLAG_COMPRESSED = 0x01,
  FILE_FLAG_CIPHERED   = 0x02
} file_flags_index;

# define IS_EXECUTABLE(flags) (flags && (1 << FILE_FLAG_EXECUTABLE))
# define IS_COMPRESSED(flags) (flags && (1 << FILE_FLAG_COMPRESSED))
# define IS_CIPHERED(flags)   (flags && (1 << FILE_FLAG_CIPHERED))

typedef struct {
  uint64_t magic;
  uint64_t flags;
  uint64_t length1;
  uint64_t length2;
} __attribute__((packed)) item_header;

typedef int (*get_computed_content)(void* opaque,
                                    uint8_t* buf, uint32_t const max);

typedef struct {
  int fd;

  uint64_t signature_position;
  uint64_t index_position;
} nar_writer;

int  init_nar_writer(nar_writer* nar, int fd);
void close_nar_writer(nar_writer* nar);
int  write_nar_header(nar_writer* nar,
                      uint64_t const cipher_type,
                      uint64_t const compression_type);
int  append_file(nar_writer* nar, uint64_t const flags,
                 char const* filepath, uint64_t const length_filepath,
                 uint64_t const length_content,
                 get_computed_content callback, void* opaque);

typedef struct {
  int fd;

  uint64_t item_offset;
  uint64_t item_offset_content1;
  uint64_t item_offset_content2;
} nar_reader;

int init_nar_reader(nar_reader* nar, int fd);
void close_nar_reader(nar_reader* nar);

int read_nar_header(nar_reader* nar, nar_header* nh);
int read_item_header(nar_reader* nar, item_header* ih);

int read_content1(nar_reader* nar, item_header const* ih,
                  char* buf, uint32_t const max);
int read_content2(nar_reader* nar, item_header const* ih,
                  char* buf, uint32_t const max);
int jump_to_next_item_header(nar_reader* nar, item_header* ih);

#endif /* !LIBNAR_H_ */
