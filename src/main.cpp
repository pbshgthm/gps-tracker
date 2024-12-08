#include <HardwareSerial.h>
#include <TinyGPS++.h>

#define RXD2 16 // Pin for RX2
#define TXD2 17 // Pin for TX2
#define BAUD_RATE 9600
#define SERIAL_BUFFER_SIZE 2048 // Increased buffer size for large messages

HardwareSerial gpsSerial(2);
TinyGPSPlus gps;

void sendUBX(uint8_t *msg, uint8_t len);
void parseUBX(uint8_t *buffer, uint32_t len);
bool checkUBXChecksum(uint8_t *buffer, uint32_t len);
void queryFirmwareVersion();
void queryPowerMode();
void readGPS();
void setPowerMode();
void printHexBuffer(uint8_t *buffer, uint32_t len);
void calculateUBXChecksum(uint8_t *msg, uint8_t len);

bool isConfigured = false;

///

unsigned long startTime;
unsigned long lastDebugTime = 0;
unsigned long lastResetTime = 0;
const unsigned long DEBUG_INTERVAL = 1000;

///

uint8_t ubxQueryFirmware[] = {
    0xB5, 0x62, // UBX Header
    0x0A, 0x04, // Message Class and ID (MON-VER)
    0x00, 0x00, // Payload length (0 for MON-VER request)
    0x0E, 0x34  // Checksum (calculated manually)
};

uint8_t ubxSetRXM[] = {
    0xB5, 0x62, // UBX Header
    0x06, 0x11, // CFG-RXM
    0x02, 0x00, // Payload length = 2 bytes
    0x00,       // Reserved
    0x01,       // LPMode = 0 (disabled)
    0x8C, 0xBE  // Checksum (calculated manually)
};

uint8_t ubxSetPM2[] = {
    0xB5, 0x62, // UBX Header
    0x06, 0x3B, // CFG-PM2 Class and ID
    0x30, 0x00, // Payload length = 48 bytes

    0x02,       // Version = 0x02
    0x00,       // Reserved1
    0x00, 0x00, // Max startup state duration = 0 (disabled)

    0x01, 0x00, 0x00, 0x00, // Flags: PSMOO enabled, no EXTINT influence

    0x60, 0xEA, 0x00, 0x00, // Update period = 60000 ms (60 seconds)
    0x40, 0x9C, 0x00, 0x00, // Search period = 40000 ms (40 seconds)
    0x00, 0x00, 0x00, 0x00, // Grid offset = 0 ms

    0x88, 0x13, // On time = 5000 ms (5 seconds)
    0x00, 0x00, // Minimum acquisition time = 0 ms (disabled)

    0x00, 0x00, 0x00, 0x00, // Reserved (20 bytes)
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, // EXTINT inactivity timeout = 0 (disabled)
    0x00, 0x00              // Checksum (calculated dynamically)
};

uint8_t ubxSaveConfig[] = {
    0xB5, 0x62,             // UBX Header
    0x06, 0x09,             // Message Class and ID (CFG-CFG)
    0x0D, 0x00,             // Payload length = 13 bytes
    0x00, 0x00, 0x00, 0x00, // clearMask (no sections cleared)
    0x10, 0x00, 0x00, 0x00, // saveMask (bit 4 for RXM, adjust for PM2 if necessary)
    0x00, 0x00, 0x00, 0x00, // loadMask (no sections loaded)
    0x01,                   // deviceMask (optional, set to 0 if not used)
    0x23, 0x2C              // Checksum (calculated manually)
};

uint8_t ubxQueryPM2[] = {
    0xB5, 0x62, // UBX Header
    0x06, 0x3B, // CFG-PM2
    0x00, 0x00, // Payload length = 0
    0x8C, 0xBE  // Checksum (calculated manually)
};

void queryFirmwareVersion()
{
  Serial.println("Querying firmware version...");
  sendUBX(ubxQueryFirmware, sizeof(ubxQueryFirmware));
}

void queryPowerMode()
{
  Serial.println("Querying current power mode...");
  sendUBX(ubxQueryPM2, sizeof(ubxQueryPM2));
}

void setPowerMode()
{
  Serial.println("Setting GPS to Power Save Mode using CFG-PMS...");
  sendUBX(ubxSetRXM, sizeof(ubxSetRXM));
  delay(1000);
  sendUBX(ubxSetPM2, sizeof(ubxSetPM2));
  delay(1000);
  sendUBX(ubxSaveConfig, sizeof(ubxSaveConfig));
  delay(1000);
}

void calculateUBXChecksum(uint8_t *msg, uint8_t len)
{
  uint8_t ckA = 0, ckB = 0;
  for (uint8_t i = 2; i < len - 2; i++)
  {
    ckA += msg[i];
    ckB += ckA;
  }
  msg[len - 2] = ckA;
  msg[len - 1] = ckB;
}

