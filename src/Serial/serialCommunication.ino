// Pin definitions
const int RED_LED_PIN = 2;
const int GREEN_LED_PIN = 3;
const int DC_MOTOR_PIN = 4;

// Variables
String COMMAND;
int COMMAND_BYTE = 0;
int DC_MOTOR_ANGLE = 0;

void serialHelp() {
  Serial.println("\nThese are all the available commands:");
  Serial.println("  - HELP");
  Serial.println("  - ON");
  Serial.println("  - OFF");
  Serial.println("  - RL_ON");
  Serial.println("  - RL_OFF");
  Serial.println("  - GL_ON");
  Serial.println("  - GL_OFF");
  Serial.println("  - DCM_ON");
  Serial.println("  - DCM_OFF");
  Serial.println("  - DCM_ANGLE <angle>\n");
}

void turnON() {
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, HIGH);
  analogWrite(DC_MOTOR_PIN, 255); // Servo motor
}

void turnOFF() {
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, LOW);
  analogWrite(DC_MOTOR_PIN, LOW); // Servo motor
}

void redLedON() {
  digitalWrite(RED_LED_PIN, HIGH);
}

void redLedOFF() {
  digitalWrite(RED_LED_PIN, LOW);
}

void greenLedON() {
  digitalWrite(GREEN_LED_PIN, HIGH);
}

void greenLedOFF() {
  digitalWrite(GREEN_LED_PIN, LOW);
}

void dcMotorON() {
  analogWrite(DC_MOTOR_PIN, 255);
}

void dcMotorOFF() {
  analogWrite(DC_MOTOR_PIN, LOW);
}

void dcMotorSetAngle(int angle = 0) {
    angle = constrain(angle, 0, 180);

    int pulseWidth = map(angle, 0, 180, 1000, 2000);

    digitalWrite(DC_MOTOR_PIN, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(DC_MOTOR_PIN, LOW);
    delay(20);
}

void dcMotorGetAngle() {
  int angle = COMMAND.substring(11).toInt();
  
  dcMotorSetAngle(angle);
}

// Definition of a structure to associate commands with functions
struct CommandEntry {
    const char* command;
    void (*function)();
};

//
CommandEntry comandos[] = {
    {"HELP", serialHelp},
    {"ON", turnON},
    {"OFF", turnOFF},
    {"RL_ON", redLedON},
    {"RL_OFF", redLedOFF},
    {"GL_ON", greenLedON},
    {"GL_OFF", greenLedOFF},
    {"DCM_ON", dcMotorON},
    {"DCM_OFF", dcMotorOFF},
    //{"DCM_ANGLE", },
};

// Function to manage the commands
void handleCommand(String CMD = "") {
  // Convert string to uppercase
  CMD.toUpperCase();
  
  bool FIND_COMMAND = false;

  for (size_t i = 0; i < sizeof(comandos) / sizeof(comandos[0]); ++i) {
    if (CMD == comandos[i].command) {
      comandos[i].function();
      FIND_COMMAND = true;
      break;
     }
     else if (CMD.startsWith("DCM_ANGLE")) {
       dcMotorGetAngle();
       FIND_COMMAND = true;
     }
   }
    
   if (!FIND_COMMAND) {
     Serial.println("The '" + CMD + "' command is wrong. Try typing it again.\n");
   }
}

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize LED pins
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(DC_MOTOR_PIN, OUTPUT); // Servo motor
  
  // Initial state
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, LOW);
  analogWrite(DC_MOTOR_PIN, LOW); // Servo motor
  
  // Welcome message
  Serial.println("Welcome. System initialized.");
  Serial.println("  - To activate the circuit type: ON");
  Serial.println("  - To disable the circuit type: OFF");
  Serial.println("\nPlease, type 'help' to get the full list of commands");
  Serial.println("------------------------------------------------------");
  Serial.println("Awaiting commands...\n");
}

void loop() {
  // Check whether values have been entered for serial
  if (Serial.available() > 0) {
    COMMAND = ""; // Clean command variable
    
    // Loop to save all entered characters
    while (Serial.available()) {
      COMMAND_BYTE = Serial.read(); // Read serial values in bytes (ascii) format
      COMMAND += (char)COMMAND_BYTE; // Change the number in ascii with the corresponding symbol
    }
    
    // Print the command received by the serial
    Serial.print("I received: ");
    Serial.println(COMMAND);
    
    // Call the function to handle the command
    handleCommand(COMMAND);
  }
  
  delay(150); // Set a waiting time
}