// Project sponsor: jeffhanko@gmail.com
// Email: sales@cedarlakeinstruments.com
// Creator: Cedar Lake Instruments LLC
// Date: April 2021
//
// Description:
// Generate multiple PWM values on Arduino
//
// Arduino pins (Nano or Uno: Mega328)
// 9 – PWM output value
// 3 – Switch input #1 (connect switch between input and ground)
// 4 – Switch input #2 (connect switch between input and ground)
// 5 – Switch input #3 (connect switch between input and ground)
//

const uint16_t divider = 256;
uint16_t PWM_TOP = 0;


// Switch input definitions
#define PWM_SET1_PIN 3
#define PWM_SET2_PIN 4
#define PWM_SET3_PIN 5

//  ******************************** USER ADJUSTMENTS *******
// Switch 1 PWM setting
const int PWM_VALUE1 = 70;

// Switch 2 PWM setting
const int PWM_VALUE2 = 50;

// Switch 3 PWM setting
const int PWM_VALUE3 = 20;

// Default PWM setting (no switches active)
const int PWM_DEFAULT = 100;

//************************************************************

void setup() 
{
    pinMode(PWM_SET1_PIN, INPUT_PULLUP);
    pinMode(PWM_SET2_PIN, INPUT_PULLUP);
    pinMode(PWM_SET3_PIN, INPUT_PULLUP);
    Serial.begin(115200);
    InitializePwm();
    Serial.println("Ready");
}

void loop() 
{
    static byte lastState = 0xFF;
    
    // Look for change in switch inputs      
    int a = digitalRead(PWM_SET3_PIN);
    int b = digitalRead(PWM_SET2_PIN);
    int c = digitalRead(PWM_SET1_PIN);
    
    byte current = (4 * a) | (2 * b) | c;
    if (current != lastState)
    {
        Serial.print(a);Serial.print(b);Serial.println(c);
        if ((current & 4) == 0)
        {
            SetDutyCycle(PWM_VALUE3);
        }
        else if ((current & 2) == 0)
        {
            SetDutyCycle(PWM_VALUE2);
        }
        else if ((current & 1) == 0)
        {
            SetDutyCycle(PWM_VALUE1);
        }        
        else
        {
            SetDutyCycle(PWM_DEFAULT);
        }
    }
    lastState = current;
}


void SetDutyCycle(uint8_t percentage)
{
  percentage =  (percentage > 100 ? 100 : (percentage < 0 ? 0 : percentage));
  uint16_t OCR = (uint16_t)(((uint32_t)percentage * (uint32_t)PWM_TOP)/100) ;    // Set pwm percent of pwm period

  OCR1AH = OCR >> 8;
  OCR1AL = OCR & 0xFF;
  Serial.print("Set duty cycle to ");Serial.println(percentage);
}

void SetFrequency(uint8_t frequency)
{
    PWM_TOP = F_CPU/(divider*frequency) - 1;
    ICR1H = PWM_TOP >> 8;
    ICR1L = PWM_TOP & 0xFF;
    Serial.print("Set frequency to ");Serial.println(frequency);
}

void InitializePwm()
{
    // Set pin 9 as output for PWM OC
    DDRB |= (1 << PINB1);
    // Mode: Fast PWM top=ICR1
    // OC1A output: Non-Inverted PWM
    // OC1B output: Non-Inverted PWM
    // Input Capture on Falling Edge
    // Timer Period: 10 ms
    TCCR1A=(1<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (0<<COM1B0) | (1<<WGM11) | (0<<WGM10);
    TCCR1B=(0<<ICNC1) | (0<<ICES1) | (1<<WGM13) | (1<<WGM12) | (1<<CS12) | (0<<CS11) | (0<<CS10);
    TCNT1H=0x00;
    TCNT1L=0x00;

    SetFrequency(100);
    // 100% PWM at start
    SetDutyCycle(PWM_DEFAULT);
}
 
