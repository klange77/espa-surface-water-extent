#ifndef _ERROR_HANDLER_H_
#define _ERROR_HANDLER_H_

#include "bool.h"

/* Prototypes */
void error_handler
(
    bool error_flag,  /* I: true for errors, false for warnings */
    char *module,     /* I: calling module name */
    char *errmsg      /* I: error message to be printed, without ending EOL */
);

#endif
