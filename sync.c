#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <fnmatch.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>
#include "mysync.h"

#define MAX_SIZE 256

// Implementation of the syncDirectories function
void syncDirectories(const char *dir1, const char *dir2, const char *include_pattern, const char *exclude_pattern, int preserve_metadata, int all_file, int verbose, int recursive_sync, int no_sync)
{
    struct dirent *entry;
    struct stat stat1, stat2;
    DIR *dirStream1, *dirStream2;

    // checks if provided directory name is actually a directory or not
    if (stat(dir1, &stat1) == 0)
    {
        if (S_ISDIR(stat1.st_mode) == 0)
        {
            perror("Not a directory\n");
            return;
        }
    }
    else
    {
        perror("Error getting directory status");
        return;
    }

    // checks if provided directory name is actually a directory or not
    if (stat(dir2, &stat2) == 0)
    {
        if (S_ISDIR(stat2.st_mode) == 0)
        {
            perror("Not a directory\n");
            return;
        }
    }
    else
    {
        perror("Error getting directory status");
        return;
    }

    if (verbose)
    {
        printf("Verbose: synchronizing directory %s and %s\n", dir1, dir2);
    }

    // opens dir1
    dirStream1 = opendir(dir1);
    if (dirStream1 == NULL)
    {
        perror("Error opening directory 1\n");
        return;
    }

    // opens dir2
    dirStream2 = opendir(dir2);
    if (dirStream2 == NULL)
    {
        perror("Error opening directory 2");
        closedir(dirStream1);
        return;
    }

    // reads the directory
    while ((entry = readdir(dirStream1)) != NULL)
    {
        // Formats the pathname of the file
        char file1[MAX_SIZE + 1], file2[MAX_SIZE + 1];
        snprintf(file1, sizeof(file1), "%s/%s", dir1, entry->d_name);
        snprintf(file2, sizeof(file2), "%s/%s", dir2, entry->d_name);

        // checks the pattern and works accordingly
        if ((include_pattern == NULL || fnmatch(include_pattern, entry->d_name, 0) == 0) &&
            (exclude_pattern == NULL || fnmatch(exclude_pattern, entry->d_name, 0) != 0))
        {
            // Does the work of option [-a]
            if (!all_file)
            {
                if (fnmatch(".*", entry->d_name, 0) == 0)
                {
                    continue;
                }
            }

            // checks the status of file1
            if (stat(file1, &stat1) == -1)
            {
                perror("Error getting file status");
                continue;
            }

            // checks the status of file2 and weather file1 is a directory or not
            if (stat(file2, &stat2) == -1 && S_ISDIR(stat1.st_mode))
            {
                if (no_sync)
                {
                    printf("Verbose: to be copied file identified -> %s\n", file1);
                }
                else
                {
                    // create directory
                    if (mkdir(file2, stat1.st_mode) == 0)
                    {
                        if (verbose)
                            printf("Verbose: Created directory %s\n", file2);
                    }
                    else
                    {
                        perror("Error creating directory");
                        continue;
                    }
                }

                // recursive synchronize
                if (recursive_sync && !no_sync)
                {
                    syncDirectories(file1, file2, include_pattern, exclude_pattern, preserve_metadata, all_file, verbose, recursive_sync, no_sync);
                }
            }
            else if (S_ISREG(stat1.st_mode) && ((stat(file2, &stat2) == -1) || stat1.st_mtime > stat2.st_mtime))
            {
                // checks if option [-n] is set or not
                if (no_sync)
                {
                    printf("Verbose: to be copied file identified %s -> %s\n", file1, file2);
                }
                else
                {
                    // copy file
                    copyFile(file1, file2, preserve_metadata);
                    if (verbose)
                    {
                        if (preserve_metadata)
                        {
                            printf("Copied: %s -> %s and preserved metadata(modification time, permission)\n", file1, file2);
                        }
                        else
                        {
                            printf("Copied: %s -> %s\n", file1, file2);
                        }
                    }
                }
            }
        }
    }

    // close dirStream1
    closedir(dirStream1);

    // set dirStream2 to point at the first file of the directory
    rewinddir(dirStream2);

    // reads the directory
    while ((entry = readdir(dirStream2)) != NULL)
    {
        // Formats the pathname of the file
        char file1[MAX_SIZE + 1], file2[MAX_SIZE + 1];
        snprintf(file1, sizeof(file1), "%s/%s", dir1, entry->d_name);
        snprintf(file2, sizeof(file2), "%s/%s", dir2, entry->d_name);

        // checks the pattern and works accordingly
        if ((include_pattern == NULL || fnmatch(include_pattern, entry->d_name, 0) == 0) &&
            (exclude_pattern == NULL || fnmatch(exclude_pattern, entry->d_name, 0) != 0))
        {
            // Does the work of option [-a]
            if (!all_file)
            {
                if (fnmatch(".*", entry->d_name, 0) == 0)
                {
                    continue;
                }
            }

            // checks the status of file1
            if (stat(file2, &stat2) == -1)
            {
                perror("Error getting file status");
                continue;
            }

            // checks the status of file2 and weather file1 is a directory or not
            if (stat(file1, &stat1) == -1 && S_ISDIR(stat2.st_mode))
            {
                // checks if the option [-n] is set or not
                if (no_sync)
                {
                    printf("Verbose: to be copied file identified -> %s\n", file2);
                }
                else
                {
                    // create directory
                    if (mkdir(file1, stat2.st_mode) == 0)
                    {
                        if (verbose)
                            printf("Verbose: Created directory %s\n", file1);
                    }
                    else
                    {
                        perror("Error creating directory");
                        continue;
                    }
                }
                // recursive synchronize
                if (recursive_sync && !no_sync)
                {
                    syncDirectories(file2, file1, include_pattern, exclude_pattern, preserve_metadata, all_file, verbose, recursive_sync, no_sync);
                }
            }
            else if (S_ISREG(stat2.st_mode) && ((stat(file1, &stat1) == -1) || stat2.st_mtime > stat1.st_mtime))
            {
                if (no_sync)
                {
                    printf("Verbose: to be copied file identified %s -> %s\n", file2, file1);
                }
                else
                {
                    copyFile(file2, file1, preserve_metadata);
                    if (verbose)
                    {
                        if (preserve_metadata)
                        {
                            printf("Copied: %s -> %s and preserved metadata(modification time, permission)\n", file2, file1);
                        }
                        else
                        {
                            printf("Copied: %s -> %s\n", file2, file1);
                        }
                    }
                }
            }
        }
    }

    // close dirStream2
    closedir(dirStream2);
}
