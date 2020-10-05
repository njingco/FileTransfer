#include "utilities.h"

size_t read_file(FILE* file, char* buffer)
{
    size_t result = 0;
    result = fread(buffer, 1, BUFFER_SIZE, file);
    fprintf(stdout, "result: %lu\n", result);
    return result;
}
