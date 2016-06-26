//Importamos las librerías necesarias para el funcionamiento
#include <SPI.h>
#include <Ethernet.h>
#include <Twitter.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);//Declaramos los pines que usa nuestra pantalla(RS,E,D4,D5,D6,D7)

////////////////////IMPORTANTE////////////////////
//Valor a modificar para ajustar cuando es de día y de noche.
#define VALOR_DE_DIA 600
#define VALOR_NOCHE 1000

// Ethernet Shield MAC
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};


// Introduzca su token(puede obtenerlo aqui: https://api.twitter.com/oauth/authorize?oauth_nonce=5894605197090105045&oauth_timestamp=1458138817&oauth_consumer_key=oQA2jr32rWowM4SpGB64yQ&oauth_signature_method=HMAC-SHA1&oauth_version=1.0&oauth_token=ZehHdQAAAAAAAViGAAABU3_YpMg&oauth_signature=semdJvg%2BAAI0PN7Qmpv%2FUQPr19U%3D)
Twitter twitter("1382631355-8B1jXvI0WVGYV3o3hRxkECGyVqDYoiVN4l9p8NN");

int ldrPin = A0; //Almacenamos el pin del sensor de humedad/temperatura
//Almacenamos tanto el valor antiguo como el nuevo para diferenciar entre el encendido de luces y el amanecer
int antiguoValor = 0;
int nuevoValor = 0;

int control = 0; //Variable para evitar que envie tweets más de una vez al día

//Almacenamos los mensajes que aleatoriamente vamos a mostrar posteriormente
String mensajes[] = {"Buenos dias, ya está amaneciendo en Espacio Res", "Buenos dias, ya es de día en Espacio Res", "Ya empezamos a trabajar en Espacio Res"};

//Iniciamos el sensor de Temperatura/humedad
#include "DHT.h"//Incluye libraría de control del sensor
#define DHTPIN 8//Defiene el pin al que se conectará el sensor
#define DHTTYPE DHT11//Seleciona el tipo de sensor
DHT dht(DHTPIN, DHTTYPE);//Configura la libraría
int temperatura = 0;
int humedad = 0;

void setup() {
  delay(1000);
  Serial.begin(9600);
  Ethernet.begin(mac); //Iniciamos la libreria Ethernet

  lcd.begin(16, 2);
  randomSeed(analogRead(5)); //Definimos el pin analógico 5 para la generación de numeros aleatorios
  Serial.println("inicializado");
}

void loop() {
  delay(5); //Añadimos un pequeño delay para no desbordar la libreria de twitter
  nuevoValor = analogRead(ldrPin); //Leemos el nuevo valor

  //Actualizamos el valor antiguo si no se había inicializado o si el incremento está dentro de los límites
  if (antiguoValor == 0 || (antiguoValor - nuevoValor > -50 && antiguoValor - nuevoValor < 50)) {
    antiguoValor = nuevoValor;
  }

  //Enviamos un tipo de mensaje si amanece
  if (antiguoValor != 0 && (antiguoValor - nuevoValor) < -50 && control) {
    control = 0;
    Serial.println("es de dia");
    postTweet();
  }
  //Enviamos un tipo de mensaje si detecta que se encienden las luces
  if ((antiguoValor - nuevoValor) > -50 && nuevoValor > VALOR_DE_DIA && control) {
    control = 0;
    Serial.println("encienden luces");
    postTweet();
  }

  //mostrar por la pantalla LCD la temperatura y humedad
  humedad = dht.readHumidity(); //Lee la humedad
  temperatura = dht.readTemperature(); //Lee la temperatura

  lcd.setCursor(0, 0); //Situamos el lugar donde empezará la escritura
  lcd.print("HUMEDAD"); //Escribimos HUMEDAD en la LCD
  lcd.setCursor(12, 0); //Situamos el lugar donde empezará la escritura.
  lcd.print(humedad); //Representamos el valor de la variable Humedad.
  lcd.setCursor(14, 0); //Situamos el lugar donde empezará la escritura
  lcd.print("%"); //Escribimos % en la LCD
  lcd.setCursor(0, 1); //Situamos el lugar donde empezará la escritura
  lcd.print("TEMPERATURA"); //Escribimos TEMPERATURA en la LCD
  lcd.setCursor(12, 1); //Situamos el lugar donde empezará la escritura
  lcd.print(temperatura); //Representamos el valor de la variable Temperatura.
  lcd.setCursor(14, 1); //Situamos el lugar donde empezará la escritura
  lcd.print("C"); //Escribimos C en la LCD
  delay(100); //Esperamos 100 milisegundos

  if (nuevoValor > VALOR_NOCHE) {
    control = 1;
  }
}

void postTweet()
{
  Serial.println("connecting ...");
  char mensaje[140];
  int randomNumber = random(sizeof(mensajes) / 6);
  mensajes[randomNumber].toCharArray(mensaje, 140);
  //sprintf(mensaje, mensajes[randomNumber]);

  Serial.println(mensaje);

  if (twitter.post(mensaje)) {
    int status = twitter.wait();
    if (status == 200) {
      Serial.println("OK.");
    } else {
      Serial.print("failed : code ");
      Serial.println(status);
    }
  } else {
    Serial.println("connection failed.");
  }
  delay(1000);
}
