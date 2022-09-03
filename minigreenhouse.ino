#include <ESP8266WiFi.h>
#include <Servo.h>

//settings
const char* ssid = "Mudbunker";
const char* password = "Mudde112";
int SerialSpeed = 115200;
int ServoPin = 14;
int WaterLevelPin = 12;
int WebServerPort = 80;

int WaterLevelValue = 0;
String ServoState = "";
String LightState = "";
String WaterState = "";

Servo servo;
WiFiServer server(WebServerPort);

void setup() {
  
  //setting up serial and wifi connections.
  Serial.begin(SerialSpeed);
  delay(10);
  servo.attach(ServoPin);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  server.begin();
  Serial.println("Server started");
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  //resetting all motors and lights
  servo.write(30);
  delay(150);
  servo.write(0);
}

void loop() {
  //checking for clients
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  WaterLevelValue = analogRead(WaterLevelPin);
  
  if (request.indexOf("/Req=0") != -1)  {
    servo.write(30);
    ServoState = "closed";
    delay(190);
    servo.write(0);
  }
  if (request.indexOf("/Req=90") != -1)  {
    servo.write(90);
    ServoState = "half open";
  }
  if (request.indexOf("/Req=180") != -1)  {
    servo.write(180);
    ServoState = "open";
  }
  if (request.indexOf("/Led=on") != -1)  {
    digitalWrite(0, HIGH);
    LightState = "on";
  }
  if (request.indexOf("/Led=off") != -1)  {
    digitalWrite(0, LOW);
    LightState = "off";
  }
  //the html webpage
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<h1 align=center>Greenhouse</h1><br><br>");
  client.print("deurtje  status: ");
  client.print(ServoState);
  client.println("<br><br>");
  client.println("<a href=\"/Req=0\"\"><button>dicht</button></a>");
  client.println("<a href=\"/Req=90\"\"><button>half open</button></a>");
  client.println("<a href=\"/Req=180\"\"><button>open</button></a>");
  client.println("<br><br>");
  client.print("licht status: ");
  client.print(LightState);
  client.println("<br><br>");
  client.println("<a href=\"/Led=on\"\"><button>uit</button></a>");
  client.println("<a href=\"/Led=off\"\"><button>aan</button></a>");
  client.println("<br><br>");
   client.print("Water level: ");
  client.print(WaterLevelValue);
  client.print("<br>Water status: ");
  client.print(LightState);
  client.println("<br><br>");
  client.println("<a href=\"/Re\"\"><button>uit</button></a>");
  client.println("<a href=\"/Re\"\"><button>aan</button></a>");
  client.println("</html>");
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");

}
