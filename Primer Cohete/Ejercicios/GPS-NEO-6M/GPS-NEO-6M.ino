//Conexiones GPS Neo-6m
//     VCC -> 3.3V
//     GND -> GND
//     TXD(Transmisión) -> 4 (Recepción)
//     RXD(Recepción) -> 3 (Transmisión)
//     PPS

#include <SoftwareSerial.h>
#include <TinyGPS.h>

TinyGPS gps;
               //RX  TX
SoftwareSerial ss(4, 3);

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Testing TinyGPS library v. "); 
  Serial.println(TinyGPS::library_version());
  Serial.println("by Mikal Hart");
  Serial.println();
  Serial.println("Holaaaa ");
 // Serial.println("Sats HDOP Latitude  Longitude  Fix  Date     Time   Date Alt     Course Speed Card  Distance Course Card  Chars  Sentences  Checksum");
  Serial.println("          (deg)     (deg)      Age                  Age  (m)     --- from GPS ----  ---- to London ----   RX      RX        Fall");
  Serial.println("------------------------------------------------------------------------------------------------------------------------------------");

  ss.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do{
    while(ss.available())
      gps.encode(ss.read());
  } while(millis() - start < ms);
}

static void print_float(float val, float invalid, int len, int prec)
{
  if(val == invalid)
  {
    while(len-->1)
      Serial.print('*');
      Serial.print(' ');
  }
  else{
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for(int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartdelay(0);
}

static void print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val==invalid)
    strcpy(sz, "*******");
  else 
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for(int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if(len > 0)
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
  if(age==TinyGPS::GPS_INVALID_AGE)
    Serial.print("********** ********");
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d", month, day, year, hour, minute, second);
    Serial.print(sz);  
  }
  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  smartdelay(0);
}

static void print_str(const char *str, int len)
{
  int slen = strlen(str);
  for(int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartdelay(0);
}
