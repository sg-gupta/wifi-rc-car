/*    wifi rc car code for workshop     */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

// Motor pins
int enA = 0;
int in1 = 2;
int in2 = 14;
int in3 = 12;
int in4 = 13;
int enB = 15;

int buzPin = 16;

// Ultrasonic sensor pins
int TRIG_FRONT = 5;
int ECHO_FRONT = 4;

String command;    // Stores the current command from the app
int SPEED = 122;
int speed_Coeff = 2;

const int objectNearThreshold = 20;  // Distance to trigger reverse (in cm)
const int objectFarThreshold = 35;   // Distance to trigger forward (in cm)

ESP8266WebServer server(80);

// Function declarations
void Stop();
void Forward();
void Backward();
void TurnRight();
void TurnLeft();
void ForwardLeft();
void BackwardLeft();
void ForwardRight();
void BackwardRight();
void BeepHorn();
long getDistance(int trigPin, int echoPin);
void HTTP_handleRoot();

void setup() {
  Serial.begin(115200);
  Serial.println("*WiFi Robot Remote Control Mode - L298N 2A*");

  // Set up pins
  pinMode(buzPin, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(TRIG_FRONT, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);

  Stop();

  WiFi.mode(WIFI_AP);
  String hostname = "RC_Car-Team B " + String(ESP.getChipId(), HEX).substring(4);
  WiFi.softAP(hostname.c_str());
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("AP Hostname: ");
  Serial.println(hostname);

  server.on("/", HTTP_handleRoot);
  server.onNotFound(HTTP_handleRoot);
  server.begin();
  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();

  if (server.hasArg("State")) {
    command = server.arg("State");

    if (command == "F") Forward();
    else if (command == "B") Backward();
    else if (command == "R") TurnRight();
    else if (command == "L") TurnLeft();
    else if (command == "G") ForwardLeft();
    else if (command == "H") BackwardLeft();
    else if (command == "I") ForwardRight();
    else if (command == "J") BackwardRight();
    else if (command == "S") Stop();
    else if (command == "V") BeepHorn();
    else if (command >= "0" && command <= "9") SPEED = command.toInt() * 20 + 60;
    else if (command == "q") SPEED = 255;
  }

  long distanceFront = getDistance(TRIG_FRONT, ECHO_FRONT);
  if (distanceFront < objectNearThreshold) {
    digitalWrite(buzPin, HIGH);
    Backward();
  } else {
    digitalWrite(buzPin, LOW);
  }
}

void HTTP_handleRoot(void) {
  server.send(200, "text/html", "");
  if (server.hasArg("State")) {
    Serial.println(server.arg("State"));
  }
}

// movement functions
void Forward() {
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  analogWrite(enA, SPEED); analogWrite(enB, SPEED);
  digitalWrite(buzPin, LOW);
}

void Backward() {
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  analogWrite(enA, SPEED); analogWrite(enB, SPEED);
}

void TurnRight() {
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  analogWrite(enA, SPEED); analogWrite(enB, SPEED);
  digitalWrite(buzPin, LOW);
}

void TurnLeft() {
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  analogWrite(enA, SPEED); analogWrite(enB, SPEED);
  digitalWrite(buzPin, LOW);
}

void ForwardLeft() {
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  analogWrite(enA, SPEED / speed_Coeff); analogWrite(enB, SPEED);
  digitalWrite(buzPin, LOW);
}

void BackwardLeft() {
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  analogWrite(enA, SPEED / speed_Coeff); analogWrite(enB, SPEED);
  digitalWrite(buzPin, LOW);
}

void ForwardRight() {
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  analogWrite(enA, SPEED); analogWrite(enB, SPEED / speed_Coeff);
  digitalWrite(buzPin, LOW);
}

void BackwardRight() {
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  analogWrite(enA, SPEED); analogWrite(enB, SPEED / speed_Coeff);
  digitalWrite(buzPin, LOW);
}

void Stop() {
  digitalWrite(in1, LOW); digitalWrite(in2, LOW);
  digitalWrite(in3, LOW); digitalWrite(in4, LOW);
  analogWrite(enA, 0); analogWrite(enB, 0);
  digitalWrite(buzPin, LOW);
}

void BeepHorn() {
  digitalWrite(buzPin, HIGH);
  delay(150);
  digitalWrite(buzPin, LOW);
  delay(80);
}

long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;  // Convert time to distance (cm)
}
