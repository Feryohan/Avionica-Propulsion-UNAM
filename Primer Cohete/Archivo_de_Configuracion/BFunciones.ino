//--- Wire Transmission ---
void wireTransmission(int direccion,int registro,int valor)
{
  Wire.beginTransmission(direccion);
  Wire.write(registro);              
  Wire.write(valor); 
  Wire.endTransmission();
}
