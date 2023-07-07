// Maci's submission for Digikey Intro to RTOS Challenge 3
// Create one task to read serial and one task to update LED blink rate

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

// Settings
#define BUF_LEN 20
#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // Specify the LED pin on my board
#endif
#define DEFAULT_BLINK_RATE_MILLIS 500

// Globals
static char *str = NULL;
static volatile int msg_ready_flag = 0;

// Define tasks.
void task_read_serial(void *pvParameters);
void task_blink_led(void *pvParameters);

// The setup function runs once when you press reset or power on the board.
void setup() {
  // Initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  // Set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    task_read_serial,
    "task_read_serial", // A name just for humans
    1024,        // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    NULL, // Task parameter which can modify the task behavior. This must be passed as pointer to void.
    1,  // Priority
    NULL, // Task handle is not used here - simply pass NULL
    ARDUINO_RUNNING_CORE
    );

  xTaskCreatePinnedToCore(
    task_blink_led,
    "task blink led",
    1024,  // Stack size
    NULL,  // When no parameter is used, simply pass NULL
    2,  // Priority
    NULL, // With task handle we will be able to manipulate with this task.
    ARDUINO_RUNNING_CORE // Core on which the task will run
    );

  Serial.printf("Maci's Solution to DK FreeRTOS Challenge 3\n");
  Serial.printf("Enter a value for the LED blink rate");
  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop(){
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void task_blink_led(void *pvParameters){
  /*
    task_blink_led
    Reads message from serial task and then blinks LED according to transmitted delay rate.
  */

  // initialize digital LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  volatile int blink_rate = DEFAULT_BLINK_RATE_MILLIS;

  for (;;){ // A Task shall never return or exit.
    if (msg_ready_flag){
      // if msg ready, update blink rate
      blink_rate = atoi(str);

      // cleanup to prepare for next message
      vPortFree(str);
      msg_ready_flag = 0;
    }
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay(blink_rate / portTICK_PERIOD_MS);
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelay(blink_rate / portTICK_PERIOD_MS);
  }
}

void task_read_serial(void *pvParameters) {
  static char buf[BUF_LEN];
  char in_char;
  int i;

  memset(buf, 0, BUF_LEN);

  for(;;){
    if(Serial.available() > 0){
      in_char = Serial.read();

      if(in_char == '\n'){
        // entered command, update things
        str = (char *)pvPortMalloc(strlen(buf) + 1);
        // create heap mem to transfer inputted str to global var
        strcpy(str, buf);

        // reset buffer to ready for next transmission
        memset(buf, 0, BUF_LEN);
        i = 0;
        msg_ready_flag = 1;
      }
      else if (msg_ready_flag == 0){
        // add char to buf
        buf[i++] = in_char;
      }
    }
  }
}
