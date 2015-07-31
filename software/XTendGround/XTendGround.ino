

void setup(){
  Serial.begin(9600);
 
}

void loop(){
  char thing;  
  while(Serial.available()){
    thing = (char) Serial.read();
    
    Serial.print(thing);
  }
  
}
