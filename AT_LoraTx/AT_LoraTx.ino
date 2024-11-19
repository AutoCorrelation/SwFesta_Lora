#include <SoftwareSerial.h>
#define FLAME_PIN 10
#define TEMPERATURE_PIN A0
#define VELOCITY_PIN 11

// 섹션 열거형 타입 (1 바이트)
enum Section
{
    SECTION_1 = 1,
    SECTION_2,
    SECTION_3,
    SECTION_4
};
// LoRa 모듈 UART rx, tx 핀
// FIXME: PC와의 직렬 통신을 사용하지 않는다면, SoftwareSerial 없이 핀 0, 1을 사용할 수 있습니다.
// 그러면 SoftwareSerial 라이브러리를 제거하고 Serial.begin()으로 통신할 수 있습니다.
SoftwareSerial lora(2, 3);
// 센서 데이터 구조체
struct SensorData
{
    uint8_t section = SECTION_4;
    int32_t speed;
    int32_t temperature;
    int is_fire;
};
// 센서 데이터를 저장할 구조체
SensorData sensor_data;
SensorData getData();
// 센서 데이터를 <16진수 문자열>로 변환 (LoRa 통신 형식)
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
    // 센서 데이터 가져오기
    sensor_data = getData();
    char command[30];

    // 센서 데이터를 LoRa 모듈로 전송
    sprintf(command, "AT+PSEND=%s", data2hex(sensor_data));
    lora.write(command);
    lora.write("\r\n");

    delay(1000);
}

// 함수 정의-----------------------------------
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
