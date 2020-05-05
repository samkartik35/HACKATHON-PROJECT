/*This code is made by Kartik Kumar of Sri venkateshwar International school for a project to be desgined in hackathon hosted by INCUBATE IND
there are a few things that are not complete in this project-
1. the accurate values for sensors as they are not available with me during lockdown , hence i have given either 0 or default values to them
2. the code as of now only serial prints whether or not there is a leak or not in under the sinks , i.e. wheter it is dry or wet
the time of completetion of this code is 19:41 UTC +5:30 , 5/05/2020
*/

#include <ESP8266WiFi.h>
 
String apiKey = "XVEHNHJBBGK0N9NY";     //  Enter your Write API key from ThingSpeak

const char *ssid = "soul knight boiiis";     // replace with your wifi ssid and wpa2 key
const char *pass =  "sam1234567890";
const char* server = "api.thingspeak.com";


#define RL 47  //The value of resistor RL is 47K
#define m -0.263 //Enter calculated Slope 
#define b 0.42 //Enter calculated intercept
#define Ro 20 //Enter found Ro value
#define MQ_sensor A0 //Sensor is connected to A4
String SensorSerialNo; 
long H2S;
long Temperature;
long RH;
long RawSensor;
long TempDigital;
long RHDigital;
int Days;
int Hours;
int Minutes;
int Seconds;
 const int capteur_D = 4;
const int capteur_A = A0 ;

int val_analogique;

#define command_delay 500
#define start_delay 2500
String dataString = "";
String responseString = "";
boolean dataStringComplete = 0;
char inChar;
#include <SoftwareSerial.h>
SoftwareSerial portOne(10, 11);
  WiFiClient client;
  
void setup()  
{
       Serial.begin(115200);
       
 
       Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
        portOne.begin(9600);
  // Normally, data is returned within one second
  portOne.setTimeout(1000);
  // reserve 80 bytes for the dataString
  dataString.reserve(150);
  responseString.reserve(150);
  
  // Wait for sensor 
  delay(500);
  flush_portOne();
  
  // EEPROM dump
  SPEC_dump_EEPROM();
  Serial.println(" ");
  Serial.println("STARTING MEASUREMENTS");
  Serial.println(" ");
    pinMode(capteur_D, INPUT);
  pinMode(capteur_A, INPUT);
 
}
 
void loop() 
{
   Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");




                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                                  float VRL; //Voltage drop across the MQ sensor
  float Rs; //Sensor resistance at gas concentration 
  float ratio; //Define variable for ratio
  VRL = analogRead(MQ_sensor)*(5.0/1023.0); //Measure the voltage drop and convert to 0-5V
  Rs = ((5.0*RL)/VRL)-RL; //Use formula to get Rs value
  ratio = Rs/Ro;  // find ratio Rs/Ro
  float ppm = pow(10, ((log10(ratio)-b)/m)); //use formula to calculate ppm
  
   SPEC_Data_read();
  SPEC_parse_data();
  SPEC_print_data();

 //if (ppm=>threshold) {
// sound alarm or turn on light to indiacate that the bathroom needs a check and needs to be cleaned
//}35

 


                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(ppm);
                             postStr +="&field2=";
                             postStr += String(VRL);
                             postStr +="&field3=";
                             postStr += String(H2S);
                             postStr +="&field4=";
                             postStr += String(Temperature);
                             postStr += "\r\n\r\n\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n\n\n");
                             client.print(postStr);
 
                             Serial.print("ammonia ppm: ");
                             Serial.print(ppm);
                             Serial.print("hydrogen sulfide ppm: ");
                             Serial.print(H2S);
                             Serial.println("%. Send to Thingspeak.");
                        }
          client.stop();
 
          Serial.println("Waiting...");
  
  
  if(digitalRead(capteur_D) == LOW) 
  {
    Serial.println("Digital value : wet"); 
    delay(10); 
  }
else
  {
    Serial.println("Digital value : dry");
    delay(10); 
  }
val_analogique=analogRead(capteur_A); 
 Serial.print("Analog value : ");
 Serial.println(val_analogique); 
 Serial.println("");
  delay(1000);
  delay(14900);
}
void SPEC_Data_read(){
  // First, we do some initialization
  // dataStringComplete is set as "false", as we don't have any valid data received
  dataStringComplete = 0;
  // Clear the data string
  dataString = "";
  // Now we trigger a measurement
  portOne.print(" ");
  // We wait for the sensor to respond
  dataString = portOne.readStringUntil('\n');
  //Serial.println(dataString);
}

