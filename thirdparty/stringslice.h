#ifndef _STRING_SLICE_H_
#define _STRING_SLICE_H_

#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#define SS_FMT "%.*s"
#define SS_ARG(ss) (int)(ss)->len, (ss)->src

typedef struct
{
  const char *src;
  uintptr_t start_ptr;
  uintptr_t current_ptr;
  size_t len;
} string_slice_t;

#define ss_current_pos(slice) ((size_t)((slice)->current_ptr - (slice)->start_ptr))

#define _SS_ASSERT_SLICE_NOT_NULL(slice) \
  assert((slice) && "Given string-slice reference was NULL!")
#define _SS_ASSERT_IF_NOT_VALID(slice)                            \
  do                                                              \
  {                                                               \
    _SS_ASSERT_SLICE_NOT_NULL(slice);                             \
    assert(slice->src && slice->len > 0 && "Not a valid input!"); \
  } while (0)
#define _SS_IS_IN_RANGE(slice) (ss_current_pos(slice) < (slice)->len)
#define _SS_IS_NEXT_IN_RANGE(slice) (ss_current_pos(slice) + 1 < (slice)->len)

void ss_init_ex(string_slice_t *slice, const char *src, size_t len)
{
  _SS_ASSERT_SLICE_NOT_NULL(slice);
  assert(src && "'src' was NULL!");
  assert(len > 0 && "'len' must be greater than 0!");

  *slice = (string_slice_t){
      .src = src,
      .len = len,
      .start_ptr = (uintptr_t)src,
      .current_ptr = (uintptr_t)src,
  };
}

void ss_init(string_slice_t *slice, const char *src)
{
  _SS_ASSERT_SLICE_NOT_NULL(slice);
  assert(src && "'src' was NULL!");
  size_t len = strlen(src);
  assert(len > 0 && "Length of 'src' was 0!");

  *slice = (string_slice_t){
      .src = src,
      .len = len,
      .start_ptr = (uintptr_t)src,
      .current_ptr = (uintptr_t)src,
  };
}

#define ss_get_current(slice) ss_get_current_ex((slice), __FILE__, __LINE__)

char ss_get_current_ex(string_slice_t *slice, const char *file, size_t line)
{
  _SS_ASSERT_IF_NOT_VALID(slice);

  if (!_SS_IS_IN_RANGE(slice))
  {
    fprintf(stderr, "%s:%zu: Out of range!\n", file, line);
    abort();
  }

  return *((char *)slice->current_ptr);
}

const char *ss_get_current_ptr_ex(string_slice_t *slice, const char *file, size_t line)
{
  _SS_ASSERT_IF_NOT_VALID(slice);

  if (!_SS_IS_IN_RANGE(slice))
  {
    fprintf(stderr, "%s:%zu: Out of range!\n", file, line);
    abort();
  }

  return (char *)slice->current_ptr;
}

#define ss_get_current_ptr(slice) ss_get_current_ptr_ex((slice), __FILE__, __LINE__)

bool ss_in_range(string_slice_t *slice)
{
  _SS_ASSERT_IF_NOT_VALID(slice);
  return _SS_IS_IN_RANGE(slice);
}

bool ss_can_peek(string_slice_t *slice)
{
  _SS_ASSERT_IF_NOT_VALID(slice);
  return _SS_IS_NEXT_IN_RANGE(slice);
}

// Peaks the next character in the string-slice without seeking.
char ss_peek_ex(string_slice_t *slice, const char *file, size_t line)
{
  _SS_ASSERT_IF_NOT_VALID(slice);

  if (!_SS_IS_NEXT_IN_RANGE(slice))
  {
    fprintf(stderr, "%s:%zu: Peeked after the end!\n", file, line);
    abort();
  }

  return *(((char *)slice->current_ptr) + 1);
}

#define ss_peek(slice) ss_peek_ex((slice), __FILE__, __LINE__)

// This function is UNSAFE because it can seek after the length of the input.
// That is implemented because else there would be no way to fully traverse the input.
char ss_seek(string_slice_t *slice)
{
  _SS_ASSERT_IF_NOT_VALID(slice);
  return *((char *)(++slice->current_ptr));
}

// Used for going one character back again.
char ss_revert_seek(string_slice_t *slice)
{
  _SS_ASSERT_IF_NOT_VALID(slice);
  return *((char *)(--slice->current_ptr));
}

bool ss_is_space(string_slice_t *slice)
{
  _SS_ASSERT_IF_NOT_VALID(slice);
  return _SS_IS_IN_RANGE(slice) && isspace(ss_get_current(slice));
}

bool ss_is_new_line(string_slice_t *slice)
{
  _SS_ASSERT_IF_NOT_VALID(slice);
  return _SS_IS_IN_RANGE(slice) && ss_get_current(slice) == '\n';
}

// Returns the amount of spaces that were seeked over.
size_t ss_seek_spaces_ex(string_slice_t *slice, bool seekNewLines)
{
  _SS_ASSERT_IF_NOT_VALID(slice);
  size_t count = 0;
  while (ss_is_space(slice))
  {
    if (!seekNewLines && ss_get_current(slice) == '\n')
      break;
    ss_seek(slice);
    count++;
  }
  return count;
}

// Returns the amount of spaces that were seeked over.
size_t ss_seek_spaces(string_slice_t *slice)
{
  _SS_ASSERT_IF_NOT_VALID(slice);
  size_t count = 0;
  while (ss_is_space(slice))
  {
    ss_seek(slice);
    count++;
  }
  return count;
}

// Used to filter out all spaces and extra new-lines.
// This function will remove excessive new-lines and leave just the first.
void ss_seek_extra_new_lines(string_slice_t *slice)
{
  ss_seek_spaces_ex(slice, false);

  if (!ss_in_range(slice))
    return;

  if (ss_is_new_line(slice))
  {
    do
    {
      ss_seek(slice);
      ss_seek_spaces_ex(slice, false);
    } while (ss_in_range(slice) && ss_is_new_line(slice));

    if (ss_in_range(slice))
      ss_revert_seek(slice);
  }
}

bool ss_compare(string_slice_t *val1, string_slice_t *val2)
{
  if (!val1 || !val2)
    return false;
  if (val1->len != val2->len)
    return false;
  if (strncmp(val1->src, val2->src, val1->len) == 0)
    return true;
  return false;
}

#endif // _STRING_SLICE_H_
