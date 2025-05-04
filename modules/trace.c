#include "trace.h"
#include <string.h>

int initTrace(Trace *t, const char *name, const char *mode) {
    if (strcmp(mode, "w") != 0 && strcmp(mode, "r") != 0) { return -1; }
    if (strlen(name) == 0) { return -1; }

    *t = fopen(name, mode);
    if (*t == NULL) {
        return -1;
    }
    return 0;
}

void writeMemoryAddressTrace(void *addr, Trace t) {
    fprintf(t, "%p ", addr);
}

int readMemoryAddressTrace(unsigned long long *addr, Trace t) {
    char buff[32];
    int res = fscanf(t, "%s ", buff);
	sscanf(buff, "%llx", addr);

    if (res == -1 || res == EOF) {
        return -1;
    }
    return 0;
}

void closeTrace(Trace t) {
    fclose(t);
}