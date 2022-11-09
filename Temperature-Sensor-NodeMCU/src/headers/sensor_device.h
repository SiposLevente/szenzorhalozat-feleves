#define MINUTE 60000

class SensorDevice
{
private:
    char *m_id;
    int m_measurements_per_min;
    float m_current_temp_avg;
    float m_previous_temp_avg;
    float *m_temp_buffer;

    void ResizeBufferSize();

public:
    SensorDevice(char *config);
    ~SensorDevice();
    char *GetId();
    char *ToString();
    void SetId(char *new_id);
};