// Maci's submission for Digikey Intro to RTOS Challenge 5
// Establish two queues to communicate between two tasks: an echo thread and a blink thread.

// Run on single core. Taken from DK example
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Macros
#define MILLIS_TO_TICKS(milliseconds) ((milliseconds) / portTICK_PERIOD_MS)

// Settings
#define BUF_LEN 100
#define LED_BUILTIN 2  // Specify the LED pin on my board
#define DEFAULT_BLINK_RATE 500

// Globals
static QueueHandle_t h_queue_1;
static QueueHandle_t h_queue_2;

// Define tasks
void task_echo(void *pvParameters);
void task_blink(void *pvParameters);

void setup() {
  // Initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  while(!Serial);

  // Create queues
  h_queue_1 = xQueueCreate(5, sizeof(int));
  h_queue_2 = xQueueCreate(5, BUF_LEN * sizeof(char));

  // Set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    task_echo,
    "task echo", // A name just for humans
    1024,  // The stack size
    NULL, // Task parameter which can modify the task behavior. This must be passed as pointer to void.
    1,  // Priority
    NULL, // Task handle is not used here - simply pass NULL
    app_cpu
  );

  xTaskCreatePinnedToCore(
    task_blink,
    "task blink",
    1024,  // Stack size
    NULL,  // When no parameter is used, simply pass NULL
    1,  // Priority
    NULL, // With task handle we will be able to manipulate with this task.
    app_cpu
  );

  Serial.println("Maci's Solution to DK FreeRTOS Challenge 5");
  Serial.println("Enter a message (delay XX to command LED):");
  
  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.

  // Delete setup and loop tasks. Passing NULL deletes calling task.
  vTaskDelete(NULL);
}

void loop(){
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void task_echo(void *pvParameters){
  /*
    task_echo (task A in DK challenge 5 docs)
    - 1) Echoes messages from Queue 2 to Serial.
    - 2) Reads message from Serial, echoes back to Serial.
    - 3) Also transmits delay to Queue 1 if Serial msg is "delay XX"
  */

  // Used for job 1
  static char msg_from_q2[BUF_LEN];
  memset(msg_from_q2, 0, BUF_LEN);

  // Used for job 2
  static char buf[BUF_LEN];
  memset(buf, BUF_LEN, sizeof(char));
  char in_char;
  int i;

  // Used for job 3
  const char delay_msg[] = "delay ";

  for(;;){
    // Job 1. Print any new messages from q2
    while (xQueueReceive(h_queue_2, msg_from_q2, 0) == pdPASS){
      Serial.println(msg_from_q2);
      memset(msg_from_q2, 0, BUF_LEN);
    }

    // Job 2. Read serial input, echo back to serial
    // Job 3. If input is "delay X", send num X to q1
    while(Serial.available() > 0){
      in_char = Serial.read();

      if(in_char == '\n'){
        // entered command, do things
        Serial.println(buf);  // this is job 2

        // This is job 3
        // see if the entered string is a delay command. ("delay " 6 digits, 7 bytes)
        if (!strncmp("delay ", buf, 6)){
          int num_to_send = atoi(buf + 6);
          xQueueSend(h_queue_1, &num_to_send, 0);
        }

        // clean up, prepare for next message
        memset(buf, 0, BUF_LEN);
        i = 0;
      }
      else {
        // add char to buf
        buf[i++] = in_char;
      }
    }
  }
}

void task_blink(void *pvParameters){
  /*
    task_blink (task B in DK challenge 5 docs)
    - 1) Blinks LED according to messages from Queue 1
    - 2) Every 100 blinks, send "blinked" to Queue 2
  */

  pinMode(LED_BUILTIN, OUTPUT);
  static int blink_rate = DEFAULT_BLINK_RATE;
  int blink_count = 0;

  for(;;){
    // 1)
    xQueueReceive(h_queue_1, &blink_rate, 0);

    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay(MILLIS_TO_TICKS(blink_rate));
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(MILLIS_TO_TICKS(blink_rate));
    blink_count++;

    // 2)
    if (blink_count == 100){
      xQueueSend(h_queue_2, "blinked 100", 0);
      blink_count = 0;
    }
  }
}
