#ifndef _C_HELPERS_H_
#define _C_HELPERS_H_

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

#define c_is_decimal_seperator(c) ((c) == _DECIMAL_SEPERATOR_CHARACTER)
#define c_is_number(c) (isdigit((c)) || c_is_decimal_seperator((c)))

bool cstr_is_new_line_ex(const char *cstr, size_t len)
{
  if (!cstr || len != 1)
    return false;
  return cstr[0] == '\n';
}

bool cstr_is_new_line(const char *cstr)
{
  if (!cstr || strlen(cstr) != 1)
    return false;
  return cstr[0] == '\n';
}

#define c_is_new_line(c) ((c) == '\n')

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

// The 'ret' value variations
typedef enum
{
  // Input was NULL or empty. Cursor will be set to 0.
  NCR_NULL_OR_EMPTY,
  // Input IS a valid number and the cursor will be set to the length of the input.
  NCR_VALID,
  // Input has too many commas. Cursor will be set to the error position in the string.
  NCR_TOO_MANY_COMMAS,
  // The comma was the last character. Cursor will be set to the error position in the string.
  NCR_COMMA_AT_END,
  // Input was NOT a number. Cursor will be set to the error position in the string.
  NCR_NOT_A_NUMBER,

  // Count of return values.
  NCR_COUNT
} num_check_ret_t;

static_assert(NCR_COUNT == 5, "Amount of number-check-return-values have changed");

bool cstr_to_number(const char *cstr, size_t length, double *result)
{
  char *endptr;
  *result = strtod(cstr, &endptr);
  if (!endptr || endptr == cstr || (size_t)(endptr - cstr) != length)
    return false;
  return true;
}

typedef struct
{
  num_check_ret_t ret;
  int cursor;
  double result;
} num_check_t;

#define _NUM_CHECK_NULL() ((num_check_t){.ret = NCR_NULL_OR_EMPTY})
#define _NUM_CHECK_SUCCESS(cur, num) ((num_check_t){.ret = NCR_VALID, .cursor = (cur), .result = (num)})
#define _NUM_CHECK_TOO_MANY_COMMA(cur) ((num_check_t){.ret = NCR_TOO_MANY_COMMAS, .cursor = (cur)})
#define _NUM_CHECK_COMMA_AT_END(cur) ((num_check_t){.ret = NCR_COMMA_AT_END, .cursor = (cur)})
#define _NUM_CHECK_INVALID(cur) ((num_check_t){.ret = NCR_NOT_A_NUMBER, .cursor = (cur)})

num_check_t cstr_check_number_ex(const char *cstr, size_t len, bool includeNumberProOps)
{
  if (!cstr || len == 0)
    return _NUM_CHECK_NULL();

  bool decimalSeperatorFound = false;

  char *current = (char *)cstr;

  for (size_t i = 0; i < len; ++i)
  {
    if (includeNumberProOps && i == 0 && (*current == '-' || *current == '+'))
      continue;

    if (!c_is_number(*current))
      return _NUM_CHECK_INVALID(i);

    if (c_is_decimal_seperator(*current))
    {
      if (decimalSeperatorFound)
        return _NUM_CHECK_TOO_MANY_COMMA(i);

      if (i == len - 1)
        return _NUM_CHECK_COMMA_AT_END(i);

      decimalSeperatorFound = true;
    }

    current++;
  }

  double number = 0.0;

  if (!cstr_to_number(cstr, len, &number))
    UNREACHABLE("Error while converting a string to a number!");

  return _NUM_CHECK_SUCCESS(len, number);
}

num_check_t cstr_check_number(const char *cstr, bool includeNumberProOps)
{
  if (!cstr)
    return _NUM_CHECK_NULL();

  return cstr_check_number_ex(cstr, strlen(cstr), includeNumberProOps);
}

#endif // !_C_HELPERS_H_
