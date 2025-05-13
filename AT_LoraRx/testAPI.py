import requests


# API class to send data to the API
class API:
    def __init__(self, url):
        self.url = url
        self.headers = {"Content-Type": "application/json"}

    def send_data(self, payload):
        try:
            response = requests.post(self.url, json=payload, headers=self.headers)
            print(f"HTTP Response: {response.status_code}, {response.text}")
        except requests.exceptions.RequestException as e:
            print(f"HTTP Request failed: {e}")

    def transform_data(self, received_hex_data):
        try:
            received_section = int(received_hex_data[0:2], 16)
            received_speed = int(
                received_hex_data[2:10], 16
            )  # 16진수 문자열을 2의 보수로 변환하여 음수 포함 정수로 변환
            received_temperature = int(received_hex_data[10:18], 16)
            if received_temperature & 0x80000000:
                received_temperature -= 0x100000000
            received_is_fire = bool(int(received_hex_data[-2:], 16))

            # print(f"Section: {received_section}")
            # print(f"Speed: {received_speed}")
            # print(f"Temperature: {received_temperature}")
            # print(f"Is Fire: {received_is_fire}")

            return {
                "section": received_section,
                "speed": received_speed,
                "temperature": received_temperature,
                "is_fire": received_is_fire,
            }
        except ValueError as e:
            print(f"Data transformation failed: {e}")
            return None


if __name__ == "__main__":
    # 예제 사용법
    api = API("https://www.kgu-shiphub.com/api/sensor")

    # 데이터 준비
    section = 2             # enum (1 byte)
    speed = 100             # integer (4 bytes)
    temperature = +20       # integer (4 bytes, 음수 포함)
    is_fire = False         # boolean (1 byte)

    # 데이터를 16진수 문자열로 변환
    hex_section = f"{section:02x}"
    hex_speed = f"{speed:08x}"
    # 음수 온도를 2의 보수로 변환하여 16진수로 표현
    hex_temperature = f"{temperature & 0xFFFFFFFF:08x}"
    hex_is_fire = f"{int(is_fire):02x}"

    # 16진수 문자열을 하나로 결합
    hex_data = hex_section + hex_speed + hex_temperature + hex_is_fire
    print(f"Hex Data: {hex_data}")

    transformed_data = api.transform_data(hex_data)
    print(transformed_data)
