# Sensor NodeMCU
## Uploading to a microcontroller
The project was created using VSCode with the extension Platform.io. To upload using regular Arduino IDE copy the content of the "src", and "src/headers/" directories into a folder named "main", and rename the "main.cpp" into "main.ino".

# BLE communination

## Service UUID
The device searches for a server with the service uuid:
`e414d508-cb57-40d5-bac5-0d237078f34f`

## Characteristics

### Temperature
For the temperature value publishing the device uses the UUID of:
`b17516f7-0b89-4ade-9a84-0b849b3b593b`

This value updates minimum every minute, when the device collected enough information.

The device publishes the temperature data with the following structure:

`{id};data`

e.g.: `df694488;21.32` meaning the device with the if of `df694488` sent a temperature data of `21.32` degrees Celcius.

### Calculation mode
For setting calculation mode, the device uses the UUID of:
`b17516f7-0b89-4ade-9a84-0b849b3b593c`

Valid values are:
- `average`
- `mode`
- `median`

When setting the characteristic's value the device changes the mode of processing the collected data.
Changing mode doesn't affect the collected data.

To send data to a specific device the data needs to be sent with the structure mentioned in the [Temperature](#Temperature) segment.
E.g: `df694488;average`. This sets the device with the id of `df694488` to the calculation mode of `average`

### Measurement per minute
For setting the measurement per minute value, the device uses the UUID of `b17516f7-0b89-4ade-9a84-0b849b3b593d`

Valid measurement per minute range is 1 ≤ x ≤ 120000

When setting the value of measurements per minute the buffer resets.

To send data to a specific device the data needs to be sent with the structure mentioned in the [Temperature](#Temperature) segment.
E.g: `df694488;30`. This sets the measurement per minute of device with the id of `df694488` to `30`
