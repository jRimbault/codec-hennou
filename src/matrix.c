/**
 * @author: jRimbault
 * @date:   2017-11-21
 * @last modified by:   jRimbault
 * @last modified time: 2017-11-21
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    FILE* keyfile = fopen(filename, "r");
    if (!keyfile) {
        printf("Couldn't access key file. Use --help.\n");
        exit(10);
    }

    fseek(keyfile, 5, SEEK_SET);
    char string_matrix[35];
    fread(string_matrix, 35, 1, keyfile);
    fclose(keyfile);

    int i = 0;
    char* key_part = strtok(string_matrix, " ");
    unsigned char* matrix = calloc(4, sizeof(char));
    while (key_part != NULL && i < 4) {
        matrix[i] = strtol(key_part, NULL, 2);
        key_part = strtok(NULL, " ");
        i += 1;
    }

    return matrix;
}
