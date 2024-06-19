// Includes
#include <Wire.h>
#include <LiquidCrystal.h>

// Unalterable values
const int MAIN_WIRE_ADDRESS = 2;
const int REMOTE_WIRE_ADDRESS = 1;
const int RS = 2, EN = 3, D4 = 4, D5 = 5, D6 = 6, D7 = 7;

// Variables
bool btnState = false;
bool isNewData = false;
float receivedIn, receivedCm;
byte receivedBytes[2 * sizeof(float)];

// Pin definitions
const int BTN_PIN = 10;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

// Function to send information over I2C port
void sendInfoOverI2C(
  const int REMOTE_WIRE_ADDRESS = 1,
  bool btnState = false
) {  
  // Start the transmission
  Wire.beginTransmission(REMOTE_WIRE_ADDRESS);
  
  // Send inches value
  Wire.write(btnState);
  
  // Finish the transmission
  Wire.endTransmission();
}

// Function to receive information over IC2 port
void receiveInfoOverI2C(int howMany) {
  if (howMany == 2 * sizeof(float)) {
    for (int i = 0; i < howMany; i++) {
      receivedBytes[i] = Wire.read();
    }
    isNewData = true;
  }
}

void printStaticTextOnLcd() {
  lcd.clear(); // Clear screen
  lcd.display();
  
  lcd.setCursor(0, 0);
  lcd.print("Distance: ");
  
  lcd.setCursor(13, 1);
  lcd.print(" cm");
  
  lcd.setCursor(0, 1);
  lcd.print("Distance: ");
  
  lcd.setCursor(13, 0);
  lcd.print(" in");
}

void updateDistanceOnLcd(
  float inches,
  float centimeters
) {
  // Clear previous values
  lcd.setCursor(10, 0);
  lcd.print("   "); // Clear area for inches value and unit
  
  lcd.setCursor(10, 1);
  lcd.print("   "); // Clear area for centimeters value and unit
  
  // Update inches
  lcd.setCursor(10, 0);
  lcd.print(inches, 0);
  
  // Update centimeters
  lcd.setCursor(10, 1);
  lcd.print(centimeters, 0);
}

void printAlarmTextOnLcd(const int timeout = 1000) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm is pressed");
  
  // Wait to finish
  delay(timeout);
}

// Function to print values in serial monitor
void printSerialDebug(
  float inches = 0,
  float centimeters = 0,
  bool btnState = false
) {
  Serial.println("Distance: " + String(inches) + " in");
  Serial.println("Distance: " + String(centimeters) + " cm");
  Serial.println("Button state: " + String(btnState ? "true" : "false"));
  Serial.println();
}

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while(!Serial); // Wait until the serial is working

  // Initialize I2C bus
  Wire.begin(MAIN_WIRE_ADDRESS);
 
  // Initialize pins
  pinMode(BTN_PIN, INPUT);

  // Initialize LCD
  lcd.begin(16, 2); // Set up number of columns and rows
  
  // Initial state
  lcd.clear(); // Clear screen
  lcd.noBlink(); // Disable blink
  lcd.noCursor(); // Disable cursor
  lcd.noDisplay(); // Disable display
  lcd.setCursor(0, 0); // Set cursor position
  
  // Calls the function when it receives info
  Wire.onReceive(receiveInfoOverI2C);
  
  // Call the function to print the static text on the LCD 
  printStaticTextOnLcd();
  
  // Welcome message
  Serial.println("Welcome. System initialized.");
  Serial.println("\nPlease, type 'help' to get the full list of commands");
  Serial.println("------------------------------------------------------");
}

void loop() {
    // Save button status
    btnState = digitalRead(BTN_PIN);
    
    // Call the function to send info over I2C port
    sendInfoOverI2C(REMOTE_WIRE_ADDRESS, btnState);
  
  // Display alarm message if button is pressed
  if (btnState) {
    printAlarmTextOnLcd();
    
    printStaticTextOnLcd();
  }
  
  // Check for new data
  if (isNewData) {
    // Save the distance value in inches
    memcpy(
      &receivedIn,
      (const void*)receivedBytes,
      sizeof(receivedIn)
    );
    
    // Save the distance value in centimeters
    memcpy(
      &receivedCm,
      (const void*)(receivedBytes + sizeof(receivedIn)),
      sizeof(receivedCm)
    );
    
    // Update LCD with new distance data
    updateDistanceOnLcd(receivedIn, receivedCm);

    isNewData = false;
    
    // Call the function to print in serial monitor the info
    printSerialDebug(receivedIn, receivedCm, btnState);
  }
  
  delay(100);
}