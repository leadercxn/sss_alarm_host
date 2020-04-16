
#ifndef USER_BOARD_H
#define USER_BOARD_H





#define BOARD_VERSION_110 

#if defined (BOARD_VERSION_100)
    #include "board_version_100.h"
#elif defined (BOARD_VERSION_110)
    #include "board_version_110.h"
#else
    #error "Board is not defined"
#endif





#endif




























