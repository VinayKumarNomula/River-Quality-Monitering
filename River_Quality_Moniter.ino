

 #include <SoftwareSerial.h>
 SoftwareSerial myGPRS(10,11);
 #include <DHT.h>
 #define DHTPIN 2     
 #define DHTTYPE DHT11   
 String host= "api.thingspeak.com";
 String GET = "GET /update?key=40M4SLLCN9DWC5VY";    //thingspeak key
DHT dht(DHTPIN, DHTTYPE);
const int analogInPin = A0; //ph sensor pin 
int sensorValue = 0; 
unsigned long int avgValue; 
const int photocellPin = A1;  //turbintiysensor pin
 float myI;
float r1;
float NTUval;
float val;
float ardval;   
float b;
int buf[10];
float hum;  
float temp;
float phValue;
char d;
void setup() {
  Serial.begin(9600);
  pinMode (photocellPin, INPUT);
  dht.begin();

   //pinMode (soundDetectedPin, INPUT);
   myGPRS.begin(9600);
  delay(1000);
  
  sendmyGPRS("AT");
  delay(5000);
  
  if(myGPRS.find("OK")) 
  {
      d = myGPRS.read();
      //Serial.println(d);
  }
}
void loop() {  
    temp= dht.readTemperature();
    val = analogRead(photocellPin);
    ardval = val*0.00488758553;
   r1 = (50000/ardval)-10000;
   myI = ardval/r1; //value of I which we are solving for
   NTUval = myI*70000;   
   Serial.print("Turbinity: ");
    Serial.print(NTUval);
    Serial.println("NTU");
  
    //Print temp and humidity values to serial monitor
  Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
   
  for(int i=0;i<10;i++) 
 { 
  buf[i]=analogRead(analogInPin);
  delay(10);
 }
 for(int i=0;i<9;i++)
 {
  for(int j=i+1;j<10;j++)
  {
   if(buf[i]>buf[j])
   {
    temp=buf[i];
    buf[i]=buf[j];
    buf[j]=temp;
   }
  }
 }
 avgValue=0;
 for(int i=2;i<8;i++)
 avgValue+=buf[i];
 float pHVol=(float)avgValue*5.0/1024/6;
 phValue = -5.70 * pHVol + 21.34;
 Serial.print("pHValue: ");
    Serial.print(pHVol);
    Serial.println(" ph");
  
    //soundDetectedVal = digitalRead(soundDetectedPin) ;
    uploadtoThingspeak();

}


void uploadtoThingspeak(){

myGPRS.listen();

sendmyGPRS("AT+CIPSHUT");
  delay(2000);
  if( myGPRS.find( "SHUT OK" ) )
  {
    
       d = myGPRS.read();
     //Serial.write(d);
     Serial.println(d);
    //return;
  }
  
sendmyGPRS("AT+CIPMUX=1");
  delay(2000);
 if( myGPRS.find( "OK" ) )
  {
       d = myGPRS.read();
      //Serial.write(d);
      Serial.println();
    //return;
  }
sendmyGPRS("AT+CGATT=1");
  delay(2000);

   if( myGPRS.find( "OK" ) )
  {
      d = myGPRS.read();
      //Serial.write(d);
      Serial.println(d);
    //return;
  } 
String cmd = "AT+CSTT=\"airtelgprs.com\",\"\",\"\"";      // Setup TCP connection
sendmyGPRS(cmd);
  delay(2000);

   if( myGPRS.find( "OK" ) )
  {
      d= myGPRS.read();
     Serial.write(d);
     Serial.println();
    //return;
  }

sendmyGPRS("AT+CIICR");
  delay(2000);

   if( myGPRS.find( "OK" ) )
  {
     d = myGPRS.read();
      Serial.write(d);
      //Serial.println();
    //return;
  }

sendmyGPRS("AT+CIFSR");
  delay(2000);
   
   if( myGPRS.find( "Error" ) )
  {
    myGPRS.println( "ERROR Received" );
    //return;
  }
  else {
    d = myGPRS.read();
    Serial.write(d);
     // Serial.println();
  }
  
  cmd = "AT+CIPSTART=3,\"TCP\",\"";// Setup TCP connection
  cmd += host;
  cmd += "\",80";

sendmyGPRS(cmd);
  delay(4000);

     if( myGPRS.find("CONNECTED") )
  {
      d = myGPRS.read();
      //Serial.write(d);
      Serial.println("Connected to Thingspeak");
    //return;
  }


String cmd1 = "AT+CIPSEND=3,100";// Setup TCP connection
sendmyGPRS(cmd1);
delay(2000);

cmd = GET+ "&field1="+temp+"&field2="+hum+"&field3="+phValue+"&field4="+ +""+"\r\n";

if( myGPRS.find( ">" ) )
  {
    myGPRS.println(cmd);
    myGPRS.println();
    Serial.println(cmd);
  }
  else
  {
    sendmyGPRS( "AT+CIPCLOSE" );//close TCP connection
  }
  sendmyGPRS("AT+CIPCLOSE");
}


void sendmyGPRS(String cmd)
{
  myGPRS.println(cmd);
}
