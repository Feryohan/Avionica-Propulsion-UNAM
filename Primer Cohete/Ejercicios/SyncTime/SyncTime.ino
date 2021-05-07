#include <NMEAGPS.h>
#include <GPSport.h>

static NMEAGPS gps;
static gps_fix fix;

static const uint32_t CLOCK_INTERVAL_MS = 100UL;
static       uint32_t lastShowTime = CLOCK_INTERVAL_MS+1; // First time never matches

//----------------------------------------------------------------

static void showTime( uint16_t subs, uint16_t factor = 100 /* hundredths */ )
{
  uint8_t showSeconds = fix.dateTime.seconds;
  
  // Step by seconds until we're in the current UTC second
  while (subs >= 1000UL) {
    subs -= 1000UL;
    if (showSeconds < 59)
      showSeconds++;
    else
      showSeconds = 0;
    //DEBUG_PORT.print( '+' );
  }

  DEBUG_PORT.print( showSeconds );
  DEBUG_PORT.print( '.' );
  
  // Leading zeroes
  for (;;) {
    factor /= 10;
    if ((factor < 10) || (subs >= factor))
      break;
    DEBUG_PORT.print( '0' );
  }

  DEBUG_PORT.println( subs );

} // showTime

//--------------------------

void setup()
{
  DEBUG_PORT.begin(9600);
  while (!DEBUG_PORT)
    ;

  DEBUG_PORT.print( F("SyncTime.INO: started\n") );
  DEBUG_PORT.println( F("Looking for GPS device on " GPS_PORT_NAME) );
  DEBUG_PORT.println( F("Local time seconds.milliseconds") );
  DEBUG_PORT.flush();

  gpsPort.begin( 9600 );
}

//--------------------------

void loop()
{
  while (gps.available( gpsPort )) {
    fix = gps.read();
  }

  if (fix.valid.time) {
    uint32_t UTCms = gps.UTCms();

    if (((UTCms % CLOCK_INTERVAL_MS) == 0) && (UTCms != lastShowTime)) {
      showTime( UTCms, 1000 );
      lastShowTime = UTCms;
    }
  }
}
