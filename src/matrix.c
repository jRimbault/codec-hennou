/**
 * @author: jRimbault
 * @date:   2017-11-21
 * @last modified by:   jRimbault
 * @last modified time: 2017-11-21
 */

#include <stdio.h>
#include <stdlib.h>

/** I don't want to get a double, so here's my own pow function */
int pow_int(int n, int p)
{
    if (p > 1) {
        return n * pow_int(n, p - 1);
    }
    return n;
}

/**
 * Parses key file, create matrix
 *
 * @param  filename file containing the matrix
 *
 * @return          matrix G4C
 */
char* matrix(char* filename)
{
    FILE* keyfile;
    char* matrix;
    char  keychar[35];
    int i, j;

    keyfile = fopen(filename, "r");
    if (!keyfile) {
        printf("Couldn't access key file. Use --help.\n");
        exit(10);
    }

    fseek(keyfile, 5, SEEK_SET);
    for (i = 0; i < 35; i++) {
        keychar[i] = getc(keyfile);
    }
    matrix = malloc(4 * sizeof(char));
    for (j = 0; j < 4; j++) {
        matrix[j] = 0;
        for (i = 0; i < 8; i++) {
            // 48 = 0 ascii
            // 49 = 1 ascii
            if (keychar[i+(j*9)] == 49) {
                matrix[j] += pow_int(2, 7-i);
            }
        }
    }
    fclose(keyfile);
    return matrix;
}
