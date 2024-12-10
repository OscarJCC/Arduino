#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <TinyGPS.h>
#include <SPI.h>
#include <SD.h>
#include <math.h>

LiquidCrystal_I2C lcd (0x27,16,2);

int rx = 8;
int tx = 9;

TinyGPS gps;
SoftwareSerial Serial_GPS (rx, tx);

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);

int i = 0, imax = 100, boton, l = 0, k = 0;
float flat = 0.0, flong = 0.0, flatP = 0.0, flongP = 0.0, flatV= 0.0, flongV = 0.0, dis = 0.0;
unsigned long age, date, time, chars = 0;
unsigned short sentences = 0, failed = 0;
static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
//static const double SALTILLO_LAT = 25.4232100, SALTILLO_LON = -101.0053000;
//static const double DURANGO_LAT = 14.7019400, DURANGO_LON = -92.2713900;

File archivo;
#define SSpin 10

void setup(){

  Serial.begin(115200);
  Serial_GPS.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Iniciando");
  lcd.setCursor(7,1);
  lcd.print("SD");
  delay(1000);
  lcd.clear();

  if(SD.begin(SSpin) == 0){
    lcd.print("Fallo Inico SD");
    delay(2000);
    lcd.clear();
    //return;
  }else{
    SD.remove("posCap.txt");
    lcd.setCursor(0,0);
    lcd.print("Inicio  Correcto");
    lcd.setCursor(7,1);
    lcd.print("SD");
    delay(1000);
    lcd.clear();
    //return;
  }
  
  //Serial.println(" ");
  //Serial.println("  Latitude      Longitude   ");
  //Serial.println("----------------------------");

  lcd.setCursor(3,0);
  lcd.print("Conectando");
  lcd.setCursor(6,1);
  lcd.print("GPS");
  delay(1000);
  while(sentences == 0){
    gps.stats(&chars, &sentences, &failed);
    smartdelay(0);
    if(sentences != 0){
      lcd.clear();
      lcd.setCursor(6,0);
      lcd.print("GPS");
      lcd.setCursor(3,1);
      lcd.print("Conectado");
      delay(1000);
      lcd.clear();
    }
  }

  pinMode(2, INPUT_PULLUP);

}

void loop(){

  boton = digitalRead(2);

  if (boton == 0){
    delay(500);
    lcd.clear();
    if (l == 0){
      l = 1;
    } else if (l == 1){
      l = 2;
    }else if (l == 2){
      l = 0;
    }
  }

  for(i = 0; i < imax; i++){
    gps.f_get_position(&flat, &flong, &age);
    smartdelay(0);
    flatP += flat;
    flongP += flong;
  }
  flatP = redondeo(flatP/imax,6);
  flongP =redondeo(flongP/imax,6);

  dis = haversine(flatP, flongP, flatV, flongV);

  if(l == 0){
    lcd.setCursor(0,0);
    lcd.print("Lat:");
    print_float(flatP, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
    lcd.setCursor(0,1);
    lcd.print("Long:");
    print_float(flongP, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
  }else if(l == 1){
    archivo = SD.open("posCap.txt",FILE_WRITE);
    if(k == 0){
      archivo.println("Latitud  -  Longitud  -  Distancia");
      dis = 0.0;
      k = 1;
    }
    archivo.print(flatP,6);
    archivo.print(" - ");
    archivo.print(flongP,6);
    archivo.print(" - ");
    archivo.println(dis,6);
    archivo.close();

    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Posicion");
    lcd.setCursor(3,1);
    lcd.print("Capturanda");
    flatV = flatP;
    flongV = flongP;
    l = 2;
    delay(500);
    lcd.clear();
  }else if(l == 2){
    lcd.setCursor(0,0);
    lcd.print("Distancia:");
    lcd.setCursor(0,1);
    lcd.print(dis,6);
    lcd.setCursor(15,1);
    lcd.print("m");
  }

  flatP = 0.0;
  flongP = 0.0;
  //gps.f_get_position(&flat, &flong, &age);
  //print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
  //print_float(flong, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
  //Serial.println();
}

static void smartdelay(unsigned long ms){
  unsigned long start = millis();
  do{
    while (Serial_GPS.available())
      gps.encode(Serial_GPS.read());
  } while (millis() - start < ms);
}

static void print_float(float val, float invalid, int len, int prec){
  lcd.print(val,prec);
  smartdelay(0);
}

static void print_int(unsigned long val, unsigned long invalid, int len){
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

static void print_date(TinyGPS &gps){
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    Serial.print("********** ******** ");
  else{
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
        month, day, year, hour, minute, second);
    Serial.print(sz);
  }
  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  smartdelay(0);
}

static void print_str(const char *str, int len){
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartdelay(0);
}

double haversine(double nlat, double nlong, double vlat, double vlong){
  double rTierra = 6371000.0; // Radio de la tierra en metros
  double phi1 = nlat*M_PI/180.0;
  double phi2 = vlat*M_PI/180.0;
  double delta1 = (vlat-nlat)*M_PI/180.0;
  double delta2 = (vlong-nlong)*M_PI/180.0;
  double a = pow(sin(delta1/2.0),2.0) + cos(phi1)*cos(phi2)*pow(sin(delta2/2.0),2.0);
  double c = 2*atan2(sqrt(a),sqrt(1.0-a));
  double d = rTierra*c;
  return d;
}

double redondeo(double numero, int decimales){
  double factor = pow(10,decimales);
  return round(numero * factor) / factor;
}