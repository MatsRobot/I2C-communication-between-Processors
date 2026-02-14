/*
  I2C_Link_With_Arduino.ino

   https://github.com/robotcreation
   Using MAxSonar on D11-RX and D12-TX
    RX = D11
    TX = D12

   I2C on A5-SCL and A4-SDA

  This module was build based on what was available on the internet
  It is very unstable and only works for a few minutes, after that the 
  main loop stops working.
  The MaxSensor on its own seems to be working ok, so a random number was generated was
  used instead of reading the MaxSensor and it seems that the problem
  is the combination of using interrupt and reading the data from MaxSensor.

  A number of potential problems were tried.
  1- using loops instead of delay() function: This was reported to be the main issue with 
     the interrupt as the delay(), millis() and writing to Serial.print and other time delays appear to crash the interrupt
  2- Using Strings sometimes making the program unstable as reading and writing to the stack can
      be mismanagged by the processor. All variables were replaced with the char array and a function
    was written to test them before implementing them to the code.

  This still didn't solve the problem.
  still to try: https://forum.arduino.cc/t/solved-crashing-after-several-hours-of-operation/591988/40

*/

// I2C parameters
#include <Wire.h>
#define Slave_Address 0x0A
#define I2C_Data_SIZE 32

// use char instead of string to avoid instability
char OutData[36];
char InData[36];

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
  port1.begin(9600);
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
  port1.flush();
  MSDelay (100);                 //Use loops to delay
  index = 0;
  port1.listen();
  MSDelay (50);                 //Use loops to delay
  while (port1.available())
    { // clear out buffer
      port1.read();
    }
  
  digitalWrite(start_pin1,HIGH);
  MSDelay (10);                 //Use loops to delay
  digitalWrite(start_pin1,LOW) ;

  while (port1.available() == 0){
    // do nothing - wait for a char 
    }

  while (port1.available())
  {   
    inChar = port1.read();
    //Serial.print(inChar);
    if (isDigit(inChar) && index < 32) 
      {
        inString[index] = inChar;   // Write Char to Index Position n
        index++;                    // Advance to the Next Index Position
        inString[index] = '\0';     // Write NULL Terminator - Last Char Stored in the Array will Always be NULL
      }
    //Serial.println(inString);
    while (port1.available() == 0)
      {
       if (inChar == '\r' || index > 32) {
        break;
       }
      }
  }  
  SensorData1 = atoi(inString)*25.4;      //Convert Char array inString to integer
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

