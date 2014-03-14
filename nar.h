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

#ifndef NAR_H_
# define NAR_H_

# include "libnar.h"

# include <stdio.h>

# if defined(DEBUG)
#  define DPRINTF(fmt, ...)                                 \
   do {                                                     \
     fprintf(stderr, "[%s:%u][%s] " fmt "\n",               \
             __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
   } while (0)

#  define ERROR(fmt, ...)                                   \
   do {                                                     \
     fprintf(stderr, "[ERROR][%s:%u][%s] " fmt "\n",        \
             __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
   } while (0)

#  define PRINTF(fmt, ...)                                  \
   do {                                                     \
     fprintf(stdout, "[%s:%u][%s] " fmt "\n",               \
             __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
   } while (0)
# else
#  define DPRINTF(fmt, ...) \
   do {                     \
   } while (0)

#  define ERROR(fmt, ...)                                  \
   do {                                                    \
     fprintf(stderr, "[ERROR] " fmt "\n", ## __VA_ARGS__); \
   } while (0)

#  define PRINTF(fmt, ...)                         \
   do {                                            \
     fprintf(stdout, "" fmt "\n", ## __VA_ARGS__); \
   } while (0)
# endif

enum option_action {
  NOTHING = 0x00,
  CREATE  = 0x01,
  APPEND  = 0x02,
  LIST    = 0x04,
  EXTRACT = 0x08
};

struct nar_options {
  enum option_action action;

  char const* output;
  nar_compression_type compression_type;
  char const* cipher_type;

  /* When appending an Item "file" */
  char const* input;
  int compress;
  int encrypt;

  char const* target;
};


#endif /* !NAR_H_ */
