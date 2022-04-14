#define pinRPM 9
#define RPM_SAMPLES 16 //Must be a power of 2 and smaller than 255
#define rpm_multiplier 20000000 //20000000 for 6 cylinder 4 stroke, //30000000 for 4 cylinder 4 stroke???
#define ADC_FILTER(input, alpha, prior) (((long)input * (256 - alpha) + ((long)prior * alpha))) >> 8

int rpms[] = {4400, 4700, 5000, 5300, 5600, 5900, 6200};
//int rpms[] = {500, 800, 1100, 1400, 1700, 2000, 2300};
int rpm_pins_led[] = {0, 1, 4, 5, 6, 7, 8};
bool led_status[] = {false, false, false, false, false, false, false};

int period_flash = 100;
unsigned long time_now = 0;
unsigned long time_now_flash = 0;

//rpm
uint16_t rpm = 0;
uint16_t prev_rpm = 0;
volatile unsigned long rpmLastPulseTime; /**< The time of the last VSS pulse of the VSS */
volatile unsigned long rpmLastMinusOnePulseTime; /**< The time of the last VSS_NUM_SAMPLES pulses of the VSS are stored in this array */
volatile unsigned long rpmTotalTime = 0; /**< Cumulative count of the last VSS_SAMPLES number of pulses */
volatile byte rpmCount = 0;
int RPM = LOW;


void setup() {
  //Serial.begin(9600);
  //set pins
  for(int i = 0; i < (sizeof(rpms) / sizeof(rpms[0])); i++){
    pinMode(rpm_pins_led[i], OUTPUT); 
  }
  attachInterrupt(digitalPinToInterrupt(pinRPM), rpmPulse, RISING); // interupt for vss from car   
  //startup sequence
  startup();
  //
  rpmCount = 0;
  rpmTotalTime = 0;
}

void loop() {
  //Serial.println(rpm);
  //rpm
  rpm = getSpeed();         // read rpm
  led();
  if(millis() >= time_now_flash + period_flash){
    time_now_flash = millis();
    if(rpm > rpms[6]){
      flash();
    }
  }  
}


//////////////////////////////////////////////////////////////////////////////////////////

void startup(){
  rpm = rpms[0] + 1;
  led();
  delay(120);
  rpm = rpms[1] + 1;
  led();
  delay(120);
  rpm = rpms[2] + 1;
  led();
  delay(120);
  rpm = rpms[3] + 1;
  led();
  delay(120);
  rpm = rpms[4] + 1;
  led();
  delay(120);
  rpm = rpms[5] + 1;
  led();
  delay(120);
  rpm = rpms[6] + 1;
  led();
  delay(120);
  rpm = rpms[5] + 1;
  led();
  delay(120);
  rpm = rpms[4] + 1;
  led();
  delay(120);
  rpm = rpms[3] + 1;
  led();
  delay(120);
  rpm = rpms[2] + 1;
  led();
  delay(120);
  rpm = rpms[1] + 1;
  led();
  delay(120);
  rpm = rpms[0] + 1;
  led();
  delay(120);
  rpm = 0;
}

void led(){
  for(int i = 0; i < (sizeof(rpms) / sizeof(rpms[0])); i++){
    if (rpm > rpms[i] and led_status[i] == false){
        digitalWrite(rpm_pins_led[i], HIGH);
        led_status[i] = true;
    }else if (rpm < rpms[i] and led_status[i] == true){
        digitalWrite(rpm_pins_led[i], LOW);
        led_status[i] = false;
    } 
  }
}

uint16_t getSpeed()
{
  uint16_t tempSpeed = 0;
  uint32_t pulseTime = 0;

  //Need to temp store the pulse time variables to prevent them changing during an interrupt
  noInterrupts();
  uint32_t temp_rpmLastPulseTime = rpmLastPulseTime;
  uint32_t temp_rpmLastMinusOnePulseTime  = rpmLastMinusOnePulseTime;
  interrupts();


    if( rpmCount == RPM_SAMPLES ) //We only change the reading if we've reached the required number of samples
    {
      if(temp_rpmLastPulseTime < temp_rpmLastMinusOnePulseTime) { tempSpeed = rpm; } //Check for overflow of micros()
      else
      {
        pulseTime = rpmTotalTime / RPM_SAMPLES;
        tempSpeed = rpm_multiplier / pulseTime ; //Convert the pulse gap into rpm  //20000000 for 6 cylinder 4 stroke, 
        tempSpeed = ADC_FILTER(tempSpeed, 32, rpm); //Apply speed smoothing factor
        if(tempSpeed > 20000) { tempSpeed = rpm; } //Safety check. This usually occurs when there is a hardware issue
      }

      rpmCount = 0;
      rpmTotalTime = 0;
    }
    else
    {
      //Either not enough samples taken yet or speed has dropped to 0
      if ( (micros() - temp_rpmLastPulseTime) > 2000000 ) { tempSpeed = 0; } // Check that the car hasn't come to a stop (2s timeout)
      else { tempSpeed = rpm; } 
    }

  return tempSpeed;
}


void flash(){
  for(int i = 0; i < (sizeof(rpms) / sizeof(rpms[0])); i++){
    digitalWrite(rpm_pins_led[i], !digitalRead(rpm_pins_led[i]));
  }
}

////interupts 


void rpmPulse()
{
  //TODO: Add basic filtering here
  rpmLastMinusOnePulseTime = rpmLastPulseTime;
  rpmLastPulseTime = micros();

  if(rpmCount < RPM_SAMPLES)
  {
    rpmTotalTime += (rpmLastPulseTime - rpmLastMinusOnePulseTime);
    rpmCount++;
  } 
}
