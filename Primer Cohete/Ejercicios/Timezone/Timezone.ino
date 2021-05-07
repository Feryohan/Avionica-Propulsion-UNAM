#include <NMEAGPS.h>

static NMEAGPS  gps; // This parses received characters
static gps_fix  fix; // This contains all the parsed pieces

#include <GPSport.h>

static void GPSloop()
{  
  while (gps.available( gpsPort )) {
    fix = gps.read();
    // Display the local time
    uint8_t showSeconds = fix.dateTime.seconds;
    uint8_t showMinutes = fix.dateTime.minutes;
    DEBUG_PORT.print(showMinutes);
    DEBUG_PORT.print(":");
    DEBUG_PORT.println(showSeconds);
  }

} 

void setup()
{
  DEBUG_PORT.begin(38400);
  while (!DEBUG_PORT)
    ;

  DEBUG_PORT.print( F("NMEAtimezone.INO: started\n") );
  DEBUG_PORT.println( F("Looking for GPS device on " GPS_PORT_NAME ) );
  DEBUG_PORT.println( F("Local time") );
  DEBUG_PORT.flush();
  
  gpsPort.begin(38400);
}

//--------------------------

void loop()
{
  GPSloop();
}
