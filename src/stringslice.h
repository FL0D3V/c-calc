#ifndef _STRING_SLICE_H_
#define _STRING_SLICE_H_

#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>


typedef struct {
  const char* src;
  size_t len;
  size_t start_ptr;
  size_t current_ptr;
} string_slice_t;


#define ss_current_pos(slice) ((slice)->current_ptr - (slice)->start_ptr)

#define _SS_ASSERT_SLICE_NOT_NULL(slice) \
    assert((slice) && "Given string-slice reference was NULL!")
#define _SS_ASSERT_IF_NOT_VALID(slice)                              \
    do {                                                            \
      _SS_ASSERT_SLICE_NOT_NULL(slice);                             \
      assert(slice->src && slice->len > 0 && "Not a valid input!"); \
    } while (0)
#define _SS_IS_IN_RANGE(slice)       (ss_current_pos(slice) < (slice)->len)
#define _SS_IS_NEXT_IN_RANGE(slice)  (ss_current_pos(slice) + 1 < (slice)->len)


void ss_init(string_slice_t* slice, const char* src)
{
  _SS_ASSERT_SLICE_NOT_NULL(slice);
  assert(src && "Given source was NULL!");

  size_t len = strlen(src);
  assert(len > 0 && "Given source was empty!");

  slice->src = src;
  slice->len = len;
  slice->start_ptr = (size_t)src;
  slice->current_ptr = (size_t)src;
}

char ss_get_current(string_slice_t* slice)
{
  _SS_ASSERT_IF_NOT_VALID(slice);
  assert(_SS_IS_IN_RANGE(slice) && "Out of range!");
  return *((char*)slice->current_ptr);
}

bool ss_in_range(string_slice_t* slice)
{
  _SS_ASSERT_IF_NOT_VALID(slice);
  return _SS_IS_IN_RANGE(slice);
}

bool ss_can_peek(string_slice_t* slice)
{
  _SS_ASSERT_IF_NOT_VALID(slice);
  return _SS_IS_NEXT_IN_RANGE(slice);
}

char ss_peek(string_slice_t* slice)
{
  _SS_ASSERT_IF_NOT_VALID(slice);
  assert(_SS_IS_NEXT_IN_RANGE(slice) && "Peeked after the end!");
  return *(((char*)slice->current_ptr) + 1);
}

// This function is UNSAFE because it can seek after the length of the input.
// That is implemented because else there would be no way to fully traverse the input.
void ss_seek(string_slice_t* slice)
{
  _SS_ASSERT_IF_NOT_VALID(slice);
  slice->current_ptr++;
}

void ss_seek_spaces(string_slice_t* slice)
{
  _SS_ASSERT_IF_NOT_VALID(slice);
  while(_SS_IS_IN_RANGE(slice) && isspace(ss_get_current(slice)))
    ss_seek(slice);
}

#endif // _STRING_SLICE_H_
