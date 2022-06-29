const char*morse_code[] = {".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--",
                  "-.","---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--..",
                  ".----","..---","...--","....-",".....","-....","--...","---..","----.","-----"," "};

const char abc_code[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M',
                  'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
                  '1','2','3','4','5','6','7','8','9','0',' '};


// write string to send.
char string_send[20] = "aBcz 019";

// set period of morse.
const int period = 500;


// set LED pin.
#define led 8


int i, j, index;

/**
 * set Serial monitor.
 * set LED pin mode.
 * convert upper case.
 */
void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);  
  for(i = 0; i < strlen(string_send); i++){
    if(string_send[i] >= 97 && string_send[i] <= 122){
      string_send[i] = string_send[i]-32;
    }
  }
}


void loop() {
  Serial.print("string to send:");
  Serial.println(string_send);
  Serial.print("period of morse:");
  Serial.println(period);
  Serial.println("Start of Morse code communication.");
  delay(period);
  
  for(i = 0; i < strlen(string_send); i++){
    Serial.println(string_send[i]);
    for(j = 0; j < 37; j++){
      if(abc_code[j] == string_send[i]){
        index = j;
        break;
      }
    }
    for(j = 0; j < strlen(morse_code[index]); j++){
      Serial.print(morse_code[index][j]);
      if(morse_code[index][j] == '.'){
        digitalWrite(led, HIGH);
        delay(period);
        digitalWrite(led, LOW);
      } else if(morse_code[index][j] == '-'){
        digitalWrite(led, HIGH);
        delay(period * 3);
        digitalWrite(led, LOW);
      } else {
        delay(period);
      }
      delay(period);
    }
    Serial.println();
    delay(period * 2);
  }
  Serial.println("=====================");
  delay(period * 10);
}
