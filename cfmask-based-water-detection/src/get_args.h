
#ifndef GET_ARGS_H
#define GET_ARGS_H


#include <stdbool.h>


int
get_args (int argc,                    /* I: number of cmd-line args */
          char *argv[],                /* I: string of cmd-line args */
          char **xml_infile,           /* O: input XML filename */
          bool * verbose_flag);        /* O: verbose messaging */


#endif /* GET_ARGS_H */
