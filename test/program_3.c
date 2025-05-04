#include "../modules/trace.h"

#define M 256

double m1[M][M];
double m2[M][M];
double m3[M][M];

void multiplyMatrixBadAlgorithm(Trace trace) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            for (int k = 0; k < M; k++) {
                m3[i][j] += m1[i][k] * m2[k][j];
                writeMemoryAddressTrace(&m1[i][k], trace);
                writeMemoryAddressTrace(&m2[k][j], trace);
                writeMemoryAddressTrace(&m3[i][j], trace);
            }
        }
    }
}

void multiplyMatrixGoodAlgorithm(Trace trace) {
    for (int k = 0; k < M; k++) {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                m3[i][j] += m1[i][k] * m2[k][j];
                writeMemoryAddressTrace(&m1[i][k], trace);
                writeMemoryAddressTrace(&m2[k][j], trace);
                writeMemoryAddressTrace(&m3[i][j], trace);
            }
        }
    }
}

void initAllMatrix() {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            m1[i][j] = i + j;
            m2[i][j] = i - j;
            m3[i][j] = 0;
        }
    }
}

void showM3() {
    printf("m3:\n");
    for (int i = 0; i < M; i++) {
        printf("   ");
        for (int j = 0; j < M; j++) {
            printf("%.1lf ", m3[i][j]);
        }
        printf("\n");
    }
}

int main() {
    initAllMatrix();
    Trace trace;
    initTrace(&trace, "test/traceMultiplyMatrixBad", "w");
    multiplyMatrixBadAlgorithm(trace);
    closeTrace(trace);
    //showM3();

    initAllMatrix();
    initTrace(&trace, "test/traceMultiplyMatrixGood", "w");
    multiplyMatrixGoodAlgorithm(trace);
    closeTrace(trace);
    //showM3();

    return 0;
}