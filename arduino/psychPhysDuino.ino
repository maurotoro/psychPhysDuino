/*
  Adquisition system for respiratory frequency, skin temperature and synchronized 
  qith data aquisition by 3 phothodiodes
  
  When plugged, arduino will wait for a 'C' character from the pc, after that,
  starts sendig a line of data at 115200 bps by the USB serial port to the pc.
  Data its ordered like this:

  timeStamp'\t'photodiodeA'\t'photodiodeB'\t'photodiodeC'\t'temperature'\t'respiratoryBand'\r\n'
  

  soyKo-Pe 2013, 
*/

#include <Streaming.h>  //Inlude library to stream data over serial port easily ()
#include <StopWatch.h>  //Inlude stopwatch creating library
#include <OneWire.h>    //Include oneWire library to use with the DS18B20 temperature sensor
#include <DallasTemperature.h> //Inlcude library to control the DS18B20

#define ONE_WIRE_BUS 5   //Pin for  DS18B20
OneWire oneWire(ONE_WIRE_BUS); //reads oneWire pin
DallasTemperature sensors(&oneWire); //reads the oneWire pin as a DS18B20
DeviceAddress tempDeviceAddress;
int  resolution = 12;   //Sets the resolution to the DS; 
                        //[9 10 11 12] bits -> [.5 .25 .125 .0625] C 

StopWatch sensePh;      //Creates stopwatch to use along the sesion

const int resp=0;       //Conenct the respiratory band pin here, A0
const int pha=7;        //Connect photodiode here, 1st
const int phb=9;        //Connect photodiode here, 2nd
const int phc=11;       //Connect photodiode here, 3rd

unsigned long time;     //timestamps are 32 bits, max time ~49 days 
int dat;                //To save serial data send to arduino, poorly used now
int valPHa;             //1st photodiode value
int valPHb;             //2nd photodiode value
int valPHc;             //3rd photodiode value
float valTemp;          //Temperature sensor
volatile int rawVal=0;  //Respiratory band sensor pin raw analog value

void setup()
{
  Serial.begin(115200);      //Serial port starts at  115200 bps
  analogReference(EXTERNAL);
  bitWrite(ADCSRA,ADPS2,1);  //Changes the prescaling factor of the ADC 
  bitWrite(ADCSRA,ADPS1,0);
  bitWrite(ADCSRA,ADPS0,0);
  pinMode(resp,INPUT);       //Sensors and photodiodes as input pins
  pinMode(pha,INPUT);
  pinMode(phb,INPUT);
  pinMode(phc,INPUT);
  sensors.begin();           //DS begin
  sensors.setWaitForConversion(false); //Doesn't wait for conversion end of DS
  sensors.setResolution(tempDeviceAddress, resolution);  //Sets the resolution to 0.0625 C precition
}

void sense(){
/*
 * This function reads the sensors and update the variables values at fixes
 * intervals, returning a timestamp also
 * Photodiodes and Respiratory Band are sampled every 8ms
 * Temperature sensor it's sampled every 731 ms
 */
    if (sensePh.elapsed() !=0 && sensePh.elapsed()%8==0){
        time=millis();
        valPHa=digitalRead(pha);
        valPHb=digitalRead(phb);
        valPHc=digitalRead(phc);
        rawVal=analogRead(resp);
        Serial << time << "\t" << valPHa << "\t" << valPHb << "\t" << valPHc << "\t";
        Serial << valTemp << "\t";
        Serial << rawVal;
        Serial << endl;
        delay(1);
    }/*
    if (sensePh.elapsed() !=0 && sensePh.elapsed()%80==0)
    {
      rawVal=analogRead(resp);
    }*/
    if (sensePh.elapsed() !=0 && sensePh.elapsed()%731==0){
        sensors.requestTemperatures();
      valTemp=sensors.getTempCByIndex(0);
    }
}

void loop()
/*
 * This function runs forever, waiting for a C over serial port from pc, when
 * it gets it, starts the sensing routine
*/

{
  if (Serial.available()>0){
      dat=Serial.read();
      if (dat=='C'){
          sensePh.start();
          while (1){
              sense();
              if (Serial.available()>0){
                  dat=Serial.read();
                  if (dat=='F'){
                      sensePh.stop();
                      sensePh.reset();
                      break;
                  }
              }
          }
      }
  }
}
