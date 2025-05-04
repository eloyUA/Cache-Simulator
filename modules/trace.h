#ifndef _TRACE_H
#define _TRACE_H

#include <stdio.h>

typedef FILE *Trace;

/**
 * FUNCTION: initTrace
 * INPUT:
 *      name: The name of trace file
 *      mode = 'w' (write) or 'r' (read)
 * OUTPUT: Error/No error (-1/0)
 */
int initTrace(Trace *t, const char *name, const char *mode);

/**
 * FUNCTION: writeMemoryAddressTrace
 * INPUT:
 *      A memory address, for example:
 *          int x = 0;
 *          addMemoryAddressTrace(&x, t);
 */
void writeMemoryAddressTrace(void *addr, Trace t);

/**
 * FUNCTION: readMemoryAddressTrace
 * OUTPUT: 
 *      Error (Or the end file has been read)/No error (-1/0)
 *      (addr): The position of the byte whose memory address
 *      is read from trace.
 */
int readMemoryAddressTrace(unsigned long long *addr, Trace t);

/**
 * FUNCTION: closeTrace
 * OUTPUT: Close the trace file.
 */
void closeTrace(Trace t);

#endif