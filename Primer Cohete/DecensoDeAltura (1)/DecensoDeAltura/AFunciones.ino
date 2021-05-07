
// DECLARACION DE FUNCIONES


// --- Funcion de calibracion ---
void calibracion()
{
  // Tomaremos una muestra de 350 valores
  //  con los que se obtendra un promedio <prom> 
  //   para tomar como valor de altura inicial
  float sum = 0;     //Suma de datos
  int N = 350;
  
  // --- Inicio de ciclo ---
  for(int i=0; i<N; i++)
  {
    status = bmp180.startTemperature();  // Pedimos que mida la temperatura
    if(status != 0)                    // Si el no hay error
    { 
      delay(status);                  // Damos tiempo para hacer la medicion
      status = bmp180.getTemperature(T); // Guardamos el valor de temperatura en <T>
      if(status != 0)                  // Si el no hay error
      {
          // ---- Obtencion de presion ----
          status = bmp180.startPressure(3); // Pedimos que mida la presion, resolucion nivel 3
          if (status != 0)                // Si el no hay error
          {
              delay(status);               // Damos tiempo para hacer la medicion  
              status = bmp180.getPressure(P,T); // Guardamos el valor de presion en <P>
              if (status != 0)                // Si el no hay error
              {                  
                  sum = sum + P;
              }
          }// --- Fin de obtencion de presion ---
      }
    }
  }// --- Fin del ciclo ---

  prom = sum/N;
  h_ante = 0;
  t_ante = millis();
  
}// --- FIN ---




// --- Funcion de Medicion de Presion ---
void medirPresion()
{
  status = bmp180.startTemperature();  // Pedimos que mida la temperatura
  if(status != 0)                    // Si el no hay error
  { 
    delay(status);                  // Damos tiempo para hacer la medicion
    status = bmp180.getTemperature(T); // Guardamos el valor de temperatura en <T>
    if(status != 0)                  // Si el no hay error
    {
        // ---- Obtencion de presion ----
        status = bmp180.startPressure(2); // Pedimos que mida la presion, resolucion nivel 2
        if (status != 0)                // Si el no hay error
        {
            delay(status);               // Damos tiempo para hacer la medicion  
            double Pp;
            status = bmp180.getPressure(Pp,T); // Guardamos el valor de presion en <P>
            if (status != 0)                // Si el no hay error
            {                  
              P = Pp;
            }
        }// --- Fin de obtencion de presion ---
    }
  }
}// --- FIN ---




// --- Funcion de calculo de la altura ---
float altura()
{
  float a = 29.3515234*(T+273.15)*log(prom/P);
  return a;
}// --- FIN ---




// --- Funcion de registro de datos ---
void registroDatos()
{
  Datos = SD.open("Datos" + String(nFile) + ".txt",FILE_WRITE);           
  if(Datos)
  {
    n++;
    Datos.print(n);
    Datos.print(",");
    Datos.print(millis());
    Datos.print(",");
    Datos.print(T);
    Datos.print(",");
    Datos.print(P);
    Serial.println(P);
    Datos.print(",");
    Datos.println(h);
    Datos.close();
  }
}// --- FIN ---
