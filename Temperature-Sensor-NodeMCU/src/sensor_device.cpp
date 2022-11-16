#include "headers/sensor_device.h"

inline char *ToString(CalculationMode mode)
{
    switch (mode)
    {
    case Average:
        return "average";
        break;
    case Mode:
        return "mode";
        break;
    case Median:
        return "median";
        break;
    default:
        return "[Unknown enum type]";
        break;
    }
}

SensorDevice::SensorDevice()
{
    Serial.begin(115200);
    Serial.println("Starting setup!");
    this->m_id = UUIDGenerator::GenerateUUID();
    Serial.print("Generated UUID: ");
    Serial.println(this->m_id);
    this->m_temp_buffer = nullptr;
    this->m_measurements_per_min = DEFAULT_MEASUREMENT_PER_MINUTE_VALUE;
    this->m_temp_avg = -1;
    this->m_last_data_time = millis();
    this->m_calc_mode = CalculationMode::Average;
    this->m_calculation_mode_function = CalculationModes::Mode;
    this->ResizeBuffer();
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
    Serial.print("Processing data...");
    this->m_temp_avg = (*this->m_calculation_mode_function)(this->m_temp_buffer, this->m_measurements_per_min);
    Serial.println("done!");
}

int SensorDevice::GetMeasurementsPerMinute()
{
    return this->m_measurements_per_min;
}

char *SensorDevice::GetCalculationMode()
{
    return ToString(this->m_calc_mode);
}

char *SensorDevice::GetID()
{
    return this->m_id;
}

float SensorDevice::GetData()
{
    Serial.println("Getting processed data!");
    this->ResetBuffer();
    this->m_last_data_time = millis();
    return this->m_temp_avg;
}

void SensorDevice::ResetBuffer()
{
    this->ResizeBuffer();
}

void SensorDevice::ResizeBuffer()
{
    Serial.println("Resizing buffer!");
    float *tmp_buffer = new float[m_measurements_per_min];
    Serial.println("New tmp buffer created!");
    if (this->m_temp_buffer)
    {
        Serial.println("Buffer is deleted created!");
        delete[] this->m_temp_buffer;
    }

    this->m_temp_buffer = tmp_buffer;
    for (int i = 0; i < this->m_measurements_per_min; i++)
    {
        this->m_temp_buffer[i] = 0.0f;
    }
    this->m_data_pointer = 0;
}

void SensorDevice::Sleep()
{
    // Divides the minute into the number of required measurements, then divides it by to prevent too long waits between measurements.
    int time_to_sleep = MINUTE / this->m_measurements_per_min;
    Serial.printf("Sleeping for %d ms\n", time_to_sleep);
    delay(time_to_sleep);
}

bool SensorDevice::canCollectData()
{
    bool result = this->m_data_pointer < this->m_measurements_per_min;
    Serial.printf("Data can be collected: %d\n", result);
    return result;
}

bool SensorDevice::canSendData()
{
    unsigned long time_delta = millis() - this->m_last_data_time;
    bool result = time_delta >= MINUTE && this->m_data_pointer == this->m_measurements_per_min;
    Serial.printf("Data can be sent: %d\n", result);
    Serial.printf("Time delta: %d\n", time_delta);
    return result;
}

void SensorDevice::CollecData()
{
    Serial.print("Collecting data...");
    int analog_in = analogRead(DATA_PIN);
    Serial.print("got analog data...");
    float data = this->ConvertValueToDataEntry(analog_in);
    Serial.print("Data collected: ");
    Serial.println(data);
    Serial.print("Data written to buffer at position: ");
    Serial.println(this->m_data_pointer);
    this->m_temp_buffer[this->m_data_pointer] = data;
    this->m_data_pointer++;
    Serial.print("Pointer incremented! New value: ");
    Serial.println(this->m_data_pointer);
}

float SensorDevice::ConvertValueToDataEntry(int value)
{
    Serial.println("data is converted");
    // TODO convert function to real data conversion
    return (float)value;
}

void SensorDevice::SetMeasurementPerMinute(int new_measurement_per_minute)
{
    this->m_measurements_per_min = new_measurement_per_minute;
}

void SensorDevice::SetCalculationMode(CalculationMode calc_mode)
{
    this->m_calc_mode = calc_mode;
    switch (calc_mode)
    {
    case CalculationMode::Average:
        this->m_calculation_mode_function = CalculationModes::Average;
        break;

    case CalculationMode::Median:
        this->m_calculation_mode_function = CalculationModes::Median;
        break;

    case CalculationMode::Mode:
        this->m_calculation_mode_function = CalculationModes::Mode;
        break;
    }
}