#define MINUTE 60000

class SensorDevice
{
private:
    char* m_id;
    float m_current_temp_avg;
    float m_previous_temp_avg;
    int m_measurement_intervall;
    float* m_temp_buffer[0];
    void SetBufferSize();
public:
    char* GetId();
    void SetId(char* new_id);
    char* ToString();
    SensorDevice(char* config);
    ~SensorDevice();
};

