#define aX A1
#define aY A2
#define aZ A3

double zeroValue[3] = { 0 }; //  accX, accY, accZ

void setup() 
{
  analogReference(EXTERNAL); // 3.3V
  Serial.begin(115200);
  delay(100);//wait for the sensor to get ready

  // Calibrate sensor in horizontal position
  for (uint8_t i = 0; i < 100; i++) 
  { 
    // Take the average of 100 readings
    zeroValue[0] += analogRead(aX);
    zeroValue[1] += analogRead(aY);
    zeroValue[2] += analogRead(aZ);
    delay(10);
  }
  zeroValue[0] /= 100;
  zeroValue[1] /= 100;
  zeroValue[2] /= 100;
  zeroValue[2] -= 102.3; // Z value is -1g when facing upwards - Sensitivity = 0.33/3.3*1023=102.3
}

void loop() {

  double accXval = (double)analogRead(aX) - zeroValue[0];
  double accYval = (double)analogRead(aY) - zeroValue[1];
  double accZval = (double)analogRead(aZ) - zeroValue[2];

  // Convert to 360 degrees resolution
  // atan2 outputs the value of -p to p (radians) - see http://en.wikipedia.org/wiki/Atan2
  // We are then convert it to 0 to 2p and then from radians to degrees
  double accXangle = (atan2(accXval, accZval) + PI) * RAD_TO_DEG;
  double accYangle = (atan2(accYval, accZval) + PI) * RAD_TO_DEG;

  Serial.print(accXangle); Serial.print("\t");
  Serial.print(accYangle); Serial.print("\t");
  Serial.print("\n");
  delay(10);
}