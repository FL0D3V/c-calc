#ifndef _VERSIONING_H_
#define _VERSIONING_H_

// Versioning
typedef struct {
  size_t major;
  size_t minor;
  size_t revision;
  size_t build;
} version_t;

#define VERSION_NEW(maj, min, rev) (version_t) { .major = (maj), .minor = (min), .revision = (rev), .build = 1 }
#define VERSION_NEW_EX(maj, min, rev, b) (version_t) { .major = (maj), .minor = (min), .revision = (rev), .build = (b) }
#define VERSION_FORMAT "v%zu.%zu.%zu-%zu"
#define VERSION_ARGS(v) (v).major, (v).minor, (v).revision, (v).build

#endif // !_VERSIONING_H_
