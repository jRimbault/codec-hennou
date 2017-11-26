/**
 * @author: jRimbault
 * @date:   2017-11-21
 * @last modified by:   jRimbault
 * @last modified time: 2017-11-26
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
char* get_matrix(char* filename)
{
    FILE* keyfile = fopen(filename, "r");
    if (!keyfile) {
        printf("Couldn't access key file. Use --help.\n");
        exit(10);
    }

    fseek(keyfile, 5, SEEK_SET);
    char string_matrix[35];
    if(!fread(string_matrix, 35, 1, keyfile)) { exit(11);}
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

/**
 * Returns directly usable matrix
 */
char* matrix(char* filename)
{
    unsigned char* matrix = get_matrix(filename);
    char* m = calloc(16, sizeof(char));
    m[15]  = 0;
    m[1]  = matrix[3];
    m[2]  = matrix[2];
    m[3]  = matrix[2] ^ matrix[3];
    m[4]  = matrix[1];
    m[5]  = matrix[1] ^ matrix[3];
    m[6]  = matrix[1] ^ matrix[2];
    m[7]  = matrix[1] ^ matrix[2] ^ matrix[3];
    m[8]  = matrix[0];
    m[9]  = matrix[0] ^ matrix[3];
    m[10] = matrix[0] ^ matrix[2];
    m[11] = matrix[0] ^ matrix[2] ^ matrix[3];
    m[12] = matrix[0] ^ matrix[1];
    m[13] = matrix[0] ^ matrix[1] ^ matrix[3];
    m[14] = matrix[0] ^ matrix[1] ^ matrix[2];
    m[0] = matrix[0] ^ matrix[1] ^ matrix[2] ^ matrix[3];
    return m;
}
