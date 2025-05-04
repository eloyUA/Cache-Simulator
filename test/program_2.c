#include "../modules/trace.h"

#define M 1024

float v1[M];
float v2[M];
float v3[M];

float v4[M];
float dummy_1;
float v5[M];
float dummy_2;
float v6[M];

int main() {
    Trace trace;
    initTrace(&trace, "test/traceThreeVectorBad", "w");
    for (int i = 0; i < M; i++) { // Example: v3[i] = v1[i] * v2[i]
        writeMemoryAddressTrace(&v1[i], trace);
        writeMemoryAddressTrace(&v2[i], trace);
        writeMemoryAddressTrace(&v3[i], trace);
    }
    closeTrace(trace);

    initTrace(&trace, "test/traceThreeVectorGood", "w");
    for (int i = 0; i < M; i++) { // Example: v3[i] = v1[i] * v2[i]
        writeMemoryAddressTrace(&v4[i], trace);
        writeMemoryAddressTrace(&v5[i], trace);
        writeMemoryAddressTrace(&v6[i], trace);
    }
    closeTrace(trace);

    return 0;
}