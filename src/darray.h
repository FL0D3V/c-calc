// A dynamic array implementation with a few extras.

#ifndef _DARRAY_H_
#define _DARRAY_H_

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))
#define ARRAY_GET(array, index)           \
    do {                                  \
      assert(index >= 0);                 \
      assert(index < ARRAY_LEN(array));   \
      return array[index];                \
    } while (0)


size_t ptr_diff(const void* start, const void* curr)
{
  assert(start && curr && "A NULL pointer can't be used for calculating the difference!");
  return (size_t)curr - (size_t)start;
}

#define PTR_DIFF(start, curr) (ptr_diff((const void*)(start), (const void*)(curr)))


#define da_free(da)       \
  do {                    \
    free((da).items);     \
    (da).items = NULL;    \
    (da).capacity = 0;    \
    (da).count = 0;       \
  } while(0)


// Initial capacity of a dynamic array
#define DA_INIT_CAP 128

// Append an item to a dynamic array
#define da_append(da, item)                                                                   \
  do {                                                                                        \
    if ((da)->count >= (da)->capacity) {                                                      \
      (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity * 2;                \
      (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items));              \
      assert((da)->items && "Not enough memory!");                                            \
                                                                                              \
      memset((da)->items + (da)->count,                                                       \
             0,                                                                               \
             ((da)->capacity - (da)->count) * sizeof(*(da)->items));                          \
    }                                                                                         \
                                                                                              \
    (da)->items[(da)->count++] = (item);                                                      \
  } while (0)

// Append several items to a dynamic array
#define da_append_many(da, new_items, new_items_count)                                      \
    do {                                                                                        \
      if ((da)->count + new_items_count > (da)->capacity) {                                     \
        if ((da)->capacity == 0) {                                                              \
          (da)->capacity = DA_INIT_CAP;                                                         \
        }                                                                                       \
        while ((da)->count + new_items_count > (da)->capacity) {                                \
          (da)->capacity *= 2;                                                                  \
        }                                                                                       \
        (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items));              \
        assert((da)->items && "Not enough memory!");                                            \
      }                                                                                         \
      memcpy((da)->items + (da)->count, new_items, new_items_count * sizeof(*(da)->items));     \
      (da)->count += new_items_count;                                                           \
                                                                                                \
      memset((da)->items + (da)->count,                                                         \
              0,                                                                                \
              ((da)->capacity - (da)->count) * sizeof(*(da)->items));                           \
    } while (0)


// The structure that must be defined to use this implementation
// the type of 'items' can be changed to the current need.
// This is just a dynamic list of an arbitrary type.
typedef struct {
  void* items;
  size_t capacity;
  size_t count;
} darray_t;


typedef struct {
    char* items;
    size_t capacity;
    size_t count;
} string_builder_t;


#define sb_append_char(sb, chr) da_append(sb, chr)

// Append a sized buffer to a string builder
#define sb_append_buf(sb, buf, size) da_append_many(sb, buf, size)

// Append a NULL-terminated string to a string builder
#define sb_append_cstr(sb, cstr)      \
    do {                              \
        const char *s = (cstr);       \
        size_t len = strlen(s);       \
        da_append_many(sb, s, len);   \
    } while (0)

// Append a single NULL character at the end of a string builder. So then you can
// use it a NULL-terminated C string
#define sb_append_null(sb) da_append_many(sb, "", 1)

// Free the memory allocated by a string builder
#define sb_free(sb) da_free(sb)


typedef struct {
  string_builder_t* items;
  size_t capacity;
  size_t count;
} string_list_t;


#define sl_append(sl, cstr)       \
    do {                          \
      string_builder_t sb = {0};  \
      sb_append_cstr(&sb, cstr);  \
      da_append(sl, sb);          \
    } while(0)

#define sl_append_null(sl) sl_append(sl, "")

#define sl_free(sl)                             \
    do {                                        \
      for (size_t i = 0; i < (sl).count; ++i)   \
        sb_free((sl).items[i]);                 \
      da_free(sl);                              \
    } while(0)

#endif // _DARRAY_H_