void SPEC_parse_data(){

  int idx1 = dataString.indexOf(',');
  SensorSerialNo = dataString.substring(0, idx1);
  int idx2 = dataString.indexOf(',', idx1 + 1);
  // Hint: after comma there's a space - it should be ignored
  String S_gas = dataString.substring(idx1 + 2, idx2);
  H2S = S_gas.toInt();
  int idx3 = dataString.indexOf(',', idx2 + 1);
  String S_temp = dataString.substring(idx2 + 2, idx3);
  Temperature = S_temp.toInt();
  int idx4 = dataString.indexOf(',', idx3 + 1);
  String S_humi = dataString.substring(idx3 + 2, idx4);
  RH = S_humi.toInt();
  int idx5 = dataString.indexOf(',', idx4 + 1);
  String S_raw_gas = dataString.substring(idx4 + 2, idx5);
  RawSensor = S_raw_gas.toInt();
  int idx6 = dataString.indexOf(',', idx5 + 1);
  String S_Tdigital = dataString.substring(idx5 + 2, idx6);
  TempDigital = S_Tdigital.toInt();
  int idx7 = dataString.indexOf(',', idx6 + 1);
  String S_RHdigital = dataString.substring(idx6 + 2, idx7);
  RHDigital = S_RHdigital.toInt();
  int idx8 = dataString.indexOf(',', idx7 + 1);
  String S_Days = dataString.substring(idx7 + 2, idx8);
  Days = S_Days.toInt();
  int idx9 = dataString.indexOf(',', idx8 + 1);
  String S_Hours = dataString.substring(idx8 + 2, idx9);
  Hours = S_Hours.toInt();
  int idx10 = dataString.indexOf(',', idx9 + 1);
  String S_Minutes = dataString.substring(idx9 + 2, idx10);
  Minutes = S_Minutes.toInt();
  int idx11 = dataString.indexOf('\r');
  String S_Seconds = dataString.substring(idx10 + 2, idx11);
  Seconds = S_Seconds.toInt();
}
void SPEC_print_data(){
  Serial.println("********************************************************************");
  Serial.print ("Sensor Serial No. is ");
  Serial.println (SensorSerialNo);
  Serial.print ("H2S level is ");
  Serial.print (H2S);
  Serial.println (" ppb");
  Serial.print ("Temperature is ");
  Serial.print (Temperature, DEC);
  Serial.println (" deg C");
  Serial.print ("Humidity is ");
  Serial.print (RH, DEC);
  Serial.println ("% RH");
  Serial.print ("Sensor is online since: ");
  Serial.print (Days, DEC);
  Serial.print (" days, ");
  Serial.print (Hours, DEC);
  Serial.print (" hours, ");
  Serial.print (Minutes, DEC);
  Serial.print (" minutes, ");
  Serial.print (Seconds, DEC);
  Serial.println (" seconds");
  Serial.println ("Raw Sensor Data");    
  Serial.print ("Raw gas level: ");
  Serial.println (RawSensor);
  Serial.print ("Temperature digital: ");
  Serial.println (TempDigital);
  Serial.print ("Humidity digital: ");
  Serial.println (RHDigital);
  Serial.println ("");
}
void SPEC_dump_EEPROM(){
  // First we trigger a measurement
  portOne.print(" ");
  // Within one second time we send the command "e"
  delay(400);
  portOne.print("e");
  dataString = portOne.readStringUntil('\n');
  // You can uncomment this line if you wish
  //Serial.println(dataString);
  for (int i=0; i<20; i++){ 
    responseString = portOne.readStringUntil('\n');
    Serial.println(responseString);
  }   
}  
 void flush_portOne(){
  // Do we have data in the serial buffer?
  // If so, flush it
  if (portOne.available() > 0){
    Serial.println ("Flushing serial buffer...");
    while(1){
      inChar = (char)portOne.read();
      delay(10);
      Serial.print(inChar);
      if (inChar == '\n') break; 
    }
    Serial.println (" ");
    Serial.println ("Buffer flushed!");
  }
}  

