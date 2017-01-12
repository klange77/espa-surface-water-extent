
#ifndef UTILITIES_H
#define UTILITIES_H


#include <stdio.h>
#include <stdint.h>

#include "const.h"
#include "error_handler.h"

#define LOG_MESSAGE(message, module) \
            write_message((message), (module), "INFO", \
                          __FILE__, __LINE__, stdout);


#define WARNING_MESSAGE(message, module) \
            write_message((message), (module), "WARNING", \
                          __FILE__, __LINE__, stdout);


#define ERROR_MESSAGE(message, module) \
            write_message((message), (module), "ERROR", \
                          __FILE__, __LINE__, stdout);


#define RETURN_ERROR(message, module, status) \
           {write_message((message), (module), "ERROR", \
                          __FILE__, __LINE__, stdout); \
            return (status);}

/******************************************************************************
MODULE:  pixel_qa_is_fill

PURPOSE: Determines if the current Level-2 pixel QA pixel is fill

RETURN VALUE:
Type = boolean
Value           Description
-----           -----------
 true            Pixel is fill
 false           Pixel is not fill

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
 ******************************************************************************/
static inline bool pixel_qa_is_fill
(
    uint16_t l2_qa_pix      /* I: Pixel QA value for current pixel */
)
{
    if (((l2_qa_pix >> L2QA_FILL) & L2QA_SINGLE_BIT) == 1)
        return true;
    else
        return false;
}

/******************************************************************************
MODULE:  pixel_qa_is_clear

PURPOSE: Determines if the current Level-2 pixel QA pixel is clear

RETURN VALUE:
Type = boolean
Value           Description
-----           -----------
true            Pixel is clear
false           Pixel is not clear

NOTES:
1. This is an inline function so it should be fast as the function call overhead
   is eliminated by dropping the code inline with the original application.
 ******************************************************************************/
static inline bool pixel_qa_is_clear
(
    uint16_t l2_qa_pix      /* I: Pixel QA value for current pixel */
)
{
    if (((l2_qa_pix >> L2QA_CLEAR) & L2QA_SINGLE_BIT) == 1)
        return true;
    else
        return false;
}



void write_message
(
    const char *message, /* I: message to write to the log */
    const char *module,  /* I: module the message is from */
    const char *type,    /* I: type of the error */
    char *file,          /* I: file the message was generated in */
    int line,            /* I: line number in the file where the message was
                               generated */
    FILE *fd             /* I: where to write the log message */
);


#endif /* UTILITIES_H */
