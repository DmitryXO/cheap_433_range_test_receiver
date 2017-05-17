#include <VirtualWire.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

uint8_t counter = 0; 

void setup() {
  /*
   * Initialize receiver
   */
  vw_set_tx_pin(11);
  vw_set_rx_pin(12);
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(1000);       // Bits per sec
  vw_rx_start();

  /*
   * Initialize OLED display
   */
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.display();
}

int currentPacket = 0;
uint8_t errorRate = 0;
uint8_t expectedPacket = 0;

long lastReceive = millis();
long oledUpdate = millis();

void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  bool isError = false;

  if (vw_get_message(buf, &buflen)) {
    currentPacket = (uint8_t) buf[0];
    lastReceive = millis();

    if (currentPacket != expectedPacket) {
      isError = true;
      errorRate++; 
    }

    expectedPacket = currentPacket + 1;

    /*
     * If there was no error, decrease error counter
     */
    if (!isError) {
      if (errorRate > 0) {
        errorRate--;
      }
    }
  }

  if (!isError && millis() - lastReceive > 200) {
    errorRate++;
    isError = true;
    lastReceive = millis();
  }

  if (errorRate > 100) {
    errorRate= 100;
  }
 
  if (millis() - oledUpdate > 150) {
    display.clearDisplay();
    
    display.setCursor(0, 0);
    display.print("Packet: ");
    display.print(currentPacket);

    display.setCursor(0, 10);
    display.print("Error rate: ");
    display.print(errorRate);
    
    
    display.display(); 

    oledUpdate = millis();
  }
  
}
