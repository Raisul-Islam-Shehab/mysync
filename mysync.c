#include <stdio.h>
#include <getopt.h>
#include "mysync.h"

int main(int argc, char *argv[])
{
    const char *include_pattern = NULL;
    const char *exclude_pattern = NULL;
    int preserve_metadata = 0;
    int recursive_sync = 0;
    int verbose = 0;
    int all_file = 0;
    int no_sync = 0;

    int opt;
    while ((opt = getopt(argc, argv, "anrpvi:o:")) != -1)
    {
        switch (opt)
        {
        case 'a':
            // Include hidden and configuration files
            // include_pattern = ".*";
            all_file = 1;
            break;
        case 'i':
            // Ignore pattern
            exclude_pattern = optarg;
            break;
        case 'o':
            // Include only pattern
            include_pattern = optarg;
            break;
        case 'n':
            // Dry run mode (only print files to be copied)
            no_sync = 1; // Preserve metadata during a dry run
            verbose = 1;
            break;
        case 'r':
            // Recursive synchronization
            recursive_sync = 1;
            break;
        case 'v':
            // Verbose output (provide additional information)
            // You can add verbose information to various parts of the code
            // For example: printf("Verbose: Some information here\n");
            verbose = 1;
            break;
        case 'p':
            // New copy of the file will have the same modification time and
            // same permission as the old file
            preserve_metadata = 1;
            break;
        default:
            // Invalid command-line option
            fprintf(stderr, "Usage: %s [-a] [-i pattern] [-o pattern] [-p] [-n] [-r] [-v] dir1 dir2 [dir3 ...]\n", argv[0]);
            return 1;
        }
    }

    // Check if enough directory arguments are provided
    if (optind >= argc - 1)
    {
        fprintf(stderr, "Usage: %s [-a] [-i pattern] [-o pattern] [-p] [-n] [-r] [-v] dir1 dir2 [dir3 ...]\n", argv[0]);
        return 1;
    }

    // Loop through directory pairs for synchronization
    for (int i = optind; i < argc - 1; i++)
    {
        for (int j = i + 1; j < argc; j++)
        {
            // synchronization (as in the original code)
            syncDirectories(argv[i], argv[j], include_pattern, exclude_pattern, preserve_metadata, all_file, verbose, recursive_sync, no_sync);
        }
    }

    return 0;
}
