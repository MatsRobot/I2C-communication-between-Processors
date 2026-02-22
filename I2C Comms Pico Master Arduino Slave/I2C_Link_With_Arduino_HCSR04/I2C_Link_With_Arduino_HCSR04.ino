/*
  I2C_Link_With_Arduino_HCSR04.ino

   https://github.com/robotcreation
   Using Ultrasonic Sensor HC-SR04
    Ultrasonic sensor Pins:
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin11
        Echo: Echo (OUTPUT) - Pin 12
        GND: GND

   I2C on A5-SCL and A4-SDA

  This module was build based on what was available on the internet

*/

// I2C parameters
#include <Wire.h>
#define Slave_Address 0x0A
#define I2C_Data_SIZE 32

// use char instead of string to avoid instability
char OutData[36];
char InData[36];

int trigPin = 11;    // Trigger
int echoPin = 12;    // Echo
long duration, cm, mm, inches;


// needed for UART interface (RX/TX on Digital pins)
#include <SoftwareSerial.h>
SoftwareSerial port1(12, 12); // MaxSonar- TX on Pin 12 to read data (RX not used on Serial)
int start_pin1=11;            // MaxSonar- RX on MaxSonar is set at Pin 11 to start the cycle

char inChar;          // MaxSonar to read each byte
char inString[36];    // MaxSonar to read all data 

int SensorData1=0;   // to hold Range from sensor in an integer
int index = 0;       // index to count the number oy bytes received

int RandomData2=0;   // To test random number sent to the I2C to test stability
unsigned long startTime;
unsigned long endTime;
long i;
long j;


//-------------------------------------------------------------------------------------------
// setup
//----------------------------------------------------------------------------------------------------------

void setup()
{
  // to make onboard led blink
  pinMode(LED_BUILTIN, OUTPUT);
  
  // sonar pin setup
  pinMode(start_pin1, OUTPUT); 
  digitalWrite(start_pin1,LOW);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // set the data rate for the SoftwareSerial port
 // port1.begin(9600);
   //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
 
  Serial.println("Serial Ready");  // Write with the carrage return and line feed
  delay(50);

  // I2C Link
  Wire.begin(Slave_Address);

  /*Event Handlers*/
  Wire.onReceive(DataReceive);
  Wire.onRequest(DataRequest);

}

//-------------------------------------------------------------------------------------------
// Main loop
//----------------------------------------------------------------------------------------------------------

void loop()
{
  SensorData1 = GetSensorData1();        // get sonar range
  Serial.println(SensorData1);

// Stable when Random number is pushed to I2C databus
// RandomData2 = GetRandomData2();        
// Serial.println(RandomData2);


  MSDelay (100);
}


//-------------------------------------------------------------------------------------------
// Function repacing delay using loop
//----------------------------------------------------------------------------------------------------------

void MSDelay(long itime){
  // every 26 loops of the calculation appear to take a ms
	for(i=0;i<(itime*26);i++)
		  {j=j/i;}
}


//-------------------------------------------------------------------------------------------
// Function for obtaining MaxSonar_Range
//----------------------------------------------------------------------------------------------------------

int GetSensorData1()

{
 // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
//  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
//  mm = (duration/2) / 2.91;     // Divide by 2.91 or multiply by 0.343
//  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  SensorData1  = (duration/2) / 2.91;
  
 // Serial.print(inches);
 // Serial.print("in, ");
 // Serial.print(mm);
 // Serial.print("mm, ");
 // Serial.print(cm);
 // Serial.print("cm");  
 // Serial.println();
  
 // delay(250);

 // SensorData1 = atoi(inString)*25.4;      //Convert Char array inString to integer
  sprintf(OutData, "%d",  SensorData1);   //Convert integer SensorData1 into OutData Char array
  return SensorData1;

}


//-------------------------------------------------------------------------------------------
// Interrupt I2C for Reading data from the I2C Bus
//----------------------------------------------------------------------------------------------------------

// I2C Link
void DataReceive(int numBytes)
{
  int i=0;
  char InData[I2C_Data_SIZE];
  memset(InData,0, I2C_Data_SIZE);
  while(Wire.available()) 
  { 
    InData[i++] = Wire.read();
  }

  
// the serial print could causes crash if works within the interrupt
// this didn't seems to be the problem
  Serial.println("Received: " + (String(InData)));

}

//-------------------------------------------------------------------------------------------
// Interrupt I2C for sending data (Outbound) to the I2C Bus
//----------------------------------------------------------------------------------------------------------
void DataRequest()
{
  byte resp[I2C_Data_SIZE];
  for (byte i=0; i<I2C_Data_SIZE; ++i) {
    resp[i] = (byte)OutData[i];
  }
  Wire.write(resp, sizeof(resp));

// the serial print could causes crash if works within the interrupt
// this didn't seems to be the problem
  Serial.println("Sent: " + (String(OutData)));

}


//-------------------------------------------------------------------------------------------
// Sending Random number instead of reading MaxSensor data seems to fix the stability issue
//----------------------------------------------------------------------------------------------------------

int GetRandomData2()
{
    RandomData2 = random(1, 255);
    sprintf(OutData, "%d",  RandomData2);   //Convert integer SensorData1 into OutData Char array

    return RandomData2;
}

