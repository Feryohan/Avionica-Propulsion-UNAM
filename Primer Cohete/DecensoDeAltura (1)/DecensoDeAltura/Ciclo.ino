

// Ciclo principal

void loop()
{
  Serial.println(contador);
  contador = contador + 1;
  
  for(int i=0; i<5; i++)
  {
    medirPresion();  //Valor de presion amacenado en <P>
    h = altura();    //Calculo de la altura
    registroDatos(); //Registro en SD de los datos
  }

  // --- Revisar si la velocidad es negativa ---
  //float velocidad = 1000*(h-h_ante)/(millis()-t_ante);  //velocidad  en [m/s]
  //if( velocidad < -0.2 )
    //Serial.println("Hubo decenso");
  // --- Fin de la revision en velocidad ---
  
  h_ante = h;         //Actualizar la altura anterior
  t_ante = millis();
  
  // *** Condicion para terminar la prueba ***
      if(millis()-t0 >= 30000)
      {
        digitalWrite(LEDR, HIGH);
        Serial.println("Fin de la ejecución");
        while(1);
      }    
  // *** Condicion para terminar la prueba ***
}
