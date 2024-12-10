//UTILIZAR PLACA ARDUINO MICRO

#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <TinyGPS.h>
#include <SPI.h>
#include <SD.h>
#include <math.h>
//#include <math.h> //revisar su uso para rutinas de geomedicion

#define rx2 9 //Pin serial rx a tx del gps
#define tx2 8 //Pin serial tx a rx del gps
#define chipSelect 10 //Pin de CS
#define led_pin 17 //Pin de LED
#define push_button 2 //Pin del botón
#define max_cont 20 // número máximo de muestras de gps para promediar
#define nombre_archivo "datalog.txt"

TinyGPS gps;
SoftwareSerial ss(rx2, tx2);//ss(3, 2); //(2,3); ss(RX,TX); RX es el receptor en el micro, TX el transmisor en el micro
                                        //RXmicro --> TXreceptor
                                        //TXmicro --> RXreceptor

//VARIABLES GLOBALES
File dataFile;
float flat_ant=0.0;
float flong_ant=0.0;
float no_valido=0.0;


#define BACKLIGHT_PIN     13
#define I2C_ADDR  0x27//0x3f// Direcciones de lcd 0x27  0x3F 0x38 

//LiquidCrystal_I2C lcd(I2C_ADDR,2, 1, 0, 4, 5, 6, 7);
LiquidCrystal_I2C lcd(I2C_ADDR,16,2);
static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);
//VARIABLES GLOBALES
bool modo=0; //Modo de trabajo 0: toma mediciones y las graba si hay boton oprimido, 1: Graba cada medición
bool flag=0; //bandera de primera grabación
bool band_boton=0; //Bandera del boton
int band_tarj=0; //Bandera de que hay buena tarjeta.
float flat=0.0,  flon=0.0; //_ant se refiere a la longitud anterior 
float flat_prom=0.0, flong_prom=0.0,dist=0.0; //latitudes y longitudes promedio
unsigned long age, date, time, chars = 0;
unsigned short sentences = 0, failed = 0;
char cont;
static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

void setup()
{
  pinMode(led_pin,OUTPUT);
  pinMode(push_button,INPUT);
  digitalWrite(led_pin,HIGH);
  Serial.begin(115200); //Velocidad de conexión con la pc
  //lcd.begin (16,2);
  //lcd.setBacklight(3,POSITIVE);
  lcd.init();
  lcd.setBacklight(HIGH);
  lcd.home (); 
  lcd.clear();
  if (!SD.begin(chipSelect)) 
        lcd.print("Fallo de Tarjeta");
  else 
      {
       lcd.print("Tarj. Memoria Ok"); 
       band_tarj=1; //bandera, tarjeta memoria ok        
      }
              
  delay(500); 
  lcd.clear();
  lcd.print("INICIO");
  lcd.setCursor(0,1);
  for(cont=0;cont<10;cont++)
  {
    delay(500);
    if (digitalRead(push_button))
      modo=1;
  }
  lcd.clear();
  //lcd.setCursor(0,1);
  if(!modo)
  {
    lcd.print("PROMEDIOS GPS");
    delay(1000);
//    dataFile=SD.open(nombre_archivo,FILE_WRITE);
//    if(dataFile)
//       dataFile.println(0.00000000);
//    dataFile.close();
    
  }
  else
  {
    if(band_tarj)
    {
      band_boton=0;
      digitalWrite(led_pin,HIGH);
      lcd.home();
      lcd.print("¿Borrar datos?");
      for(cont=5;cont>=1;cont--)
      {
        lcd.setCursor(0,0);
        lcd.print("Presione boton  ");
        checa_boton();
        delay(1000);
      }
     // if (band_boton) {
  //SD.remove(nombre_archivo);
  //lcd.clear();
  //lcd.print("Datos Borrados");
  //lcd.setCursor(0, 1);
  //lcd.print("Apagar ");
  //while (1);
//}
    }
    else
    {
      lcd.clear();
      lcd.print("Revise Tarjeta");
      lcd.setCursor(0,1);
      lcd.print("Apagar ");
      while(1);
    }
  }
  delay(500);
  lcd.clear();      
  ss.begin(9600);
  no_valido=TinyGPS::GPS_INVALID_F_ANGLE;
  digitalWrite(led_pin, HIGH);
  flat_prom=0.0;
  flong_prom=0.0;
}

