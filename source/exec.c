/**
 * author:      KMEXEC
 * mail:        kmeflexable@163.com
 */

#include "../include/functions.h"

void ExecKernel(UINT64 entry_point)
{
        void (*system_entry)(void) = (void (*)(void))entry_point;
        system_entry();
}