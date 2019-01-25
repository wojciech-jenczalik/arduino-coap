#include "AllocationFix.h"

struct __freelist
{
 size_t sz;
 struct __freelist *nx;
};

extern struct __freelist *__flp;
extern uint8_t* __brkval;

void fix_malloc_bug()
 {
   for (__freelist *fp = __flp, *lfp = 0; fp; fp = fp->nx)
   {
     if (((uint8_t*)fp + fp->sz + 2) == __brkval)
     {
       __brkval = (uint8_t*)fp;
       if (lfp)
         lfp->nx = 0;
       else
         __flp = 0;
       break;
     }
     lfp = fp;
   }
 }
