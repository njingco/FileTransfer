#include "utilities.h"

int open_file(FILE* file, char* filepath)
{
    int result = 0;
    if((file = fopen(filepath, "r")) == NULL)
    {
        result = -1;
    }
    return result;
}
