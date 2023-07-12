// Maci's submission for Digikey Intro to RTOS Challenge 4
// Create one task to read serial and send to another task, which prints it

// Taken from arduino example, but doesn't work! Use DK example just below
// #if CONFIG_FREERTOS_UNICORE
// #define ARDUINO_RUNNING_CORE 0
// #else
// #define ARDUINO_RUNNING_CORE 1
// #endif

// Run on single core. Taken from DK example
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Settings
#define BUF_LEN 50
#define LED_BUILTIN 2  // Specify the LED pin on my board
#define DEBUG 0

// Globals
static char *str = NULL;
static volatile int msg_ready_flag = 0;

// Define tasks
void task_read_serial(void *pvParameters);
void task_print_serial(void *pvParameters);

// The setup function runs once when you press reset or power on the board.
void setup() {
  // Initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  while(!Serial);

  // Set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    task_read_serial,
    "task_read_serial", // A name just for humans
    1024,        // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    NULL, // Task parameter which can modify the task behavior. This must be passed as pointer to void.
    1,  // Priority
    NULL, // Task handle is not used here - simply pass NULL
    app_cpu
  );

  xTaskCreatePinnedToCore(
    task_print_serial,
    "task print serial",
    1024,  // Stack size
    NULL,  // When no parameter is used, simply pass NULL
    1,  // Priority
    NULL, // With task handle we will be able to manipulate with this task.
    app_cpu
  );

  Serial.println("Maci's Solution to DK FreeRTOS Challenge 4");
  Serial.println("Enter a message:");
  
  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.

  // Delete setup and loop tasks. Passing NULL deletes calling task.
  vTaskDelete(NULL);
}

void loop(){
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void task_print_serial(void *pvParameters){
  /*
    task_print_serial
    Reads message from task_read_serial and echo back to Serial connection
  */

  for (;;){
    if (msg_ready_flag){
      // if msg ready, print to serial
      Serial.println(str);

      // cleanup to prepare for next message
      vPortFree(str);  // free ptr heap memory
      str = NULL;
      msg_ready_flag = 0;  // clear msg flag
    }
  }
}

void task_read_serial(void *pvParameters) {
  /*
    task_read_serial
    Reads message from Serial and creates msg for task_print_serial
  */

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
        // create heap mem to transfer inputted data buf to global var str
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
