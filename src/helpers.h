#ifndef _HELPERS_H_
#define _HELPERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>


// Errors
#define UNREACHABLE(message) do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); abort(); } while(0)
#define return_defer() goto defer
#define ASSERT_NULL(value) assert((value) && #value)



// String functions
void cstr_chop_till_last_delim(char** cstr, char delimiter)
{
  if (!cstr)
    return;

  size_t len = strlen(*cstr);
  size_t index = 0;
  
  for (size_t i = 0; i < len; ++i)
    if ((*cstr)[i] == delimiter)
      index = i;
  
  *cstr += index + 1;
}


// Bit-Operations
#define set_bit(flags, bit) flags |= bit
#define unset_bit(flags, bit) flags &= ~bit
#define is_bit_set(flags, bit) (flags & bit)
#define is_only_bit_set(flags, bit) (flags == bit)
bool is_not_only_bit_set(int flags, int bit)
{
  int index = 0;

  if (!is_bit_set(flags, bit))
    return false;

  while (index < (int)sizeof(int))
  {
    int mask = (1u << (index++));

    if (is_bit_set(flags, mask) && bit != mask)
      return true;
  }

  return false;
}


#endif // !_HELPERS_H_
