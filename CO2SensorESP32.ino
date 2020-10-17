#define RXD2 16
#define TXD2 17
#define INTERVAL 5000
#define IDX_mhz19   26

long previousMillis = 0;


void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); 
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis < INTERVAL)
   return;
 
  previousMillis = currentMillis;
  Serial.print("Requesting CO2 concentration...");
  int ppm = readCO2();
  Serial.println("  PPM = " + String(ppm));

}

int readCO2() {
  //  From original code https://github.com/jehy/arduino-esp8266-mh-z19-serial
  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  // command to ask for data
  byte response[9]; // for answer

  Serial2.write(cmd, 9); //request PPM CO2

  // The serial stream can get out of sync. The response starts with 0xff, try to resync.
  while (Serial2.available() > 0 && (unsigned char)Serial2.peek() != 0xFF) {
    Serial2.read();
  }
  memset(response, 0, 9);
  Serial2.readBytes(response, 9);

  if (response[1] != 0x86)
  {
    Serial.println("Invalid response from co2 sensor!");
    return -1;
  }

  byte crc = 0;
  for (int i = 1; i < 8; i++) {
    crc += response[i];
  }
  crc = 255 - crc + 1;

  if (response[8] == crc) {
    int responseHigh = (int) response[2];
    int responseLow = (int) response[3];
    int ppm = (256 * responseHigh) + responseLow;
    return ppm;
  } else {
    Serial.println("CRC error!");
    return -1;
  }
}
