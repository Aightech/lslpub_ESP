/*
  This sketch receives UDP request and replied with the value of its inputs

  created 24 04 2019
  by Alexis Devillard

*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define STASSID "Mi Phone"          // credentials
#define STAPSK  "nabiloubilou"
unsigned int localPort = 8888;      // local port to listen on

#define NB_CH 5
#define BUF_LEN 100
float sample[NB_CH*BUF_LEN];
int index_save = 0;
int index_send = 0;


// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
WiFiUDP Udp;

void setup() 
{
  Serial.begin(115200);

  //connect to network
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) 
    delay(500);

  Serial.print("\nConnected.\n IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP server on port %d\n", localPort);

  //start listening
  Udp.begin(localPort);

  //for(int i =0; i< NB_CH-1; i++)
    //pinMode(5+i, OUTPUT);
}

void loop() 
{
  for(int i = 0 ; i< NB_CH; i++)
    sample[index_save + i]= analogRead(0);
  index_save+=NB_CH;
  index_send+=NB_CH;
  
  index_save%=NB_CH*BUF_LEN;
  
  
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) 
  {
    
    // read the packet into packetBufffer
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    //Serial.printf("Request : %s", packetBuffer);

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Serial.printf("Sent %d bytes [ %f, %f, %f, %f, %f]\n", index_save*4, sample[0],sample[1],sample[2],sample[3],sample[4]);
    delay(1);
    Udp.write((char *)sample,index_save*4);
    index_save=0;
    
    Udp.endPacket();
  }
  delay(10);

  
}

/*
  test (shell/netcat):
  --------------------
	  nc -u 192.168.esp.address 8888
*/
