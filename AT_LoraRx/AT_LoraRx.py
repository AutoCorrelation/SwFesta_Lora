import serial
import time
from testAPI import API

# 시리얼 통신 설정
ser = serial.Serial(
    port="/dev/ttyS0",
    baudrate=57600,
    timeout=1,
)

# AT 커맨드 전송 함수
def send_at_command(command):
    ser.write(f"{command}\r\n".encode())
    time.sleep(1)

# 메인 함수
def main():
    # 시리얼 포트 열기 Lora 모듈이 연결된 포트
    interval = 5
    if ser.isOpen():
        print("Serial port opened successfully")
    else:
        print("Failed to open serial port")
    time.sleep(0.5)

    # AT 커맨드 전송 (수신 모드 설정)
    send_at_command("AT+PRECV=65534")
    # API endpoint 설정
    api = API("https://www.kgu-shiphub.com/api/sensor")

    # 데이터 수신 및 전송 (무한루프)
    while True:
        # 데이터 수신
        if ser.in_waiting > 0:
            data = ser.readline().decode()
            received_data = data.split(":")[-1].strip()
        else: # 데이터가 없을 경우
            print("No data received")
            continue
        # 데이터 전송 (API endpoint)
        payload = api.transform_data(received_data)
        api.send_data(payload)

        time.sleep(interval)

# 파일이 직접 실행될 경우 main 함수 실행
if __name__ == "__main__":
    main()
