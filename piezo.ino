void setup()
	{
	Serial.begin(38400);
	}
uint8_t	pin = 0;
int	val = 0;
void loop()
	{
 	val = analogRead(pin);
	Serial.print(xofval);
	Serial.print("\n");
	delay(2);
	}
