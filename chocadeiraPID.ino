//Chocadeira caseira feita com arduino utilizando controle PID.
//Código implementado por Pedro Henrique Knup facebook.com/pedroknup
//Nov/2016


#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <TimerOne.h>
#include "PID_v1.h"




 
#include <dht.h>
#define dht_dpin A1 //Pino DATA do Sensor ligado na porta Analogica A1
dht DHT; //Inicializa o sensor

Adafruit_PCD8544 display = Adafruit_PCD8544(8,7,6,5,4);
Adafruit_BMP085 bmp;
int triac = 2; // Triac: pino D2
int pot = A5;  // Potenciometro: pino A5
int zc = 3;    // Detetor de zero: pino D3
int comparativo = 400;
volatile int t1=0;
int t;
int RECV_PIN = 11;



int POT=0;
unsigned long valorcode;
unsigned long repetir;
int incremento = 200;
int a=1;
int estado=1;
int comando;
int b=1;

int delayv = 2500;


bool passou = true;
int  grau = 0;
double variacao = 0;
double variancia = 100;
double temperatura = 0;
bool primeiro = true;
String estadoo="Desligado";
int ultimo  = 0;
int umidadedelay = 5;
int tempoa = 125;
bool mantendo=false;
int contmantendo = 0;
bool mudou = false;

#define kp 20
#define ki 2
#define kd  0
 
double SetPoint = 37.9;
double PresentValue, ManipulatedValue;

PID pid(&PresentValue, &ManipulatedValue, &SetPoint, kp, ki, kd, DIRECT);
 
volatile int power = 100;
// Tempo entre a deteccao de zero e o acionamento do triac
int time;


void setup()
{
  // Inicializa os pinos como entrada ou sa�da
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");

  }
  t=millis();
  DHT.read11(dht_dpin); 
  pinMode(triac, OUTPUT);
  pinMode(pot, INPUT);
 // pinMode(pinoRecep,INPUT);
 // pinMode(pinoRecp,INPUT);
  pinMode(zc, INPUT_PULLUP); //Entrada para deteccao de passagem por zero da rede eletrica.
  Serial.begin(9600); // Liga a serial
 // receptorIr.enableIRIn();
  digitalWrite(RECV_PIN,HIGH);
  // Associa a borda de descida do detetor de zero com
  //   a fun��o dimmer(), que aciona o triac
  attachInterrupt(1, passagem_por_zero, FALLING);
  noInterrupts(); 
  digitalWrite(triac,LOW);
  POT=analogRead(pot);
  refresh_temporizacao_Triac();
  interrupts();


  display.begin();
  display.setContrast(35); //Ajusta o contraste do display
  display.clearDisplay();   //Apaga o buffer e o display
  display.setTextSize(1);  //Seta o tamanho do texto
  display.setTextColor(BLACK); //Seta a cor do texto
  display.setCursor(0,0);  //Seta a posição do cursor
  display.println("Chocadeira");  

  //Texto invertido - Branco com fundo preto
  display.setTextColor(WHITE, BLACK); 
display.println("");
  display.println("ARDUINO");

  display.setTextSize(2); 
  display.setTextColor(BLACK);
  display.print("v1.0");
  display.display();

  pid.SetOutputLimits(150, 950);
  pid.SetMode(AUTOMATIC); 
 
  t=millis();

  
//  delay(2500);
}




void loop()
{
 
  
  if ((millis()-t) > 5000)
  {
    if (umidadedelay == 5){
    DHT.read11(dht_dpin); //Lê as informações do sensor
      umidadedelay = 0;
    }
  if (temperatura>=37.6 && temperatura <= 37.9)
    estadoo="Estavel";
  else if (temperatura < 37.6)
    estadoo="Aumentando";
  else 
    estadoo="Diminuindo";
   display.clearDisplay();   //Apaga o buffer e o display
  display.setTextSize(1);  //Seta o tamanho do texto
  display.setTextColor(BLACK); //Seta a cor do texto
  display.setCursor(0,0);  //Seta a posição do cursor
    if ((temperatura <37.5)||(temperatura > 37.9))
      display.setTextColor(WHITE, BLACK);
  display.print(" T: ");
  display.println(temperatura);
    display.setTextColor(BLACK);
  if ((DHT.humidity <65)||(DHT.humidity > 75)){
      display.setTextColor(WHITE, BLACK); 
  }
  display.print(" U: ");
  display.println(DHT.humidity);
  display.setTextColor(BLACK);
  display.println(" " + estadoo);
    if (comparativo>970)
    comparativo = 950;


  int percent = map(comparativo, 150,950, 0,60);
  display.print(" Lamp: ");
  display.print(percent);
  display.println("W");

  

  display.display();
    t=millis();
    
  } 

   temperatura = bmp.readTemperature();  
  PresentValue=temperatura;
  pid.Compute();
  power = ManipulatedValue;
  comparativo = power;

  Serial.println(power);
  int umidadedelay = 0;



  delay(300);


  refresh_temporizacao_Triac();

  

}

void passagem_por_zero(void)
{
  Timer1.initialize(time);
  Timer1.attachInterrupt(dimmer);
}  

void dimmer(void)
{
  if(estado==1)
  digitalWrite(triac,HIGH); 
  delayMicroseconds(10); 
  digitalWrite(triac, LOW);
  Timer1.stop();
  t1=time;// acrescentei esta linha para monitorar os pulsos no pino D3
}

void refresh_temporizacao_Triac(void)
{
if(comparativo<150)
comparativo=150;
if(comparativo>970)
comparativo=970;
t = map(comparativo, 0, 1000 , 8333  , 0);  
noInterrupts();
time=t;  	
interrupts();
}
