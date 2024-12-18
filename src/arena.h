// Arena implementation inspired from:
// https://github.com/tsoding/arena/blob/master/arena.h

#ifndef _ARENA_H_
#define _ARENA_H_

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

typedef struct region region_t;

struct region {
  region_t* next;
  size_t capacity;
  size_t count;
  uintptr_t data[];
};

typedef struct {
  region_t *begin, *end;
} arena_t;

#define REGION_DEFAULT_CAPACITY 1024 // 1 kb

region_t* region_alloc(size_t capacity);
void region_free(region_t* region);

void* arena_alloc(arena_t* arena, size_t size_bytes);
void* arena_realloc(arena_t* arena, void* oldptr, size_t oldsz, size_t newsz);
void arena_free(arena_t* arena);

#define ARENA_DA_INIT_CAP 256

#define cast_ptr(...)

#define arena_da_append(a, da, item)                                                          \
    do {                                                                                      \
      if ((da)->count >= (da)->capacity) {                                                    \
        size_t new_capacity = (da)->capacity == 0 ? ARENA_DA_INIT_CAP : (da)->capacity * 2;   \
        (da)->items = cast_ptr((da)->items)arena_realloc(                                     \
          (a), (da)->items,                                                                   \
          (da)->capacity * sizeof(*(da)->items),                                              \
          new_capacity * sizeof(*(da)->items));                                               \
        (da)->capacity = new_capacity;                                                        \
      }                                                                                       \
                                                                                              \
      (da)->items[(da)->count++] = (item);                                                    \
    } while (0)

#endif // _ARENA_H_

#ifdef ARENA_IMPLEMENTATION

#include <stdlib.h>

region_t* region_alloc(size_t capacity)
{
  // size = size-of(REGION) + size-of(DATA in REGION) * capacity
  size_t regionSize = sizeof(region_t) + sizeof(uintptr_t) * capacity;
  
  region_t* region = (region_t*) malloc(regionSize);
  assert(region && "Not enough memory!");
  region->next = NULL;
  region->capacity = capacity;
  region->count = 0;
  return region;
}

void region_free(region_t* region)
{
  free(region);
}

void* arena_alloc(arena_t* arena, size_t size_bytes)
{
  size_t size = (size_bytes + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);
  
  assert(arena);
  
  if (!arena->end)
  {
    assert(!arena->begin); // Begin was already set.
    arena->end = region_alloc(size > REGION_DEFAULT_CAPACITY ? size : REGION_DEFAULT_CAPACITY);
    arena->begin = arena->end;
  }

  while (arena->end->count + size > arena->end->capacity && arena->end->next) {
    arena->end = arena->end->next;
  }

  if (arena->end->count + size > arena->end->capacity)
  {
    assert(!arena->end->next); // The next after current end was already set.
    arena->end->next = region_alloc(size > REGION_DEFAULT_CAPACITY ? size : REGION_DEFAULT_CAPACITY);
    arena->end = arena->end->next;
  }

  void* result = &arena->end->data[arena->end->count];
  arena->end->count += size;
  return result;
}

void* arena_realloc(arena_t* arena, void* oldptr, size_t oldsz, size_t newsz)
{
  if (newsz <= oldsz)
    return oldptr;
  
  void *newptr = arena_alloc(arena, newsz);
  char *newptr_char = (char*)newptr;
  char *oldptr_char = (char*)oldptr;
  
  for (size_t i = 0; i < oldsz; ++i) {
    newptr_char[i] = oldptr_char[i];
  }

  return newptr;
}

void arena_free(arena_t* arena)
{
  assert(arena);
  region_t* region = arena->begin;
  
  while (region)
  {
    region_t* tmp = region;
    region = region->next;
    region_free(tmp);
  }

  arena->begin = NULL;
  arena->end = NULL;
}

#endif // ARENA_IMPLEMENTATION
