#include "headers/calulation_modes.h"

float CalculationModes::Average(float *buffer, int size)
{
    float sum = 0.0f;
    for (int i = 0; i < size; i++)
    {
        sum += buffer[i];
    }
    return sum / (float)size;
}

float CalculationModes::Mode(float *buffer, int size)
{
}

float CalculationModes::Median(float *buffer, int size)
{
}