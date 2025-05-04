#include "../modules/trace.h"

float m[6][128];

int main() {
    Trace trace;

    initTrace(&trace, "test/traceMatrixColumn", "w");
    for (int j = 0; j < 128; j++) {
        for (int i = 0; i < 6; i++) {
            writeMemoryAddressTrace(&m[i][j], trace);
        }
    }
    closeTrace(trace);

    return 0;
}