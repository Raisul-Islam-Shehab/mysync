#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utime.h>
#include "mysync.h"

// Implementation of the copyFile function
void copyFile(const char *src, const char *dest, int preserve_metadata)
{
    FILE *sourceFile, *destFile;
    char ch;

    sourceFile = fopen(src, "rb");
    if (sourceFile == NULL)
    {
        perror("Error opening source file");
        return;
    }

    // open the file
    destFile = fopen(dest, "wb");
    if (destFile == NULL)
    {
        perror("Error creating destination file");
        fclose(sourceFile);
        return;
    }

    // copy the file information
    while ((ch = fgetc(sourceFile)) != EOF)
    {
        fputc(ch, destFile);
    }

    fclose(sourceFile);
    fclose(destFile);

    if (preserve_metadata)
    {
        struct stat st;
        // get the file status
        if (stat(src, &st) == 0)
        {
            // change the file mode
            chmod(dest, st.st_mode);
            struct utimbuf ut;
            ut.actime = st.st_atime;
            ut.modtime = st.st_mtime;
            // set the time for the file
            utime(dest, &ut);
        }
        else
        {
            perror("Error getting file status\n");
            return;
        }
    }
}
