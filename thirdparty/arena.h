// Arena implementation inspired from:
// https://github.com/tsoding/arena/blob/master/arena.h

#ifndef _ARENA_H_
#define _ARENA_H_

#include <stddef.h>
#include <stdint.h>

typedef struct region region_t;

struct region
{
  region_t *next;
  size_t capacity;
  size_t count;
  uintptr_t data[];
};

typedef struct
{
  region_t *begin, *end;
} arena_t;

#define REGION_DEFAULT_CAPACITY 1024 // 1 kb

region_t *region_alloc(size_t capacity);
void region_free(region_t *region);

void *arena_alloc(arena_t *arena, size_t size_bytes);
void *arena_realloc(arena_t *arena, void *oldptr, size_t oldsz, size_t newsz);
char *arena_strdup(arena_t *arena, const char *s);
void arena_free(arena_t *arena);

typedef enum
{
  FILE_OK,
  FILE_NOT_EXIST,
  FILE_TOO_LARGE,
  FILE_READ_ERROR,
} e_read_file_err_codes;

typedef struct
{
  const char *value;
  size_t length;
} string_view_t;

e_read_file_err_codes arena_read_file(arena_t *arena, const char *fileName, string_view_t *output);
void print_read_file_error(const char *fileName, e_read_file_err_codes errorCode);

#define ARENA_DA_INIT_CAP 256

#define cast_ptr(...)

#define arena_da_append(a, da, item)                                                      \
  do                                                                                      \
  {                                                                                       \
    if ((da)->count >= (da)->capacity)                                                    \
    {                                                                                     \
      size_t new_capacity = (da)->capacity == 0 ? ARENA_DA_INIT_CAP : (da)->capacity * 2; \
      (da)->items = cast_ptr((da)->items) arena_realloc(                                  \
          (a), (da)->items,                                                               \
          (da)->capacity * sizeof(*(da)->items),                                          \
          new_capacity * sizeof(*(da)->items));                                           \
      (da)->capacity = new_capacity;                                                      \
    }                                                                                     \
                                                                                          \
    (da)->items[(da)->count++] = (item);                                                  \
  } while (0)

#endif // _ARENA_H_

#ifdef ARENA_IMPLEMENTATION

#include <stdlib.h>
#include <assert.h>

region_t *region_alloc(size_t capacity)
{
  // size = size-of(REGION) + size-of(DATA in REGION) * capacity
  size_t regionSize = sizeof(region_t) + sizeof(uintptr_t) * capacity;

  region_t *region = (region_t *)malloc(regionSize);
  assert(region && "Not enough memory!");
  region->next = NULL;
  region->capacity = capacity;
  region->count = 0;
  return region;
}

void region_free(region_t *region)
{
  assert(region);

  free(region);
}

void *arena_alloc(arena_t *arena, size_t size_bytes)
{
  assert(arena);

  size_t size = (size_bytes + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);

  if (!arena->end)
  {
    assert(!arena->begin); // Begin was already set.
    arena->end = region_alloc(size > REGION_DEFAULT_CAPACITY ? size : REGION_DEFAULT_CAPACITY);
    arena->begin = arena->end;
  }

  while (arena->end->count + size > arena->end->capacity && arena->end->next)
  {
    arena->end = arena->end->next;
  }

  if (arena->end->count + size > arena->end->capacity)
  {
    assert(!arena->end->next); // The next after current end was already set.
    arena->end->next = region_alloc(size > REGION_DEFAULT_CAPACITY ? size : REGION_DEFAULT_CAPACITY);
    arena->end = arena->end->next;
  }

  void *result = &arena->end->data[arena->end->count];
  arena->end->count += size;
  return result;
}

void *arena_realloc(arena_t *arena, void *oldptr, size_t oldsz, size_t newsz)
{
  if (newsz <= oldsz)
    return oldptr;

  void *newptr = arena_alloc(arena, newsz);
  char *newptr_char = (char *)newptr;
  char *oldptr_char = (char *)oldptr;

  for (size_t i = 0; i < oldsz; ++i)
  {
    newptr_char[i] = oldptr_char[i];
  }

  return newptr;
}

char *arena_strdup(arena_t *arena, const char *cstr)
{
  assert(arena);
  assert(cstr);

  size_t len = strlen(cstr);
  char *res = arena_alloc(arena, len);
  strcpy(res, cstr);
  return res;
}

void arena_free(arena_t *arena)
{
  assert(arena);

  region_t *region = arena->begin;

  while (region)
  {
    region_t *tmp = region;
    region = region->next;
    region_free(tmp);
  }

  arena->begin = NULL;
  arena->end = NULL;
}

e_read_file_err_codes arena_read_file(arena_t *arena, const char *fileName, string_view_t *output)
{
  assert(arena);
  assert(fileName);
  assert(output);

  bool isNullTerminated = false;

  FILE *f = fopen(fileName, "rb");

  if (!f)
    return FILE_NOT_EXIST;

  if (fseek(f, 0, SEEK_END) != 0)
    return FILE_READ_ERROR;

  size_t length = ftell(f);

  // 1 GiB; best not to load a whole large file in one string
  if (length > 1073741824)
    return FILE_TOO_LARGE;

  if (fgetc(f) == EOF)
    isNullTerminated = true;

  if (fseek(f, 0, SEEK_SET) != 0)
    return FILE_READ_ERROR;

  char *buffer = (char *)arena_alloc(arena, isNullTerminated ? length : length + 1);

  if (length)
  {
    size_t read_length = fread(buffer, 1, length, f);

    if (length != read_length)
      return FILE_READ_ERROR;
  }

  fclose(f);

  if (!isNullTerminated)
    buffer[length] = '\0';

  *output = (string_view_t){
      .value = buffer,
      .length = isNullTerminated ? length : length + 1,
  };

  return FILE_OK;
}

void print_read_file_error(const char *fileName, e_read_file_err_codes errorCode)
{
  switch (errorCode)
  {
  case FILE_OK:
    fprintf(stdout, "FILE(%s): File ok!\n", fileName);
    break;
  case FILE_NOT_EXIST:
    fprintf(stderr, "FILE-ERROR(%s): File not found!\n", fileName);
    break;
  case FILE_TOO_LARGE:
    fprintf(stderr, "FILE-ERROR(%s): File too big!\n", fileName);
    break;
  case FILE_READ_ERROR:
    fprintf(stderr, "FILE-ERROR(%s): Error while reading file!\n", fileName);
    break;
  default:
    UNREACHABLE("Invalid read-file-error-code!");
  }
}

#endif // ARENA_IMPLEMENTATION
