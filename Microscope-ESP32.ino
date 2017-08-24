#include <driver/dac.h>
#include <WiFi.h>

//SETUP Wlan connection
char ssid[] = "SSID";
const char* password = "PASSWORD";

int Powerbutton = 25;

const char* buttonTypeName = "";
int type = 0;

int ok = 0;
int mode = 29;
int menu = 57;
int right = 76;
int left = 104;

WiFiServer server(80);

// Client variables 
char linebuf[80];
int charcount=0;


void setup() {
  // Enable Serial
  Serial.begin(115200);
  while(!Serial){
    ; // wait for serial port to connect. 
  }
  // Connect to Wifi
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    // Connect to a WPA/WPA2 Network
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  //Power button
  pinMode(Powerbutton, OUTPUT);
  
  // Enable DAC
  //Power button
  dac_output_enable(DAC_CHANNEL_1);
  //Navigation
  dac_output_enable(DAC_CHANNEL_2);
  dac_output_voltage(DAC_CHANNEL_2, 255);
  
  /*
   * 255 = 3,15V
   * 
   * Resistors: 0       15K         33K         47K         68K
   * Voltage:   0 V     400 mV      750 mV      975 mV      1325 mV
   * Pins:      OK      MODE        MENU        RIGHT       LEFT
   * IO25:      0       29          57/58       76          104/106                       
   *    
   * The Power Button need to be controlled seperately
   */
  server.begin();

}

void loop() {
  // put your main code here, to run repeatedly:

WiFiClient client = server.available();
if(client) {
  Serial.println("new client connected");
  memset(linebuf,0,sizeof(linebuf));
  charcount=0;
  //a http request ends with a blank line
  boolean currentLineIsBlank=true;
  while (client.connected()){
    if (client.available()){
      char c = client.read();
      Serial.write(c);
      //read charset by http request
        linebuf[charcount]=c;
      if (charcount<sizeof(linebuf)-1) charcount++;
      /* if you've gotten to the end of the line (received a newline
         character) and the line is blank, the http request has ended,
         so you can send a reply
      */
      if(c =='\n' && currentLineIsBlank) {
         // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println();
          client.println("<!DOCTYPE HTML><html><head>");
          client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>");
          client.println("<h1>ESP32 Microscope- Web Server</h1>");
          client.println("<p>Power<a href=\"pwrOn\"><button>ON</button></a>&nbsp;<a href=\"pwrOff\"><button>OFF</button></a></p>");
          client.println("<p>OK<a href=\"ok\"><button>OK</button></a></p>");
          client.println("<p>Menu<a href=\"mnu\"><button>Menu</button></a></p>");
          client.println("<p>Mode<a href=\"mde\"><button>Mode</button></a></p>");
          client.println("<p>Left<a href=\"lft\"><button>Left</button></a></p>");
          client.println("<p>Right<a href=\"rgt\"><button>Right</button></a></p>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
          if (strstr(linebuf,"GET /pwrOn") > 0){
            Serial.println("Power ON");
            PowerToggle();
          }
          else if (strstr(linebuf,"GET /ok") > 0){
            Serial.println("OK");
            Button(ok);
          }
          else if (strstr(linebuf,"GET /mnu") > 0){
            Serial.println("Menu");
            Button(menu);
          }
          else if (strstr(linebuf,"GET /mde") > 0){
            Serial.println("Mode");
            Button(mode);
          }
          else if (strstr(linebuf,"GET /lft") > 0){
            Serial.println("Left");
            Button(left);
          }
          else if (strstr(linebuf,"GET /rgt") > 0){
            Serial.println("Right");
            Button(right);
          }
          // you're starting a new line
          currentLineIsBlank = true;
          memset(linebuf,0,sizeof(linebuf));
          charcount=0;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

//PowerToggle();
//Serial.println("Power on");

/*
  Serial.println("Power button on");
  dac_output_voltage(DAC_CHANNEL_1, 255);
  delay(1325);
  dac_output_voltage(DAC_CHANNEL_1, 0);
  Serial.println("wait");
  delay(10000);
  Serial.println("Power off");
  dac_output_voltage(DAC_CHANNEL_1, 255);
  delay(1325);
  dac_output_voltage(DAC_CHANNEL_1, 0);
  delay(10000);
*/

int PowerToggle(){
  Serial.println("Power button on");
  dac_output_voltage(DAC_CHANNEL_1, 255);
  delay(1325);
  dac_output_voltage(DAC_CHANNEL_1, 0);
  delay(3000);
  }
  
int Button(int type){
  if (type == 0){
    buttonTypeName = "OK";
  }
  else if (type == 29){
    buttonTypeName = "Mode";
    }
  else if (type == 57){
    buttonTypeName = "Menu";
    }
  else if (type == 76){
    buttonTypeName = "Right";
    }
  else if (type == 104){
    buttonTypeName = "Left";
    }
  Serial.print(buttonTypeName);
  Serial.println(" button pressed");
  dac_output_voltage(DAC_CHANNEL_2,type);
  delay(200);
  dac_output_voltage(DAC_CHANNEL_2, 255);
}

