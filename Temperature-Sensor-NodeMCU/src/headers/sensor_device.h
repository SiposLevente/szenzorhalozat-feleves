#include <Arduino.h>

#include "headers/uuid_generator.h"
#include "headers/calulation_modes.h"

/// A minute in miliseconds.
#define MINUTE 60000

/// Default value for measurement number per minute.
#define DEFAULT_MEASUREMENT_PER_MINUTE_VALUE 60

/// Data pin, this is where the sensor is connected.
#define DATA_PIN A16

/// Standard "average" text value for calculation mode.
#define CALCULATION_MODE_AVERAGE_TYPE_TEXT "average"
/// Standard "median" text value for calculation mode.
#define CALCULATION_MODE_MEDIAN_TYPE_TEXT "median"
/// Standard "mode" text value for calculation mode.
#define CALCULATION_MODE_MODE_TYPE_TEXT "mode"

/// Holds the possible calculation modes.
enum CalculationMode
{
    Average,
    Median,
    Mode,
};

/// Handles the data processing and collecting of the connected sensor.
class SensorDevice
{
private:
    /// ID of the sensor device.
    char *m_id;
    /// Indicates measurements done each minute.
    int m_measurements_per_min;
    /// Indicates the point where the data will be written in the temperature buffer.
    int m_data_pointer;
    /// Holds the processed data once the process is buffered.
    float m_temp_avg;
    /// Holds the collected temperature data.
    float *m_temp_buffer;
    /// Time of the last processed data.
    unsigned long m_last_data_time;

    /// Function used for processig the collected data.
    float (*m_calculation_mode_function)(float *buffer, int size);
    /// Indicates current data procession mode.
    CalculationMode m_calc_mode;

    /// Clears and resizes the temperature buffer to the size of specified collection.
    void ResizeBuffer();
    /// Does the same thing as ResizeBuffer. Different function naming is for the differentiation in functionality.
    void ResetBuffer();
    /// Converts collected data from the sensor into useable data.
    float ConvertValueToDataEntry(int value);

public:
    /// Default constructor.
    SensorDevice();
    /// Default destructor.
    ~SensorDevice();

    /// Determines if data is ready to be collected.
    bool canCollectData();
    /// Determines if data is ready to be sent.
    bool canSendData();

    /// Delays the program according to the value of measurements per minute.
    void Sleep();
    /// Collects data from the sensor.
    void CollecData();
    /// Processes data collected from the sensor.
    void ProcessData();
    /// Changes the value of measurements per minute.
    void SetMeasurementPerMinute(int new_measurement_per_minute);
    /// Changes the value of calculation mode.
    void SetCalculationMode(CalculationMode calc_mode);
    /// Gets the value of measurements per minute.
    int GetMeasurementsPerMinute();
    /// Gets the value of calculation mode.
    char *GetCalculationMode();
    /// Gets the id of the device.
    char *GetID();
    /// Returns the processed data. Resets the buffer!
    float GetData();
};