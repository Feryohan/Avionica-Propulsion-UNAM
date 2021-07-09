void setup() {
  Serial.begin(9600);
  Wire.begin();

  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(0x6B); //PWR_MGMT_1
  Wire.write(0); //Despierta al MPU6050
  Wire.endTransmission();

/*  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(0x6B); //PWR_MGMT_1
  Wire.write(0x80);
  Wire.endTransmission();
  */
  
  wireRead(MPU6050_ADDRESS,0x23);

  wireConfiguration(MPU6050_ADDRESS,CONFIG,DLPF_CFG);
  wireRead(MPU6050_ADDRESS,CONFIG);
  wireConfiguration(MPU6050_ADDRESS,GYRO_CONFIG,FS_SEL);
  wireConfiguration(MPU6050_ADDRESS,ACCEL_CONFIG,AFS_SEL);
  wireRead(MPU6050_ADDRESS,GYRO_CONFIG);
  wireRead(MPU6050_ADDRESS,ACCEL_CONFIG);
  Serial.println("AcelX,AcelY,AcelZ,GyroX,GyroY,GyroZ");
}
