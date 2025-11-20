#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// Pines del sensor ultrasónico
#define echoPin 16  // HC SR04 Trig <——> GPIO16(D2)
#define trigPin 5   // HC SR04 Echo <——> GPIO5 (D3)
int Front_Distance;

#define IN_2 4   // L9110S  A-1B <——> GPIO4 (D4) MOTOR A
#define IN_1 14  // L9110S  A-1A <——> GPIO14(D5) MOTOR A
#define IN_4 12  // L9110S  B-1B <——> GPIO12(D6) MOTOR B
#define IN_3 13  // L9110S  B-1A <——> GPIO13(D7) MOTOR B

int speedCar = 255;  // 0 - 200

// Wi-Fi
const char* ssid = "INFINITUM9757";
const char* password = "Ec2Nm4Dd4i";

// URL de la API
const String api_url = "http://192.168.1.64:5000/api/iot-devices/last-status-texto";

// Función para medir distancia con el sensor HC-SR04
float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float distance = pulseIn(echoPin, HIGH) / 58.00;
  delay(10);
  return distance;
}

// Movimiento del carro
void mover(String accion) {
  // Detiene el carro primero
  /*stopRobot();*/

  if (accion == "Adelante") {
    Serial.println("El carro va hacia: " + accion);
    goAhead();
  } else if (accion == "Atrás") {
    Serial.println("El carro va hacia: " + accion);
    goback();
  } else if (accion == "Detener") {
    Serial.println("El carro va hacia: " + accion);
    stopRobot();
  } else if (accion == "Vuelta adelante izquierda") {
    Serial.println("El carro va hacia: " + accion);
    goAheadLeft();
    //forwardLeftTurn();
  } else if (accion == "Vuelta adelante derecha") {
    Serial.println("El carro va hacia: " + accion);
    goAheadRight();
    //forwardRightTurn();
  } else if (accion == "Vuelta atrás izquierda") {
    Serial.println("El carro va hacia: " + accion);
    gobackLeft();
        //backwardLeftTurn();
  } else if (accion == "Vuelta atrás derecha") {
    Serial.println("El carro va hacia: " + accion);
    gobackRight();
    //backwardRightTurn();
  } else if (accion == "Giro 90° izquierda") {
    Serial.println("El carro va hacia: " + accion);
    turnLeft90();
    //turnLeft90();
  } else if (accion == "Giro 90° derecha") {
    Serial.println("El carro va hacia: " + accion);
    turnRight90();
    //turnRigth90();
  } else if (accion == "Giro 360° izquierda") {
    Serial.println("El carro va hacia: " + accion);
    spinLeft360();
    //turnLeft360();
  } else if (accion == "Giro 360° derecha") {
    Serial.println("El carro va hacia: " + accion);
    spinRight360();
    //turnRigth360();
  }

 /* delay(1000);     // Espera tras ejecutar la acción
  stopRobot();     // Detiene sin llamar a mover() otra vez */
}

// Verifica obstáculos y decide movimiento
void evitarObstaculo() {
  Front_Distance = getDistance();

  if (Front_Distance < 20) {
    mover("Detener");
    delay(500);

    // Revisa lado izquierdo
    mover("Giro 90° izquierda");
    delay(500);
    long distIzq = getDistance();

    // Revisa lado derecho
    mover("Giro 90° derecha");  // regresa al frente
    mover("Giro 90° derecha");
    delay(500);
    long distDer = getDistance();

    // Vuelve al frente
    mover("Giro 90° izquierda");

    if (distIzq > 20) {
      mover("Giro 90° izquierda");
      mover("Adelante");
    } else if (distDer > 20) {
      mover("Giro 90° derecha");
      mover("Adelante");
    } else {
      mover("Giro 360° derecha");
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Pines de motor
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);

  // Pines del sensor
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);   // Sets the echoPin as an Input

  // Conexión Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("WiFi conectado");
}

void loop() {
  //evitarObstaculo();
  //Serial.println(WiFi.status());

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, api_url);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      //Serial.println("Respuesta API: " + payload);

      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      String accion = doc["status_texto"];

      Serial.println("Valor: " + accion);

      mover(accion);
    } else {
      Serial.println("Error HTTP: " + String(httpCode));
    }
    http.end();
  }


  delay(2000);  // Espera antes de la siguiente verificación
}


/****** Funciones de dirección **************/

void goAhead()
{ 
  analogWrite(IN_1, speedCar);
  digitalWrite(IN_2, LOW);
  digitalWrite(IN_3, LOW);
  analogWrite(IN_4, speedCar);
}

void goAheadRight()
{ 
  analogWrite(IN_1, speedCar);
  digitalWrite(IN_2, LOW);
  digitalWrite(IN_3, LOW);
  analogWrite(IN_4, speedCar*.6);
}

void goAheadLeft()
{ 
  analogWrite(IN_1, speedCar*.6);
  digitalWrite(IN_2, LOW);
  digitalWrite(IN_3, LOW);
  analogWrite(IN_4, speedCar);
}

void goback()
{ 
  digitalWrite(IN_1, LOW);
  analogWrite(IN_2, speedCar);
  analogWrite(IN_3, speedCar);
  digitalWrite(IN_4, LOW);
}

void gobackRight()
{ 
  digitalWrite(IN_1, LOW);
  analogWrite(IN_2, speedCar);
  analogWrite(IN_3, speedCar*.6);
  digitalWrite(IN_4, LOW);
}

void gobackLeft()
{ 
  digitalWrite(IN_1, LOW);
  analogWrite(IN_2, speedCar*.6);
  analogWrite(IN_3, speedCar);
  digitalWrite(IN_4, LOW);
}

void goLeft()
{ 
  analogWrite(IN_1, speedCar);
  digitalWrite(IN_2, LOW);
  analogWrite(IN_3, speedCar);
  digitalWrite(IN_4, LOW);
}

// Giro 360° a la derecha
void spinLeft360() {
  digitalWrite(IN_1, LOW);
  analogWrite(IN_2, speedCar);
  digitalWrite(IN_3, LOW);
  analogWrite(IN_4, speedCar);
  delay(2000);
  stopRobot();
}

// Giro 360° a la izquierda
void spinRight360() {
  analogWrite(IN_1, speedCar);
  digitalWrite(IN_2, LOW);
  analogWrite(IN_3, speedCar);
  digitalWrite(IN_4, LOW);
  delay(2000);
  stopRobot();
}

// Girar 90° a la derecha (en su eje)
void turnLeft90() {
  digitalWrite(IN_1, LOW);
  analogWrite(IN_2, speedCar);
  digitalWrite(IN_3, LOW);
  analogWrite(IN_4, speedCar);
  delay(500);  // Ajusta este tiempo para que equivalga a 90 grados
  stopRobot();
}

// Girar 90° a la izquierda (en su eje)
void turnRight90() {
  analogWrite(IN_1, speedCar);
  digitalWrite(IN_2, LOW);
  analogWrite(IN_3, speedCar);
  digitalWrite(IN_4, LOW);
  delay(500);  // Ajusta este tiempo para que equivalga a 90 grados
  stopRobot();
}

void stopRobot()
{  
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, LOW);
}