void sendUBX(uint8_t *msg, uint8_t len)
{
  calculateUBXChecksum(msg, len);
  Serial.println("Sending UBX message:");
  printHexBuffer(msg, len);
  gpsSerial.write(msg, len);
}

void printHexBuffer(uint8_t *buffer, uint32_t len)
{
  for (uint32_t i = 0; i < len; i++)
  {
    if (i > 0 && i % 16 == 0)
      Serial.println();
    Serial.printf("0x%02X ", buffer[i]);
  }
  Serial.println();
}

void readGPS()
{
  static uint8_t buffer[SERIAL_BUFFER_SIZE];
  static uint32_t bufferIndex = 0;
  static bool isNMEA = false;
  static uint32_t messageStartIndex = 0;
  static bool messageInProgress = false;

  while (gpsSerial.available())
  {
    uint8_t byte = gpsSerial.read();

    // Handle NMEA messages
    if (byte == '$')
    {
      isNMEA = true;
      continue;
    }
    if (byte == '\n' && isNMEA)
    {
      isNMEA = false;
      continue;
    }
    if (isNMEA)
      continue;

    // Store byte in buffer
    if (bufferIndex < SERIAL_BUFFER_SIZE)
    {
      buffer[bufferIndex++] = byte;
    }
    else
    {
      Serial.println("Buffer overflow! Resetting...");
      bufferIndex = 0;
      messageInProgress = false;
      continue;
    }

    // Look for UBX message start
    if (!messageInProgress && bufferIndex >= 2)
    {
      if (buffer[bufferIndex - 2] == 0xB5 && buffer[bufferIndex - 1] == 0x62)
      {
        messageInProgress = true;
        messageStartIndex = bufferIndex - 2;
        Serial.println("\nFound UBX message start");
      }
    }

    // Process message if we have enough bytes
    if (messageInProgress && (bufferIndex - messageStartIndex) >= 6)
    {
      uint32_t payloadLength = buffer[messageStartIndex + 4] |
                               (buffer[messageStartIndex + 5] << 8);
      uint32_t expectedLength = payloadLength + 8; // Header(2) + Class/ID(2) + Length(2) + Payload(n) + Checksum(2)

      if ((bufferIndex - messageStartIndex) >= expectedLength)
      {
        Serial.println("Complete message received:");
        printHexBuffer(&buffer[messageStartIndex], expectedLength);

        if (checkUBXChecksum(&buffer[messageStartIndex], expectedLength))
        {
          Serial.println("Checksum valid - Processing message");
          parseUBX(&buffer[messageStartIndex], expectedLength);
        }
        else
        {
          Serial.println("Checksum invalid!");
        }

        // Move any remaining bytes to start of buffer
        if (bufferIndex > (messageStartIndex + expectedLength))
        {
          memmove(buffer,
                  &buffer[messageStartIndex + expectedLength],
                  bufferIndex - (messageStartIndex + expectedLength));
          bufferIndex -= (messageStartIndex + expectedLength);
        }
        else
        {
          bufferIndex = 0;
        }
        messageInProgress = false;
      }
    }
  }
}

bool checkUBXChecksum(uint8_t *buffer, uint32_t len)
{
  uint8_t ckA = 0, ckB = 0;

  // Calculate checksum
  for (uint32_t i = 2; i < len - 2; i++)
  {
    ckA += buffer[i];
    ckB += ckA;
  }

  // Debug output
  // Serial.printf("Calculated checksum: CK_A=0x%02X, CK_B=0x%02X\n", ckA, ckB);
  // Serial.printf("Received checksum:   CK_A=0x%02X, CK_B=0x%02X\n",
  //               buffer[len - 2], buffer[len - 1]);

  return (ckA == buffer[len - 2] && ckB == buffer[len - 1]);
}

void parseUBX(uint8_t *buffer, uint32_t len)
{
  uint16_t payloadLength = buffer[4] | (buffer[5] << 8);
  uint8_t classId = buffer[2];
  uint8_t messageId = buffer[3];

  Serial.printf("UBX Class: 0x%02X, ID: 0x%02X, Payload Length: %d\n",
                classId, messageId, payloadLength);

  if (classId == 0x05)
  { // ACK class
    if (messageId == 0x01)
    { // ACK-ACK
      uint8_t ackClass = buffer[6];
      uint8_t ackId = buffer[7];
      if (ackClass == 0x06 && ackId == 0x86)
      {
        Serial.println("CFG-PMS acknowledged (ACK-ACK)");
      }
      else
      {
        Serial.printf("Acknowledged message: Class=0x%02X, ID=0x%02X\n", ackClass, ackId);
      }
    }
    else if (messageId == 0x00)
    { // ACK-NAK
      uint8_t nakClass = buffer[6];
      uint8_t nakId = buffer[7];
      if (nakClass == 0x06 && nakId == 0x86)
      {
        Serial.println("CFG-PMS rejected (ACK-NAK)");
      }
      else
      {
        Serial.printf("Rejected message: Class=0x%02X, ID=0x%02X\n", nakClass, nakId);
      }
    }
  }
  else if (classId == 0x0A && messageId == 0x04)
  { // MON-VER response
    Serial.println("Firmware version info received:");
    Serial.write(&buffer[6], payloadLength); // Write payload as ASCII
    Serial.println();
  }
  else if (classId == 0x06 && (messageId == 0x86 || messageId == 0x11 || messageId == 0x3B))
  { // CFG-PMS response
    Serial.println("Power mode settings received:");
    isConfigured = true;
    if (payloadLength >= 1)
    {
      uint8_t pmsMode = buffer[7];
      switch (pmsMode)
      {
      case 0x00:
        Serial.println("Power mode: Full Power");
        break;
      case 0x01:
        Serial.println("Power mode: Balanced");
        break;
      case 0x02:
        Serial.println("Power mode: Interval");
        break;
      case 0x03:
        Serial.println("Power mode: Aggressive 1hz");
        break;

      default:
        Serial.printf("Power mode: Unknown (0x%02X)\n", pmsMode);
      }
    }
  }
  Serial.printf("\n\n\n\n\n");
}

