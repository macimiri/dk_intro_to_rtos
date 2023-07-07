// Maci's submission for Digikey Intro to RTOS Challenge 2
// Add another task that also blinks the LED concurrently to task 1

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // Specify the LED pin on my board
#endif

#define DELAY1 500
#define DELAY2 233

// Define tasks.
void TaskBlink1(void *pvParameters);
void TaskBlink2(void *pvParameters);

// The setup function runs once when you press reset or power on the board.
void setup() {
  // Initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  // Set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    TaskBlink1,
    "Task Blink 1", // A name just for humans
    1024,        // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    NULL, // Task parameter which can modify the task behavior. This must be passed as pointer to void.
    1,  // Priority
    NULL, // Task handle is not used here - simply pass NULL
    ARDUINO_RUNNING_CORE
    );

  xTaskCreatePinnedToCore(
    TaskBlink2,
    "Task Blink 2",
    1024,  // Stack size
    NULL,  // When no parameter is used, simply pass NULL
    1,  // Priority
    NULL, // With task handle we will be able to manipulate with this task.
    ARDUINO_RUNNING_CORE // Core on which the task will run
    );

  Serial.printf("Maci's Solution to DK FreeRTOS Challenge 2\n");
  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop(){
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskBlink1(void *pvParameters){
  /*
    Blink1
    Turns on an LED on for one second, then off for one second, repeatedly.
  */

  // initialize digital LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  for (;;){ // A Task shall never return or exit.
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay(DELAY1 / portTICK_PERIOD_MS);
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelay(DELAY1 / portTICK_PERIOD_MS);
  }
}

void TaskBlink2(void *pvParameters){  // This is a task.
  /*
    Blink2
    Turns on an LED on for a bit, then off for a bit, repeatedly.
  */

  // initialize digital LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  for (;;){ // A Task shall never return or exit.
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay(DELAY2 / portTICK_PERIOD_MS);
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelay(DELAY2 / portTICK_PERIOD_MS);
  }
}
