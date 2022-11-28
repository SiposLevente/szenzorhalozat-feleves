#include "headers/id_generator.h"
#include <cstdlib>

/// Generates a UUID.
char *IDGenerator::GenerateID()
{
    char characters[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    static char buf[8] = {0};
    for (int i = 0; i < 8; ++i)
    {
        buf[i] = characters[std::rand() % 16];
    }

    return buf;
}