#include "headers/sensor_device.h"

SensorDevice device = SensorDevice();

void setup()
{
}

void loop()
{
  if (device.canCollectData())
  {
    device.CollecData();
  }

  if (device.canSendData())
  {
    device.ProcessData();
    device.GetData();
  }
  device.Sleep();
}

void SetDeviceMeasurementPerMinute(int new_measurement_per_minute)
{
  device.SetMeasurementPerMinute(new_measurement_per_minute);
}

void SetCalculationMode(char *new_calc_mode)
{
  CalculationMode calc_mode;
  if (new_calc_mode == CALCULATION_MODE_AVERAGE_TYPE_TEXT)
  {
    calc_mode = CalculationMode::Average;
  }
  else if (new_calc_mode == CALCULATION_MODE_MEDIAN_TYPE_TEXT)
  {
    calc_mode = CalculationMode::Median;
  }
  else
  {
    calc_mode = CalculationMode::Mode;
  }

  device.SetCalculationMode(calc_mode);
}
