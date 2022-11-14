#include "headers/sensor_device.h"

// Constructor
SensorDevice::SensorDevice()
{
    Serial.begin(115200);
    this->m_id = UUIDGenerator::generate_uuid();
    this->m_buffer_is_ready = false;
    float *m_temp_buffer = nullptr;
    this->m_measurements_per_min = DEFAULT_MEASUREMENT_PER_MINUTE_VALUE;
    this->m_temp_avg = -1;
    this->m_last_data_time = 0;
    this->m_calc_mode = CalculationMode::Average;
    this->ResizeBuffer();
    this->m_ble_client = BLEClient();
    Serial.println("Setup done!");
}

SensorDevice::~SensorDevice()
{
    if (this->m_temp_buffer)
    {
        delete[] m_temp_buffer;
    }
}

void SensorDevice::ProcessData()
{
    this->m_temp_avg = this->m_calculation_mode_function(this->m_temp_buffer, this->m_measurements_per_min);
}

void SensorDevice::ResizeBuffer()
{
    float *tmp_buffer = new float[m_measurements_per_min];
    if (this->m_temp_buffer)
    {
        delete[] this->m_temp_buffer;
    }

    m_temp_buffer = tmp_buffer;
    for (int i = 0; i < this->m_measurements_per_min; i++)
    {
        this->m_temp_buffer[i] = 0.0f;
    }
    m_data_pointer = 0;
}

void SensorDevice::Sleep()
{
    // Divides the minute into the number of required measurements, then divides it by to prevent too long waits between measurements.
    int time_to_sleep = MINUTE / this->m_measurements_per_min / 2;
    delay(time_to_sleep);
}

bool SensorDevice::isConnectedToGateway()
{
    return this->m_ble_client.isConnected();
}

bool SensorDevice::canCollectData()
{
    return this->m_data_pointer < this->m_measurements_per_min;
}

bool SensorDevice::canSendData()
{
    unsigned long time_delta = millis() - this->m_last_data_time;
    return time_delta >= MINUTE && this->m_data_pointer == this->m_measurements_per_min;
}

void SensorDevice::CollecData()
{
    float data = this->ConvertValueToDataEntry(analogRead(DATA_PIN));
    this->m_temp_buffer[this->m_data_pointer] = data;
    this->m_data_pointer++;
}

float SensorDevice::ConvertValueToDataEntry(uint16_t value)
{
    // TODO convert function to real data conversion
    return (float)value;
}