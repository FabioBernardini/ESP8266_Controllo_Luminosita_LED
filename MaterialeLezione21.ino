//Carichiamo la libreria
#include <ESP8266WiFi.h>

//Sostituire i dati relativi alla propria rete wireless
const char* ssid     = "NETGEAR07";
const char* password = "magicalsheep499";

//Impostiamo il server con la relativa porta
WiFiServer server(80);

//Creiamoci una variabile per "ascoltare" le richieste dei client
String header;

//Variabile per impostare il grado di luminosità
int lumos=0;


//Pin analogico
const int pinLED= 14;


void setup() {
  Serial.begin(57600);
  pinMode(pinLED,OUTPUT);
  //Impostazioni iniziali del LED
  analogWrite(pinLED, 1023);
  delay(500);
  analogWrite(pinLED, 0);

  //Connettiamoci alla rete
  Serial.print("Mi sto connettendo a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  //Finchè non siamo connessi, stampiamo un punto
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //Mostriamo l'indirizzo IP della shceda non appena siamo connessi
  Serial.println("");
  Serial.println("Connesso alla rete.");
  Serial.println("Il mio indirizzo IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  //Ci mettiamo in ascolto continuo dei client che si connettono
  WiFiClient client = server.available();  

  if (client) {                 
    //Se c'è un nuovo client connesso, gestiamo la sua richiesta            
    Serial.println("Nuovo client");         
    //Ci creiamo una variabile per ottenere i dati in arrivo dalla richiesta del client
    String currentLine = "";                
    //Fintanto che il client è connesso, ne gestiamo la richiesta
    while (client.connected()) {            
      //Se ci sono byte nella richiesta, li leggiamo
      if (client.available()) {             
        //E li leggiamo uno per volta
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        if (c == '\n') {             
          //Se il carattere letto corrisponde a un ritorno a capo e la lunghezza è zero, abbiamo finito di leggere la sua richiesta e procediamo a gestirla
          if (currentLine.length() == 0) {

            //Rispondiamo al client dicendogli che è andato tutto in porto e gli rispondiamo in HTML con protocollo HTTP
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            //In base a ciò che c'è nell'indirizzo, aumentiamo o diminuiamo la luminosità
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
            
            //Mostriamo in risposta la pagina web
            client.println("<!DOCTYPE html><html>");
            client.println("<head><title>Server web ESP</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>");
            
            //Impostiamo il body, il contenuto della nostra pagina web
            client.println("<body><h1 style=\"width: 100%; height:64px\">Server web ESP</h1>");
            
            //Mostriamo lo stato attuale del LED
            
            client.println("<p>Luminosit&agrave; LED: " + String(lumos/10) + "%</p>");
            //Mostriamo i due bottoni e in funzione della percentuale li mettiamo attivi o meno
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
            
            
               
           //Chiudiamo la pagina
            client.println("</body></html>");
            
            // La risposta HTML deve terminare con una riga vuota
            client.println();
            //Infine, rompiamo il ciclo
            break;
          } else { 
            //Altrimenti, se abbiamo una nuova linea, cancelliamo la precedente
            currentLine = "";
          }
        } else if (c != '\r') {  
          //Se invece il carattere letto è un ritorno a capo, lo aggiungiamo alla linea di testo corrente
          currentLine += c;      
        }
      }
    }
    //Svuotiamo la variabile header
    header = "";
    //Chiudiamo la connessione col client
    client.stop();
    Serial.println("Client disconnesso");
    Serial.println("");
  }
}
