/*======================================================

Defines.h

some global defines

======================================================*/

#if 0
	#define MRTFLOAT Fixed
	#define MRTFLOAT_MAX Fixed::MAXVALUE()
	#include "FixedPoint.h"
#else
	#define MRTFLOAT float
	#define MRTFLOAT_MAX FLT_MAX
#endif

#define PIXELDEBUG true

#define GAME_NONE 0
#define GAME_SNAKE 1
#define GAME_DOTS 2
#define GAME_MANDELBROT 3
#define GAME_DOTS2D 4

#define FINALGAME GAME_SNAKE

#define MRTARRAYSIZE(A) (sizeof(A) / sizeof(A[0]))
