
int sensor_pin = A0; 
int output_value ;

void setup() {
  Serial.begin(115200);
  delay(2000);
  }

void loop() {

  output_value = analogRead(sensor_pin);
  Serial.print("Sensor em : ");
  Serial.print(output_value);
  delay(1000);
  }
