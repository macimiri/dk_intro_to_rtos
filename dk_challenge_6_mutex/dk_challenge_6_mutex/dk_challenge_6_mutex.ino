// Maci's submission for Digikey Intro to RTOS Challenge 6 Mutex
// Hack a mutex to allow a parameter to be read before it is cleared.

// Run on single core. Taken from DK example
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
static SemaphoreHandle_t mutex;

// Define tasks
void task_blink(void *pvParameters);

void setup() {
  // Initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Maci's Solution to DK FreeRTOS Challenge 6 Mutex");

  Serial.println("Enter a delay arg (milliseconds):");
  while (Serial.available() <= 0);
  long int delay_arg = Serial.parseInt();

  // Create mutex before starting tasks
  mutex = xSemaphoreCreateMutex();

  // Set up blink task
  xTaskCreatePinnedToCore(
    task_blink,
    "task blink", // A name just for humans
    1024,  // The stack size
    (void *)&delay_arg, // Task parameters. This must be passed as pointer to void.
    1,  // Priority
    NULL, // Task handle is not used here - simply pass NULL
    app_cpu
  );

  // Mutex hack: check to see when task_inc takes mutex (and param var), then we can allow setup() to end
  while(xSemaphoreTake(mutex, 0) == pdTRUE){
    xSemaphoreGive(mutex);
  }
  
  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.

  // Delete setup and loop tasks. Passing NULL deletes calling task.
  vTaskDelete(NULL);
}

void loop(){
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void task_blink(void *pvParameters){
  /*
    task_blink (task B in DK challenge 5 docs)
    - Hack mutex to signal to calling function that parameter var has been accessed
    - 2) Blink LED
  */

  // Copy the parameter into a local var
  int local_delay_arg = *(int *)pvParameters;
  Serial.print("Received delay arg: ");
  Serial.println(local_delay_arg);

  // Signal to setup() that var has been accessed
  while(xSemaphoreTake(mutex, 0) == pdFALSE);

  pinMode(LED_BUILTIN, OUTPUT);

  for(;;){
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay(MILLIS_TO_TICKS(local_delay_arg));
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(MILLIS_TO_TICKS(local_delay_arg));
  }
}
