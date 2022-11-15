#include <Arduino.h>

#include "headers/uuid_generator.h"
#include "headers/calulation_modes.h"

#define MINUTE 60000
#define DEFAULT_MEASUREMENT_PER_MINUTE_VALUE 60

#define DATA_PIN A16

#define CALCULATION_MODE_AVERAGE_TYPE_TEXT "average"
#define CALCULATION_MODE_MEDIAN_TYPE_TEXT "median"
#define CALCULATION_MODE_MODE_TYPE_TEXT "mode"

enum CalculationMode
{
    Average,
    Median,
    Mode,
};

class SensorDevice
{
private:
    char *m_id;
    bool m_buffer_is_ready;
    bool m_first_data;
    int m_measurements_per_min;
    int m_data_pointer;
    float m_temp_avg;
    float *m_temp_buffer;
    unsigned long m_last_data_time;

    float (*m_calculation_mode_function)(float *buffer, int size);

    CalculationMode m_calc_mode;

    void ResizeBuffer();
    void ResetBuffer();
    float ConvertValueToDataEntry(int value);

public:
    SensorDevice();
    ~SensorDevice();

    bool canCollectData();
    bool canSendData();

    void Sleep();
    void CollecData();
    void ProcessData();
    void SetMeasurementPerMinute(int new_measurement_per_minute);
    void SetCalculationMode(CalculationMode calc_mode);
    float GetData();
};