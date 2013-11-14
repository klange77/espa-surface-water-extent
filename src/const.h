#ifndef CONST_H
#define CONST_H

#ifndef PI
    #ifndef M_PI
        #define PI (3.141592653589793238)
    #else
        #define PI (M_PI)
    #endif
#endif

#define TWO_PI (2.0 * PI)
#define HALF_PI (PI / 2.0)

#define DEG (180.0 / PI)
#define RAD (PI / 180.0)

/* Set up default global defines */
#define SUCCESS 0
#define ERROR 1

#endif
