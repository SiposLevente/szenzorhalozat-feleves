#ifndef SENSOR_DEVICE_DEFINED
#define SENSOR_DEVICE_DEFINED
#include "headers/sensor_device.h"
#endif

SensorDevice::SensorDevice(char *config)
{
}

SensorDevice::~SensorDevice()
{
}

void SensorDevice::SetBufferSize()
{
    int new_size = MINUTE / m_measurement_intervall;
    if (m_temp_buffer != nullptr)
    {
        delete[] m_temp_buffer;
    }
    *m_temp_buffer = new float(new_size);
}