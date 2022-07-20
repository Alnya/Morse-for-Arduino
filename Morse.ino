#include <Arduino_FreeRTOS.h>
#include <LiquidCrystal_I2C.h>


const char *morse_code[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--",
                            "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--",
                            "--..",
                            ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----.", "-----",
                            " "};

const char abc_code[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
                         'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                         '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', ' '};


// set string to send.
String string_send = "hello";

// set period of morse.
const int period = 190;


// set LED pin.
#define led 8

// secure address 0x27, create lcd of 16*2
LiquidCrystal_I2C lcd(0x27, 16, 2);


int i, j, index;

bool is_blocked = false;


void TaskMorse(void *pvParameters);

void TaskReadSerialMonitor(void *pvParameters);

String convert_upper_case(String str);

void lcd_setup_for_task_morse(int i);

void blink_ton();

void blink_tsu();

void run_ton_tsu(int j);

void search_abc_index();

void print_start_declaration();

void print_change_string_send(String string_input);


/**
 * set Serial monitor.
 * set LED pin mode.
 * set LCD monitor.
 * convert upper case.
 * create tasks.
 */
void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  string_send = convert_upper_case(string_send);
  while (!Serial) { ; //wait for Serial Monitor.
  }

  xTaskCreate(
          TaskMorse,
          "Morse",
          128,
          NULL,
          1,
          NULL
  );
  xTaskCreate(
          TaskReadSerialMonitor,
          "ReadMonitor",
          128,
          NULL,
          2,
          NULL
  );
}


void loop() { //don't use
}

/**
 * Morse
 * memory: 128
 * priority: 1
 *
 * The main function of morse communication.
 * While morse light communication,
 * write serial monitor and lcd monitor.
 * If new letter accepted from serial monitor,
 * this function would be interrupted and initialized.
 */
void TaskMorse(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    print_start_declaration();
    delay(300);

    for (i = 0; i < string_send.length(); i++) {
      Serial.println(string_send[i]);
      lcd_setup_for_task_morse(i);

      search_abc_index();

      for (j = 0; j < strlen(morse_code[index]); j++) {
        Serial.print(morse_code[index][j]);
        lcd.print(morse_code[index][j]);

        run_ton_tsu(j);
      }

      Serial.println();
      delay(period * 2);
    }
    Serial.println("=====================");
    delay(period * 5);
  }
}

/**
 * ReadMonitor
 * memory: 128
 * priority: 2
 *
 * If new letter accepted from serial monitor,
 * this function would rewrite text for send.
 * And then, write serial monitor and lcd monitor.
 */
void TaskReadSerialMonitor(void *pvParameters) {
  (void) pvParameters;
  int tmp;
  String string_input;
  for (;;) {
    if (Serial.available() > 1) {
      is_blocked = true;
      tmp = Serial.read();
      if (tmp == '\n')continue;
      string_input += char(tmp);
      delay(50);
    } else {
      if (is_blocked) {
        tmp = Serial.read();
        string_input = convert_upper_case(string_input);
        print_change_string_send(string_input);
        string_send = string_input;
      }
      is_blocked = false;
      string_input = "";
      vTaskDelay(1);
    }
  }
}

String convert_upper_case(String str) {
  String tmp = "";
  for (i = 0; i < str.length(); i++) {
    if (str[i] >= 97 && str[i] <= 122) {
      tmp += char(str[i] - 32);
    } else {
      tmp += char(str[i]);
    }
  }
  return String(tmp);
}

void lcd_setup_for_task_morse(int i) {
  lcd.clear();
  lcd.print(string_send);
  lcd.setCursor(0, 1);
  lcd.print(string_send[i]);
  lcd.print(":");
}

void blink_ton() {
  digitalWrite(led, HIGH);
  delay(period);
  digitalWrite(led, LOW);
}

void blink_tsu() {
  digitalWrite(led, HIGH);
  delay(period * 3);
  digitalWrite(led, LOW);
}

void run_ton_tsu(int j) {
  if (morse_code[index][j] == '.') {
    blink_ton();
  } else if (morse_code[index][j] == '-') {
    blink_tsu();
  } else {
    delay(period);
  }
  delay(period);
}

void search_abc_index() {
  for (j = 0; j < 37; j++) {
    if (abc_code[j] == string_send[i]) {
      index = j;
      break;
    }
  }
}

void print_start_declaration() {
  Serial.print("string to send:");
  Serial.println(string_send);
  Serial.print("period of morse:");
  Serial.println(period);
  Serial.println("Start of Morse code communication.\n");
}


void print_change_string_send(String string_input) {
  Serial.println("\n=====================");
  Serial.println("送信文字列が変更されました");
  Serial.println(string_send + " -> " + string_input);
  Serial.println("=====================\n");
  lcd.clear();
  lcd.print("---Change---");
  lcd.setCursor(0, 1);
  lcd.print(string_send + "->" + string_input);
}