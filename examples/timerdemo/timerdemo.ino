// This is a 'low level' demo showing how to use timer 2 (the only available timer really)
// to blink an LED. This code turned into the matrix handler, so you cant use it and the 
// Adafruit_Microbit library at the same time but maybe its useful!

const int COL1 = 3;     // Column #1 control
const int LED = 26;     // 'row 1' led


void start_timer(void)
{    
  NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;              // Set the timer in Counter Mode
  NRF_TIMER2->TASKS_CLEAR = 1;                           // clear the task first to be usable for later
  NRF_TIMER2->PRESCALER   = 8;  
  NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_16Bit;     //Set counter to 16 bit resolution
  NRF_TIMER2->CC[0] = 32000;                               //Set value for TIMER2 compare register 0
  NRF_TIMER2->CC[1] = 5;                                   //Set value for TIMER2 compare register 1
    
  // Enable interrupt on Timer 2, both for CC[0] and CC[1] compare match events
  NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos) | (TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos);
  NVIC_EnableIRQ(TIMER2_IRQn);
    
  NRF_TIMER2->TASKS_START = 1;               // Start TIMER2
}

    
/** TIMTER2 peripheral interrupt handler. This interrupt handler is called whenever there it a TIMER2 interrupt
 * Don't mess with this line. really.
 */
 extern "C"  { void TIMER2_IRQHandler(void) { timer_pal();  } }

void timer_pal(void) {
  if ((NRF_TIMER2->EVENTS_COMPARE[0] != 0) && ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE0_Msk) != 0))
  {
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;         //Clear compare register 0 event 
    digitalWrite(LED, HIGH);
  }
  
  if ((NRF_TIMER2->EVENTS_COMPARE[1] != 0) && ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE1_Msk) != 0))
  {
    NRF_TIMER2->EVENTS_COMPARE[1] = 0;         //Clear compare register 1 event
    digitalWrite(LED, LOW);
  }
}



void setup() {  
  Serial.begin(9600);
  Serial.println("microbit is ready!");

  // because the LEDs are multiplexed, we must ground the opposite side of the LED
  pinMode(COL1, OUTPUT);
  digitalWrite(COL1, LOW); 
   
  pinMode(LED, OUTPUT);  

  start_timer();
}

void loop(){
  Serial.println("blink!");
  delay(500);
}
