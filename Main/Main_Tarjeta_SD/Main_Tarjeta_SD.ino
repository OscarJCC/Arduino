#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <SPI.h> // Biblioteca para comunicacion de base de datos
#include <SD.h> // Biblioteca para el manejo de la tarjeta SD

#define SSpin 10 // Definimos varible con el pin que va conectado al < CS >

File archivo; // Creamos objeto del tipo File

int m = 2000;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.print("Iniciando SD ....");
  Serial.println("Iniciando SD ...");
  delay(1500);
  lcd.clear();

  if (SD.begin(SSpin) == 0){ // Inicia la tarjeta con SD.begin(Variable del pin)
    lcd.print("Fallo Inico SD");
    Serial.println("Fallo Inico SD");
    delay(1500);
    lcd.clear();
    return;
  }

  SD.remove("Prueba.txt"); // Elimina el archivo de el nombre indicado
  lcd.print("Inicio Correcto");
  Serial.println("Inicio Correcto");
  delay(1500);
  lcd.clear();

  // Abre un archivo
  // FILE_WRITE -> 
  // 
  archivo = SD.open("Prueba.txt",FILE_WRITE);

  // si SD.open() falla el objeto archivo tendra un voliano = FALSE
  // Si no falla tiene un voliano = TRUE
  // archivo == TRUE
  if(archivo != 0){
    archivo.println("Prueba"); // Escribe un texto en la primera linea del archivo
    lcd.print("Escribiendo ....");
    Serial.println("Escribiendo ....");
    delay(1500);
    lcd.clear();

    Serial.println("Contenido en Prueba.txt");
    //    archivo.available() == TRUE
    while(archivo.available() != 0){ // Available retorna TRUE mientas exita contenido en el archivo
      Serial.write(archivo.read()); // read() lee el contenido del archivo de uno por un caracter y con Serial.write se imprime como variable tipo char
    }

    archivo.close(); // Cerramos el archivo
    lcd.print("Archivo Cerrado");
    Serial.println("Archivo Cerrado");
    delay(1500);
    lcd.clear();
  }
  else{
    lcd.print("ERROR prueba.txt");
    Serial.println("ERROR prueba.txt");
    delay(1500);
    lcd.clear();
  }
}

void loop() {
}