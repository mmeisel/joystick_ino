#include <Wire.h>

#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

char ssid[] = "your-ssid-here";          // your network SSID (name)
char pass[] = "your-password-here";                    // your network password

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(10, 0, 0, 125);     // remote IP of your computer
const unsigned int outPort = 9999;          // remote port to receive OSC
const unsigned int localPort = 8888;        // local port to listen for OSC packets (actually not used for sending)

#define X_AXIS_PIN A2
#define Y_AXIS_PIN A3
#define SWITCH_PIN 21

void setup(void)
{
  pinMode(X_AXIS_PIN, INPUT);
  pinMode(Y_AXIS_PIN, INPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  Serial.begin(115200);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
#ifdef ESP32
  Serial.println(localPort);
#else
  Serial.println(Udp.localPort());
#endif
}

void loop(void)
{
  /* Get a new reading */
  float x = (float) analogRead(X_AXIS_PIN) / 4095.0;
  float y = (float) analogRead(Y_AXIS_PIN) / 4095.0;
  // Since the pin is set to pullup mode, HIGH means the switch is off.
  float sw = digitalRead(SWITCH_PIN) == HIGH ? 0.0 : 1.0;

  Serial.print("x:");
  Serial.print(x);
  Serial.print(" y:");
  Serial.print(y);
  Serial.print(" sw:");
  Serial.print(sw);
  Serial.println();

  OSCMessage msg("/accel");
  msg.add(x);
  msg.add(y);
  msg.add(sw);

  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();

  delay(100);
}