enum State
{
  IDLE,
  QUERY_FIRMWARE,
  SET_POWER_MODE,
  QUERY_POWER_MODE
};

State currentState = IDLE;
unsigned long lastCommandTime = 0;
const unsigned long commandInterval = 2000;

void setup()
{
  Serial.begin(115200);
  gpsSerial.setRxBufferSize(SERIAL_BUFFER_SIZE);
  gpsSerial.begin(BAUD_RATE, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Starting GPS communication...");
  currentState = QUERY_FIRMWARE;
}

void loop()
{

  if (!isConfigured)
  {
    readGPS();
  }
  else
  {
    while (gpsSerial.available() > 0)
    {
      gps.encode(gpsSerial.read());
    }

    if (millis() - lastDebugTime > DEBUG_INTERVAL)
    {
      lastDebugTime = millis();

      Serial.println("\n=== GPS Status ===");
      Serial.print("Time since start: ");
      Serial.print((millis() - startTime) / 1000);
      Serial.println(" seconds");

      // GPS Quality Indicators
      Serial.println("\nSignal Quality:");
      Serial.print("Satellites: ");
      if (gps.satellites.isValid())
      {
        Serial.println(gps.satellites.value());
      }
      else
      {
        Serial.println("No Data");
      }

      Serial.print("HDOP (lower is better): ");
      if (gps.hdop.isValid())
      {
        Serial.println(gps.hdop.value());
      }
      else
      {
        Serial.println("No Data");
      }

      // Location Data
      Serial.println("\nLocation Data:");
      if (gps.location.isValid())
      {
        Serial.print("Latitude: ");
        Serial.println(gps.location.lat(), 6);
        Serial.print("Longitude: ");
        Serial.println(gps.location.lng(), 6);
        Serial.print("Age of fix: ");
        Serial.print(gps.location.age());
        Serial.println("ms");
      }
      else
      {
        Serial.println("Location: Waiting for fix...");
        Serial.print("Characters processed: ");
        Serial.println(gps.charsProcessed());
        Serial.print("Sentences with fix: ");
        Serial.println(gps.sentencesWithFix());
      }

      // Altitude
      if (gps.altitude.isValid())
      {
        Serial.print("Altitude: ");
        Serial.print(gps.altitude.meters());
        Serial.println(" meters");
      }

      // Time and Date
      if (gps.time.isValid() && gps.date.isValid())
      {
        Serial.print("UTC DateTime: ");
        Serial.print(gps.date.year());
        Serial.print("/");
        Serial.print(gps.date.month());
        Serial.print("/");
        Serial.print(gps.date.day());
        Serial.print(" ");
        Serial.print(gps.time.hour());
        Serial.print(":");
        Serial.print(gps.time.minute());
        Serial.print(":");
        Serial.println(gps.time.second());
      }

      Serial.println("====================");
    }
    // if (millis() - lastResetTime > 66000)
    // {
    //   Serial.println("Resetting GPS...");
    //   currentState = SET_POWER_MODE;
    //   isConfigured = false;
    //   lastResetTime = millis();
    // }
    return;
  }

  unsigned long currentTime = millis();
  if (currentTime - lastCommandTime >= commandInterval)
  {
    switch (currentState)
    {
    case QUERY_FIRMWARE:
      queryFirmwareVersion();
      currentState = SET_POWER_MODE;
      lastCommandTime = currentTime;
      break;

    case SET_POWER_MODE:
      setPowerMode();
      currentState = QUERY_POWER_MODE;
      lastCommandTime = currentTime;
      break;

    case QUERY_POWER_MODE:
      queryPowerMode();
      currentState = IDLE;
      lastCommandTime = currentTime;
      break;

    case IDLE:
      // No action, continuously read GPS
      break;
    }
  }
}