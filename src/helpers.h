#ifndef _HELPERS_H_
#define _HELPERS_H_

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

// Array functions

#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

// Errors

#define UNREACHABLE_MSG(message) fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message)
#define UNREACHABLE(message)  \
  do                          \
  {                           \
    UNREACHABLE_MSG(message); \
    abort();                  \
  } while (0)
#define UNREACHABLE_DEFER(message) \
  do                               \
  {                                \
    UNREACHABLE_MSG(message);      \
    goto defer;                    \
  } while (0)
#define ASSERT_NULL(value) assert((value) && #value)
#define return_defer() goto defer

// Bool functions

#define bool_cstr(flag) (flag) ? "true" : "false"

// Char functions

// TODO: Does currently not print correctly after around 5 decimal digits!
#define DOUBLE_PRINT_FORMAT "%.05lf"

#define _DECIMAL_SEPERATOR_CHARACTER '.'
#define _MINUS_CHARACTER '-'

#define c_is_decimal_seperator(c) ((c) == _DECIMAL_SEPERATOR_CHARACTER)
#define c_is_number(c) (isdigit((c)) || c_is_decimal_seperator((c)))

// String functions

void cstr_chop_till_last_delim(char **cstr, char delimiter)
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

// Number functions

// Compares two doubles up to a specific number of decimal places.
// Returns true if they match up to the given precision.
bool compare_doubles(double a, double b, int decimal_places)
{
  // Calculate epsilon (e.g., 2 decimal places -> 0.01, 3 -> 0.001)
  double epsilon = pow(10, -decimal_places);

  // fabs() gets the absolute difference between the two numbers
  return fabs(a - b) < epsilon;
}

typedef struct
{
  int ret;
  int cursor;
} num_check_t;

#define _NUM_CHECK_NULL() ((num_check_t){.ret = 1, .cursor = 0})
#define _NUM_CHECK_SUCCESS(cur) ((num_check_t){.ret = 0, .cursor = (cur)})
#define _NUM_CHECK_COMMA_ERR(cur) ((num_check_t){.ret = -1, .cursor = (cur)})
#define _NUM_CHECK_INVALID(cur) ((num_check_t){.ret = -2, .cursor = (cur)})

// Returns an 'num_check_t'.
// The 'ret' value variations:
//  1: Input was NULL or empty. Cursor will be set to 0.
//  0: Input IS a valid number and the cursor will be set to the length of the input.
// -1: Input has too many commas. Cursor will be set to the error position in the string.
// -2: Input was NOT a number. Cursor will be set to the error position in the string.
num_check_t cstr_is_number_ex(const char *cstr, size_t len)
{
  if (!cstr || len == 0)
    return _NUM_CHECK_NULL();

  bool decimalSeperatorFound = false;

  // TODO: Rethink!
  char *current = (char *)cstr;

  for (size_t i = 0; i < len; ++i)
  {
    if (i == 0 && *current == _MINUS_CHARACTER)
      continue;

    if (!c_is_number(*current))
      return _NUM_CHECK_INVALID(i);

    if (c_is_decimal_seperator(*current))
    {
      if (decimalSeperatorFound)
        return _NUM_CHECK_COMMA_ERR(i);
      else
        decimalSeperatorFound = true;
    }

    current++;
  }

  return _NUM_CHECK_SUCCESS(len);
}

num_check_t cstr_is_number(const char *cstr)
{
  // Needs to check for null before trying to get the length.
  if (!cstr)
    return _NUM_CHECK_NULL();

  return cstr_is_number_ex(cstr, strlen(cstr));
}

#endif // !_HELPERS_H_
