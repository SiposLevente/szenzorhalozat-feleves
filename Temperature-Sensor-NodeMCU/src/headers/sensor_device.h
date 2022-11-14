#include <BLEClient.h>
#include <Arduino.h>

#include "headers/uuid_generator.h"
#include "headers/calulation_modes.h"

#define CHARACTERISTIC_UUID "1adab8b2-100d-4dbd-9880-7ce062b02bd5"
#define SERVICE_UUID "c8de7840-55e8-4a79-8286-0db52a0f1a94"
#define GATEWAY_NAME "Gateway_ESP32"
#define MINUTE 60000
#define DEFAULT_MEASUREMENT_PER_MINUTE_VALUE 60

#define DATA_PIN A0

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
    int m_measurements_per_min;
    int m_data_pointer;
    float m_temp_avg;
    float *m_temp_buffer;
    unsigned long m_last_data_time;

    CalculationMode m_calc_mode;
    float(*m_calculation_mode_function)(float*, int);

    BLEClient m_ble_client;

    void ConnectToServer();
    void ResizeBuffer();
    float ConvertValueToDataEntry(uint16_t value);

public:
    SensorDevice();
    ~SensorDevice();
    char *GetId();
    bool canCollectData();
    bool canSendData();
    bool isConnectedToGateway();
    void CollecData();
    void ProcessData();
    void Sleep();
};