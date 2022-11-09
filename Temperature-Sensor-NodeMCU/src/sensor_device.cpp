#ifndef SENSOR_DEVICE_DEFINED
#define SENSOR_DEVICE_DEFINED
#include "headers/sensor_device.h"
#endif

SensorDevice::SensorDevice(char *config)
{
}

void SensorDevice::ResizeBufferSize()
{
    m_temp_buffer = new float[m_measurements_per_min];
}