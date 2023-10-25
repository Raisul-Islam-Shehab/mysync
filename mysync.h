#ifndef MYSYNC_H
#define MYSYNC_H

// Function prototypes
void copyFile(const char *src, const char *dest, int preserve_metadata);
void syncDirectories(const char *dir1, const char *dir2, const char *include_pattern, const char *exclude_pattern, int preserve_metadata, int all_file, int verbose, int recursive_sync, int no_sync);

#endif // MYSYNC_H
