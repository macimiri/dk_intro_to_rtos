//Maci's submission for Digikey Intro to RTOS Challenge 8 Software Timers

/*
 * Challenge: Your challenge is to create an auto-dim feature using the 
 * onboard LED. We’ll pretend that the onboard LED (LED_BUILTIN) is the 
 * backlight to an LCD. 
 * 
 * Create a task that echoes characters back to the serial terminal (as we’ve 
 * done in previous challenges). When the first character is entered, the 
 * onboard LED should turn on. It should stay on so long as characters are 
 * being entered.
 * 
 * Use a timer to determine how long it’s been since the last character was 
 * entered (hint: you can use xTimerStart() to restart a timer’s count, even 
 * if it’s already running). When there has been 5 seconds of inactivity, 
 * your timer’s callback function should turn off the LED.
 */

//Run on single core. Taken from DK example
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Macros
#define MILLIS_TO_TICKS(milliseconds) ((milliseconds) / portTICK_PERIOD_MS)

// Settings
#define LED_BUILTIN 2  // Specify the LED pin on my board

// Globals
static TimerHandle_t led_timer = NULL;

// Define tasks
void task_serial(void *pvParameters);

// Define callbacks
void led_timer_callback(TimerHandle_t xTimer);


void setup() {
  // Configure Serial
  Serial.begin(115200);

  // Wait a moment to start (so we don't miss Serial output)
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println("Start Challenge 8: Software Timers");

  pinMode(LED_BUILTIN, OUTPUT);

  led_timer = xTimerCreate(
    "led timer",
    MILLIS_TO_TICKS(5000),
    pdFALSE,
    (void *)0,
    led_timer_callback
  );
  if (led_timer == NULL){
    Serial.println("ERROR: Timer creation failed.");
  }

  // Start producer tasks (wait for each to read argument)
  xTaskCreatePinnedToCore(task_serial,
                          "task serial",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);

  vTaskDelete(NULL);
}

void loop(){
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void task_serial(void *pvParameters) {
  /*
    task_serial
    Reads bytes from Serial and echoes them back to Serial.
    Will refresh led dim callback on keypress.
  */

  static char in_char;

  for(;;){
    if(Serial.available() > 0){
      in_char = Serial.read();
      Serial.print(in_char);

      // turn on LED, start/refresh 5 sec timer to turn off.
      digitalWrite(LED_BUILTIN, HIGH);
      xTimerStart(led_timer, portMAX_DELAY);
    }
  }
}

/*--------------------------------------------------*/
/*-------------------- Callbacks -------------------*/
/*--------------------------------------------------*/

// When led_timer_callback is called, turn off LED since time has elapsed.
void led_timer_callback(TimerHandle_t xTimer){
  digitalWrite(LED_BUILTIN, LOW);
}