void loop()
{    
    lcd.clear();
    lcd.print("CALCULANDO GPS");
    lcd.setCursor(0,1);
    lcd.print("Buscando GPS");
    band_boton=0;
    for(cont=0;cont<max_cont;cont++)
    {
       flat=no_valido;
       //checa_boton();
       while(flat==no_valido)
       {
          gps.f_get_position(&flat, &flon, &age);
          //checa_boton();
          lcd.setCursor(0,1);
          Serial.print("Dirección ");
          Serial.print(age);
          Serial.print(" ");
          Serial.println(flon);
          smartdelay(1000);
       }             
       lcd.print("OK SENAL GPS");
       flat_prom+=flat;  //cuenta las posiciones para promedios
       flong_prom+=flon;
       
    }   
    flat_prom=flat_prom/max_cont;
    flong_prom=flong_prom/max_cont;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Lat: ");
    print_float(flat_prom, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
    lcd.setCursor(0,1);
    lcd.print("Long: ");
    print_float(flong_prom, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
    delay(2000);
    lcd.clear();
    lcd.print("Dist: ");
    //lcd.setCursor(0,1);
    dist=haversine(flat_prom, flong_prom, flat_ant, flong_ant);
    lcd.print(dist,2);
    lcd.print(" m");
    delay(2000);
    digitalWrite(led_pin,HIGH);
    Serial.println();
  
    //smartdelay(1000); //REVISAR
  if(band_boton)
  {
    lcd.clear();
    lcd.print("Guardando Posicion");
    dataFile=SD.open(nombre_archivo,FILE_WRITE);
    if(dataFile)
    {
      if(!flag)
      {
        dataFile.println("Latitud,Longitud,Distancia ");
        dist=0.0;
        flag=1;
      }
      dataFile.print(flat_prom,6);
      delay(50);
      dataFile.print(",");
      delay(50);
      dataFile.print(flong_prom,6);
      delay(50);
      dataFile.print(",");
      delay(50);
      dataFile.println(dist,2);
      delay(50);
      dataFile.close();
    }
    lcd.setCursor(0,1);
    lcd.print("  OK  ");
    delay(1000);
    lcd.clear();
    flat_ant=flat_prom;
    flong_ant=flong_prom;
    band_boton = 0; //reinicia la bandera del botón
  }
  flat_prom=0.0;
  flong_prom=0.0;
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void print_float(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    lcd.print(val,prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartdelay(0);
}

static void print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartdelay(0);
}

static void print_date(TinyGPS &gps)
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    Serial.print("********** ******** ");
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
        month, day, year, hour, minute, second);
    Serial.print(sz);
  }
  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  smartdelay(0);
}

static void print_str(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartdelay(0);
}

void checa_boton()
{
  if (digitalRead(push_button))
       {
          band_boton=1;
          digitalWrite(led_pin,LOW);
       }
}


// Returns the great-circle distance (in meters) between two points on a sphere
// lat1, lat2, lon1, lon2 must be provided in Degrees.  (Radians = Degrees * PI / 180, Degrees = Radians / PI * 180)
double haversine(double lat1, double lon1, double lat2, double lon2) {
    const double rEarth = 6371000.0; // in meters
    double x = pow( sin( ((lat2 - lat1)*M_PI/180.0) / 2.0), 2.0 );
    double y = cos(lat1*M_PI/180.0) * cos(lat2*M_PI/180.0);
    double z = pow( sin( ((lon2 - lon1)*M_PI/180.0) / 2.0), 2.0 );
    double a = x + y * z;
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0-a));
    double d = rEarth * c;
    // Serial.printlnf("%12.9f, %12.9f, %12.9f, %12.9f, %12.9f, %12.9f", x, y, z, a, c, d);
    return d; // in meters
}
/* Notas del proyecto.
 *  
 *  Se tomaron del ejemplo gps, se combinó con el display.
 *  Despliega latitud y longitud
 *  Ideas:
 *        Desplegar el número de medición en proceso, recibirlo de la computadora.
 *        Probar con la otra antena.
 * 
 * */
 
