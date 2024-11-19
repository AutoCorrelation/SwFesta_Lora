#include <SoftwareSerial.h>
#define FLAME_PIN 10
#define TEMPERATURE_PIN A0
#define VELOCITY_PIN 11

//Section enum type (1 byte)
enum Section
{
    SECTION_1 = 1,
    SECTION_2,
    SECTION_3,
    SECTION_4
};
//Lora module UART rx, tx pin
//FIXME: If don't use serial communication with pc, you can use pin 0, 1 without SoftwareSerial
//Then you can remove SoftwareSerial library and communicate with Serial.begin()
SoftwareSerial lora(2, 3);
//Sensor data structure 
struct SensorData
{
    uint8_t section = SECTION_4;
    int32_t speed;
    int32_t temperature;
    int is_fire;
};
//Structure to store sensor data
SensorData sensor_data;
SensorData getData();
//Convert sensor data to <hex string> (Lora communication format)
char *data2hex(SensorData sensor_data);

void setup()
{
    pinMode(FLAME_PIN, INPUT);
    Serial.begin(57600);
    lora.begin(57600);
    delay(10);
}

void loop()
{
    //Get sensor data
    sensor_data = getData();
    char command[30];

    //Send sensor data to Lora module
    sprintf(command, "AT+PSEND=%s", data2hex(sensor_data));
    lora.write(command);
    lora.write("\r\n");

    delay(1000);
}

//define function-----------------------------------
char *data2hex(SensorData _data)
{
    static char hex_data[23];

    char hex_section[3];
    sprintf(hex_section, "%02x", _data.section);

    char hex_speed[9];
    sprintf(hex_speed, "%08x", _data.speed);

    char hex_temperature[9];
    sprintf(hex_temperature, "%08x", _data.temperature);

    char hex_is_fire[3];
    sprintf(hex_is_fire, "%02x", _data.is_fire);

    sprintf(hex_data, "%s%s%s%s", hex_section, hex_speed, hex_temperature, hex_is_fire);

    return hex_data;
}

SensorData getData()
{
    SensorData _data;

    _data.is_fire = digitalRead(FLAME_PIN);
    _data.temperature = (analogRead(TEMPERATURE_PIN) * 0.48828125);
    _data.speed = (1e6 / (pulseIn(VELOCITY_PIN,HIGH)+pulseIn(VELOCITY_PIN,LOW))) / 44.0;

    return _data;
}
