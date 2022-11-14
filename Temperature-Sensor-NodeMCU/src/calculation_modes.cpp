#include "headers/calulation_modes.h"
#include <bits/stdc++.h>
using namespace std;

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
    sort(buffer, buffer + size);

    int number = buffer[0];
    int mode = number;
    int count = 1;
    int countMode = 1;

    for (int i = 1; i < size; i++)
    {
        if (buffer[i] == number)
        {
            ++count;
        }
        else
        {
            if (count > countMode)
            {
                countMode = count;
                mode = number;
            }
            count = 1;
            number = buffer[i];
        }
    }
    return mode;
}

float CalculationModes::Median(float *buffer, int size)
{
    sort(buffer, buffer + size);
    if (size % 2 == 0)
    {
        return buffer[size / 2];
    }
    else
    {
        float num1 = buffer[size / 2];
        float num2 = buffer[(size / 2) + 1];
        return (num1 + num2) / 2.0;
    }
}