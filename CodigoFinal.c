#include <Stepper.h>
#define STEPS 2048



//Sensor luminosidad exterior
const int LDRPin1 = A0;

//Sensor luminosidad interior
const int LDRPin2 = A1;
////////////////////////////////////////////////////////////////   LEDs


//LED Luz habitacion 
const int LEDPin2 = 7;

// LED Automatico / Manual
const int LEDPin3=6;

//////////////////////////////////////////////////////////////////  Botones

/////MODO MANUAL (entra en modo manual y sale de automatico( en caso de que esté) a la hora de pulsar cualquiera de estos botones)

//Boton luz interior
const int boton1=3;

//Boton persianas
const int boton2=2;


/////MODO AUTOMATICO

//Boton modo automático
const int boton3=4;

/////////////////////////////////////////////////////////////////  Motor

// Declaración Motor paso a paso 28BYJ-48

Stepper motor1(2048,8,10,9,11); // pasos completos

/////////////////////////////////////////////////////////////////Conversion a luxes
const long A = 1000;     //Resistencia en oscuridad en KΩ
const int B = 15;        //Resistencia a la luz (10 Lux) en KΩ
const int Rc = 10;       //Resistencia calibracion en KΩ


int LuzF; // variable de conversión a luxes sensor luminosidad exterior;
int LuzD; // variable de conversión a luxes sensor luminosidad interior;
int limite1;  // variable de conversión a luxes umbral superior;
int limite2;  // variable de conversión a luxes umbral inferior;
const int umbral_alto=540;
const int umbral_bajo=470;
//////////////////////////////////////////////////////////////////Auxiliares

int auxPersiana=0; // 0 cerrada, 1 media abierta, 2 abierta;
int auxLuz=0;      //0 apagada, 1 medio encendida, 2 encendida;

int auxAutomatico=0;        //mantener el estado en automatico
int auxManual=0;        //mantener el estado en manual

////// Auxiliares Sensor luminosidad exterior
int dia=0;         //Estado de alta luminosidad
int medioDia=0;    //Estado de luminosidad intermedia
int noche=0;       //Estado de luminosidad baja

//////Auxiliar Sensor luminosidad interior

int luzON_OFF=0;    //Luz habitacion encendida(1) o apagada(0)

///////////////////////////////////////////////////////Impresión por pantalla del cambio de modo

int imprime1=0;  // entero auxiliar para la impresion del tipo de modo una vez entre en el
int imprime2=0;  // entero auxiliar para la impresion del tipo de modo una vez entre en el
////////////////////////////////////////////////////////////////////////////////////////////////////// Funciones

void modoAutomatico(int mode, int luzHabitacion, int lim1, int lim2);
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
  
  pinMode(LEDPin2, OUTPUT);  //luz habitacion azul
  pinMode(LEDPin3, OUTPUT);   //luz manual-automático rojo

  
  motor1.setSpeed(2); // en RPM(valores de 1, 2 o 3 para el 28BYJ-48)


} 
 /////////////////////////////////////////////////////////////////////////////////////////////////////////
 
void loop()  // Stepper.step(20);
{
   int value = analogRead(LDRPin1);     //sensor de luz del exterior de la habitacion   
   int value2= analogRead(LDRPin2);     //sensor de luz del interior de la habitacion   
   int estado1= digitalRead(boton1);    //modo manual, encender o apagar la luz de dentro de la habitacion
   int estado2=digitalRead(boton2);     //modo manual, subir bajar persiana.
   int estado3=digitalRead(boton3);     //boton automatico-manual
  
   //Conversor a luxes
   
   limite1= ((long)umbral_alto*A*10)/((long)B*Rc*(1024-umbral_alto));
   limite2= ((long)umbral_bajo*A*10)/((long)B*Rc*(1024-umbral_bajo));
   
   LuzD = ((long)value*A*10)/((long)B*Rc*(1024-value)); 
   LuzF = ((long)value2*A*10)/((long)B*Rc*(1024-value2));
  
   if(estado2==LOW || estado1==LOW || estado2!=LOW && auxManual==1 || estado1!=LOW && auxManual==1)  //MODO MANUAL
   {
      digitalWrite(LEDPin3,LOW); //entrar en modo manual y se apagan el led rojo
      imprime1=0;
      modoManual(estado1,estado2);
      auxManual=1;
      if(estado3==LOW)
      {
        auxAutomatico=0;
        auxManual=0;
      }
   }
   else  //MODO AUTOMÁTICO
   {
      if(estado3==LOW && auxAutomatico==0 && auxManual==0|| estado3!=LOW && auxAutomatico==1 && auxManual==0)
       {
         modoAutomatico(LuzF,LuzD,limite1,limite2);
         auxAutomatico=1;
         imprime2=0;
       }
   }
  
}


