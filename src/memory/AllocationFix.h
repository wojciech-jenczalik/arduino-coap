//copypasted from http://forum.arduino.cc/index.php/topic,46948.0.html
//"Topic: Memory leak in malloc/realloc/free. A workaround."
//written by user "fdivitto".

#ifndef	ALLOCATION_FIX_H
#define ALLOCATION_FIX_H

#include <stddef.h>
#include <stdint.h>

void fix_malloc_bug();

#endif
