/* Linux open syscall implementation, LFS.
   Copyright (C) 1991-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

#include <sysdep-cancel.h>

#ifdef CC_USE_SYSCALL_SHIMS
extern void*  __libc_malloc(size_t);
extern void  __libc_free(void*);
#include <string.h>
#include "../no_dependency_encoding.h"
#endif  // CC_USE_SYSCALL_SHIMS

#ifdef __OFF_T_MATCHES_OFF64_T
# define EXTRA_OPEN_FLAGS 0
#else
# define EXTRA_OPEN_FLAGS O_LARGEFILE
#endif

/* Open FILE with access OFLAG.  If O_CREAT or O_TMPFILE is in OFLAG,
   a third argument is the file protection.  */
int
__libc_open64 (const char *file, int oflag, ...)
{
  int mode = 0;

  if (__OPEN_NEEDS_MODE (oflag))
    {
      va_list arg;
      va_start (arg, oflag);
      mode = va_arg (arg, int);
      va_end (arg);
    }

#ifdef CC_USE_SYSCALL_SHIMS
  if(is_encoded_pointer(file)){
    size_t plaintext_file_len = strnlen(file, PATH_MAX)+1;
    void * plaintext_file = __libc_malloc(plaintext_file_len);
    strncpy(plaintext_file, file, plaintext_file_len);
    int ret = SYSCALL_CANCEL (openat, AT_FDCWD, plaintext_file, oflag | EXTRA_OPEN_FLAGS, mode);
    __libc_free(plaintext_file);
    return ret;
  }
#endif  // CC_USE_SYSCALL_SHIMS
  return SYSCALL_CANCEL (openat, AT_FDCWD, file, oflag | EXTRA_OPEN_FLAGS,
			 mode);
}

strong_alias (__libc_open64, __open64)
libc_hidden_weak (__open64)
weak_alias (__libc_open64, open64)

#ifdef __OFF_T_MATCHES_OFF64_T
strong_alias (__libc_open64, __libc_open)
strong_alias (__libc_open64, __open)
libc_hidden_weak (__open)
weak_alias (__libc_open64, open)
#endif
