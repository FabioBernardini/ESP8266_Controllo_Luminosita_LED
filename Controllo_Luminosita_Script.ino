
#include <ESP8266WiFi.h>


const char* ssid     = "********";
const char* password = "********";


WiFiServer server(80);


String header;


int lumos=0;



const int pinLED= 14;


void setup() {
  Serial.begin(57600);
  pinMode(pinLED,OUTPUT);
  //Impostazioni iniziali del LED
  analogWrite(pinLED, 1023);
  delay(500);
  analogWrite(pinLED, 0);

 
  Serial.print("Mi sto connettendo a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connesso alla rete.");
  Serial.println("Il mio indirizzo IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  
  WiFiClient client = server.available();  

  if (client) {                 
    
    Serial.println("Nuovo client");         
    
    String currentLine = "";                
    
    while (client.connected()) {            
      
      if (client.available()) {             
        
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        if (c == '\n') {             
          
          if (currentLine.length() == 0) {

            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            
            if (header.indexOf("ledUP") >= 0) {
              if (lumos<1000){
                Serial.println("Aumento la luminosità");
              lumos+=100;
              analogWrite(pinLED,lumos);  
              }
              else{
                Serial.println("Luminosità massima raggiunta");
              }
              
            } else if (header.indexOf("ledDW") >= 0) {
              if (lumos>0){
                lumos-=100;
                Serial.println("Riduco la luminosità");
                analogWrite(pinLED,lumos);
              }
              else{
                Serial.println("Il LED è spento");
              }
            } 
            
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head><title>Server web ESP</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>");
            
            
            client.println("<body><h1 style=\"width: 100%; height:64px\">Server web ESP</h1>");
            
            
            
            client.println("<p>Luminosit&agrave; LED: " + String(lumos/10) + "%</p>");
            
            if (lumos==1000){
              client.println("<p style=\"width: 100%; height:64px\"><a href=\"/ledUP\"><button disabled>Aumenta Luminosit&agrave;</button></a></p>");  
            }
            else{
              client.println("<p style=\"width: 100%; height:64px\"><a href=\"/ledUP\"><button>Aumenta Luminosit&agrave;</button></a></p>");
            }
            if (lumos==0){
             client.println("<p style=\"width: 100%; height:64px\"><a href=\"/ledDW\"><button disabled>Riduci Luminosit&agrave;</button></a></p>"); 
            }
            else{
              client.println("<p style=\"width: 100%; height:64px\"><a href=\"/ledDW\"><button>Riduci Luminosit&agrave;</button></a></p>"); 
            }
            
            
               
           
            client.println("</body></html>");
            
            
            client.println();
            //Infine, rompiamo il ciclo
            break;
          } else { 
            
            currentLine = "";
          }
        } else if (c != '\r') {  
          
          currentLine += c;      
        }
      }
    }
    
    header = "";
    
    client.stop();
    Serial.println("Client disconnesso");
    Serial.println("");
  }
}
