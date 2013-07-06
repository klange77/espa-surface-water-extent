/******************************************************************************
NAME:           split_filename

PURPOSE:
Split the specified filename into a directory, a root file name, and an
extension.  The last character of the directory path will be a '/'.

RETURN VALUE: None

HISTORY:
Date          Programmer       Reason
----------    ---------------  -------------------------------------
05/03/2013    Song Guo         Original Development based on
                               ias_misc_split_filename routine

******************************************************************************/
#include <string.h>
#include <limits.h>         /* For PATH_MAX */
#include "error.h"
#include "input.h"

#ifndef ERROR
#define ERROR 1
#endif

#ifndef SUCCESS
#define SUCCESS 0
#endif

void split_filename 
(
    const char *filename,       /* I: Name of file to split */
    char *directory,            /* O: Directory portion of file name */
    char *root,                 /* O: Root portion of the file name */
    char *extension             /* O: Extension portion of the file name */
)
{
    char file_name[PATH_MAX];   /* Local copy of filename */
    char *ptr;                  /* String pointer */

    /* Make a local copy of filename so it is not destroyed */
    strcpy(file_name, filename);

    /* Check for a directory path */
    /* Find ending '/' */
    ptr = (char *) strrchr(file_name, '/');
    if (ptr != NULL)
    {
        strcpy (directory, file_name);
        ptr = (char *) strrchr(directory, '/');
        ptr++;
        strcpy (file_name, ptr);
        *ptr = '\0';
    }
    else
        strcpy (directory, "");

    /* Check for a file extension */
    ptr = (char *) strchr(file_name, '.');
    if (ptr != NULL)
    {
        *(ptr++) = '\0';
        strcpy (root, file_name);
        strcpy (extension, ptr);
    }
    else
    {
        strcpy (root, file_name);
        strcpy (extension, "");
    }

    /* Check for a file extension */
    ptr = (char *) strchr(extension, '.');
    if (ptr != NULL)
    {
        *(ptr++) = '\0';
        strcpy (root, extension);
        strcpy (extension, ptr);
    }
    else
    {
        strcpy (root, extension);
        strcpy (extension, "");
    }
}
