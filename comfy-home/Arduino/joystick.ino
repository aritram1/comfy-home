int xVal; //X values from joystick
int yVal; //Y values from joystick
int sw; //switch value from joystick

void setup() {
  Serial.begin(9600); //Starts serial at 9600 baud
  pinMode(A0, INPUT); //Sets the analog ports used to an input
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
}

void loop() {
  xVal = analogRead(A0); 
  yVal = analogRead(A1);
  sw = analogRead(A2);
   
  Serial.print(xVal);
  Serial.print(",");
  Serial.print(yVal);
  Serial.print(",");
  Serial.println(sw);
  //delay(100);
}