void modoAutomatico(int mode, int luzHabitacion, int lim1, int lim2)
{ 
 
  if(imprime1==0)
  {
    Serial.println("MODO AUTOMATICO ACTIVADO\nSe enciende el led rojo de modo automatico");
    imprime1=1;
  }
  

  //Se enciende el led rojo, indicando que está en modo automático
  
  digitalWrite(LEDPin3,HIGH);
  delay(1000);

  
 if(mode>lim1 && dia==0)  // Si es de día (el sensor de luz detecta luz exterior alta) queremos abrir la persiana
  {
    Serial.print("Luminosidad alta:\nEl sensor de luz externo detecta ");
    Serial.print(mode);
    Serial.println(" luxes. ");
    if(auxPersiana==0)      //y la persiana está bajada
    {
      motor1.step(512);
      Serial.println("Se sube la persiana.");
      delay(1000);
      auxPersiana=2;
    }
    else if(auxPersiana==1) //y la persiana está a la mitad
    {
      Serial.println("La persina estaba a la mitad de recorrido y se sube del todo.");
      motor1.step(256);
      delay(1000);
      auxPersiana=2;
    }
    else                   //ya está subida la persiana
    {
      delay(1000);
      auxPersiana=2;
    }
    dia=1;
    medioDia=0;
    noche=0;
  }
   else if(mode<lim1 && mode>lim2 && medioDia==0) //se detecta una cantidad de luz exterior media, abrimos la persiana a la mitad
  {
    Serial.print("Luminosidad media:\nEl sensor de luz externo detecta ");
    Serial.print(mode);
    Serial.println(" luxes.");
    if(auxPersiana==0)      
    {
      Serial.println("La persiana sube hasta la mitad");
      motor1.step(256);
      delay(1000);
      auxPersiana=1;
    }
    else if(auxPersiana==1) 
    {
      delay(500);
      auxPersiana=1;
    }
    else                   
    {
      Serial.println("La persiana baja hasta la mitad.");
      motor1.step(-256);
      delay(1000);      
      auxPersiana=1;
    }
    dia=0;
    medioDia=1;
    noche=0;
  }
  else if(noche==0 && mode<lim2) //si es de noche(no hay luz en el exterior) , cerramoa la persiana                                           
  {
    Serial.print("Luminosidad baja:\nEl sensor de luz externo detecta ");
    Serial.print(mode);
    Serial.println(" luxes");
     if(auxPersiana==0)      
    {
      delay(1000);
      auxPersiana=0;
    }
    else if(auxPersiana==1) 
    {
      Serial.println("Se cierra la persiana partiendo de de quee está a la mitad del recorrido.");
      motor1.step(-256);
      delay(1000);
      auxPersiana=0;
    }
    else                   
    {
      Serial.println("La persiana estaba abierta y se cierra.");
      motor1.step(-512);
      delay(1000);
      auxPersiana=0;
    }
   noche=1;
   dia=0;
   medioDia=0;
  }


  //////////////////////////////////////////////////////////////////////////
  
  if(luzHabitacion>lim2 && luzON_OFF==1 && auxLuz==1)  
  {
    Serial.print("El sensor de luz interior de la habitacion detecta ");
    Serial.print(luzHabitacion);
    Serial.println(" luxes");
    Serial.println("Se apaga la luz de la habitacion.");
    
    digitalWrite(LEDPin2,LOW);
    delay(1000);
    auxLuz=0;
    luzON_OFF=0;
    
   
  }
   else if(luzHabitacion<lim2 && luzON_OFF==0 && auxLuz==0)//En la habitacion no hay luz asique encendemos el led                                       
  {
    Serial.print("El sensor de luz interior de la habitacion detecta ");
    Serial.print(luzHabitacion);
    Serial.println(" luxes.");
    Serial.println("Se enciende la luz de la habitacion(led azul).");
    digitalWrite(LEDPin2,HIGH);
    delay(500);
    auxLuz=1;
    luzON_OFF=1;
    
  }

///////////////////////////////////////////////////////////////////
 
  
}
void modoManual(int es1, int es2)
{

  if(imprime2==0  )
  {
    Serial.println("MODO MANUAL ACTIVADO");
    if(auxAutomatico==1){
      Serial.println("Se apaga el led rojo del modo automatico.");
    }
    imprime2=1;
    
  }
  

  
  ///////////////////////////////////////////////BOTON ENCENDIDO Y APAGADO DE LUZ INTERIOR
  if(auxLuz==0 && es1==LOW){
        Serial.println("Boton luz pulsado.\nSe enciende el led azul.");
        
        
        digitalWrite(LEDPin2,HIGH);
        auxLuz=1;        
        
   }
   
   else if(auxLuz==1 && es1==LOW){
        Serial.println("Boton luz pulsado.\nSe apaga el led azul.");
        digitalWrite(LEDPin2,LOW);
        auxLuz=0;
       
   }

   /////////////////////////////////////////////////BOTON SUBIR/BAJAR PERSIANA
   
   if(auxPersiana==0 && es2==LOW){
        Serial.println("Boton persiana pulsado.\nSe abre la persiana");
        
        motor1.step(512);
        auxPersiana=2; 
        delay(500);
               
   }   
   else if(auxPersiana==1 && es2==LOW){
       Serial.println("Boton persiana pulsado.");
       Serial.println("La persiana estaba abierta a la mitad y sube hasta arriba.");
        motor1.step(256);
        auxPersiana=2;
        delay(500);
   }
   else if(auxPersiana==2 && es2==LOW){
      Serial.println("Boton persiana pulsado.\nSe cierra la persiana.");
        motor1.step(-512);
        auxPersiana=0;
        delay(500);
   }
   
}
