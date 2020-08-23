#include <stdio.h>
#include "calculatePI.h"


void CalculatePI() {
    int r[280 + 1];
    int i, k;
    int b, d;
    int c = 0;

    for (i = 0; i < 280; i++) {
        r[i] = 2000;
    }

    for (k = 280; k > 0; k -= 14) {
        d = 0;

        i = k;
        for (;;) {
            d += r[i] * 10000;
            b = 2 * i - 1;

            r[i] = d % b;
            d /= b;
            i--;
            if (i == 0) break;
            d *= i;
        }
        printf("%.4d", c + d / 10000);
        c = d % 10000;
    }
	printf("\n");
	return;
}

