#include <Stepper.h>
#define STEPS 2048



//Sensor exterior
const int LDRPin1 = A0;

//Sensor interior
const int LDRPin2 = A1;
////////////////////////////////////////////////////////////////   LEDs

//Led mega
const int LEDPin1 = 13;

//LED Luz habitacion 
const int LEDPin2 = 7;

// LED Automatico / Manual
const int LEDPin3=6;

//////////////////////////////////////////////////////////////////  Botones



//Boton luz interior
const int boton1=3;

//Boton persianas
const int boton2=2;

//Boton modo automático
const int boton3=4;

/////////////////////////////////////////////////////////////////  Motor

//Motor paso a paso 28BYJ-48

Stepper motor1(2048,8,10,9,11); // pasos completos


//Conversion
const long A = 1000;     //Resistencia en oscuridad en KΩ
const int B = 15;        //Resistencia a la luz (10 Lux) en KΩ
const int Rc = 10;       //Resistencia calibracion en KΩ

int LuzF; // conversor sensor exterior;
int LuzD; // conversor sensor interior;
int umbral2;
const int umbral=450;
//Condicionales


int aux=0;
int aux2=0;
int aux3=0;
int aux4=0;
int aux5=0;
int aux6=0;

////////////////////////////////////////////////////////////////////////////////////////////////////// Funciones

void modoAutomatico(int mode, int luzHabitacion, int limite);
void modoManual(int es1,int es2);


///////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{

 
  
  Serial.begin(9600);  
  
  pinMode(boton1, INPUT_PULLUP);   
  pinMode(boton2, INPUT_PULLUP);
  pinMode(boton3, INPUT_PULLUP);
  
  pinMode(LDRPin1, INPUT);   
  pinMode(LDRPin2, INPUT);
  
  pinMode(LEDPin1, OUTPUT);
  pinMode(LEDPin2, OUTPUT);  //luz habitacion azul
  pinMode(LEDPin3, OUTPUT);   //luz manual-automático rojo

  
  motor1.setSpeed(2); // en RPM(valores de 1, 2 o 3 para el 28BYJ-48)


} 
 /////////////////////////////////////////////////////////////////////////////////////////////////////////
 
void loop()  // Stepper.step(20);
{
   int value = analogRead(LDRPin1);     //sensor exterior
   
   int value2= analogRead(LDRPin2);     //sensor interior
   
   int estado1= digitalRead(boton1);    //boton luz

   int estado2=digitalRead(boton2);     //boton persiana

   int estado3=digitalRead(boton3);     //boton automatico-manual

 


   
   umbral2= ((long)umbral*A*10)/((long)B*Rc*(1024-umbral)); 
   LuzF = ((long)value*A*10)/((long)B*Rc*(1024-value)); 
   LuzD = ((long)value2*A*10)/((long)B*Rc*(1024-value2));
  
   if(estado2==LOW || estado1==LOW || estado2!=LOW && aux3==1 || estado1!=LOW && aux3==1)
   {
      digitalWrite(LEDPin3,LOW);
      modoManual(estado1,estado2);
      aux3=1;
      if(estado3==LOW)
      {
        aux3=0;
      }
   }
   else
   {
      if(estado3==LOW && aux2==0 && aux3==0|| estado3!=LOW && aux2==1 && aux3==0)
       {
        
         modoAutomatico(LuzF,LuzD,umbral2);
         aux2=1;
       }
   }
   
    
   //Serial.println(umbral2);
   //Serial.println(LuzF);
   //Serial.println(LuzD);
 

   delay(500);
  
}


void modoAutomatico(int mode, int luzHabitacion, int limite)
{ 
  digitalWrite(LEDPin3,HIGH);
  delay(2000);
  
  if(mode<limite && aux==0 && luzHabitacion<limite)
  {
    digitalWrite(LEDPin2,HIGH);
    motor1.step(512);
    delay(2000);
    aux=1;
  }
  else if(aux==1 && mode>limite && luzHabitacion>limite)
   {
    digitalWrite(LEDPin2,LOW);
    motor1.step(-512);
    delay(2000);
    aux=0;
   }
  
}
void modoManual(int es1, int es2)
{
  
  
  
  if(aux4==0 && es1==LOW){
        digitalWrite(LEDPin2,HIGH);
        aux4=1;        
        delay(2000);
   }
   
   else if(aux4==1 && es1==LOW){
        digitalWrite(LEDPin2,LOW);
        aux4=0;
        delay(2000);
   }
   if(aux5==0 && es2==LOW){
        motor1.step(512);
        aux5=1; 
        delay(2000);       
   }
   
   else if(aux5==1 && es2==LOW){
        motor1.step(-512);
        aux5=0;
        delay(2000);
   }
   
}
