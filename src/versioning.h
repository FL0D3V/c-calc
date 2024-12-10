#ifndef _VERSIONING_H_
#define _VERSIONING_H_

// Versioning
typedef struct {
  size_t major;
  size_t minor;
  size_t revision;
} version_t;

#define version(maj, min, rev) ((version_t) { .major = (maj), .minor = (min), .revision = (rev) })
#define VERSION_FORMAT "v%zu.%zu.%zu"
#define VERSION_ARGS(v) (v).major, (v).minor, (v).revision

#endif // !_VERSIONING_H_
