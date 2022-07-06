#ifdef ARDUINO_ARCH_ESP32

#define SOUND_PWM_CHANNEL   0
#define SOUND_RESOLUTION    8 //8 bit resolution
#define SOUND_ON            (1<<(SOUND_RESOLUTION-1)) //50% duty cycle
#define SOUND_OFF           0                         //0% duty cycle

void tone(int pin, int frequency, int duration)
{
  ledcSetup(SOUND_PWM_CHANNEL, frequency, SOUND_RESOLUTION);  //Set up PWM channel
  ledcAttachPin(pin, SOUND_PWM_CHANNEL);                      //Attach channel to pin
  ledcWrite(SOUND_PWM_CHANNEL, SOUND_ON);
  delay(duration);
  ledcWrite(SOUND_PWM_CHANNEL, SOUND_OFF);
}

#endif