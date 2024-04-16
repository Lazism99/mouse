//Libraries
#include "Mouse.h" //mouse library 
#include "LIS3DHTR.h" //built-in accelerometer library 
#include "TFT_eSPI.h" //TFT LCD library 
#include "Keyboard.h"
//Initializations
LIS3DHTR <TwoWire> lis; //initialize accelerometer 
TFT_eSPI tft; //initialize TFT LCD 
TFT_eSprite spr = TFT_eSprite(&tft); //initialize buffer

// int range = 25; //output range of X or Y movement (affects movement speed)       

int responseDelay = 2; //response delay of the mouse (in ms)

//sensitive
#define LOW_SENSITIVITY 25
#define MEDIUM_SENSITIVITY 50
#define HIGH_SENSITIVITY 100

int sensitivity = MEDIUM_SENSITIVITY; // Initial sensitivity level

void adjustSensitivity() {
  // Cycle through sensitivity levels
  if (sensitivity == LOW_SENSITIVITY) {
    sensitivity = MEDIUM_SENSITIVITY;
  } else if (sensitivity == MEDIUM_SENSITIVITY) {
    sensitivity = HIGH_SENSITIVITY;
  } else if (sensitivity == HIGH_SENSITIVITY) {
    sensitivity = LOW_SENSITIVITY;
  }
}
//doubleclick
#define DEBOUNCE_DELAY 50 // 디바운싱을 위한 시간(ms)

bool aButtonPressed = false; // A 버튼이 눌렸는지 여부
unsigned long lastAButtonClickTime = 0; // 마지막 A 버튼 클릭 시간

void setup() {
  //configure button and switch pins as inputs 
  pinMode(WIO_5S_UP, INPUT);
  pinMode(WIO_5S_DOWN, INPUT);
  pinMode(WIO_5S_LEFT, INPUT);
  pinMode(WIO_5S_RIGHT,INPUT);
  pinMode(WIO_5S_PRESS,INPUT);
  pinMode(WIO_KEY_C, INPUT);
  pinMode(WIO_KEY_B, INPUT);
  pinMode(WIO_KEY_A,INPUT);
  
  Mouse.begin(); //start mouse control
  lis.begin(Wire1); //start accelerometer
  
  //Configure accelerometer 
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ); //data output rate (1Hz up to 5kHz)
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G); //Scale range set to 2g (2,4,8,16g)

  tft.begin(); //start TFT LCD
  tft.setRotation(3); //set TFT LCD rotation 
  
}

void loop() {

  //Drawing UI on TFT LCD 
  spr.createSprite(TFT_HEIGHT,TFT_WIDTH); //create buffer
  spr.fillSprite(TFT_WHITE); //set background color 

  //Draw header title
  spr.fillRect(0,0,320,50,TFT_BLUE); //draw rectangle with fill color 
  spr.setTextSize(4); //set text size
  spr.setTextColor(TFT_WHITE); //set text color 
  spr.drawString("AIR MOUSE",50,10); //draw a text string 

  //Draw for scroll
  spr.setTextSize(3);
  spr.setTextColor(TFT_BLACK);
  spr.drawString("SCROLL",110,130);
  spr.drawTriangle(160,70,120,120,200,120,TFT_BLUE); //draw triangle with border
  spr.drawTriangle(160,210,120,160,200,160,TFT_BLUE);
  

  //draw for left and right mouse buttons
  spr.drawRect(0,50,100,190,TFT_DARKGREEN); //draw rectangle with border 
  spr.drawString("LEFT",15,130);
  spr.drawRect(220,50,100,190,TFT_DARKGREEN);
  spr.drawString("RIGHT",228,130); 
  
  spr.pushSprite(0,0); //push to LCD
  
  float x_values = lis.getAccelerationX(); //assign variable to store accleration in x-axis 
  float y_values = lis.getAccelerationY(); //assign variable to store accleration in y-axis 
  
  // calculate the movement distance based on the button and switch states
  // int  xDistance = (-y_values) * range;
  // int  yDistance = (x_values)  * range;
// Adjust mouse movement based on sensitivity
  int xDistance = (-y_values) * sensitivity;
  int yDistance = (x_values) * sensitivity;

//mouse move 
  if ((xDistance != 0) || (yDistance != 0)) {   // if X or Y is non-zero, then move the mouse pointer
    Mouse.move(xDistance, yDistance, 0); //move the mouse pointer in x and y
  }
  
//mouse button left click 
  if (digitalRead(WIO_KEY_C) == LOW) { //detect left mouse button press
    if (!Mouse.isPressed(MOUSE_LEFT)) { //check whether left mouse button is pressed
      Mouse.click(MOUSE_LEFT); //click left mouse button 
    }

      spr.createSprite(TFT_HEIGHT,TFT_WIDTH); //create buffer
      spr.setTextSize(3);
      spr.setTextColor(TFT_WHITE);
      spr.fillRect(0,50,100,190,TFT_DARKGREEN);
      spr.drawString("LEFT",15,130);
      spr.pushSprite(0,0);
      spr.deleteSprite(); //clear buffer
      delay(100);
      
  }
  else {
    // if the left mouse button is pressed, release it
    if (Mouse.isPressed(MOUSE_LEFT)) {
      Mouse.release(MOUSE_LEFT);
    }
  }

  
//mouse button right click 
  if (digitalRead(WIO_KEY_B) == LOW) { //detect right mouse button press
    if (!Mouse.isPressed(MOUSE_RIGHT)) { //check whether right mouse button is pressed
      Mouse.click(MOUSE_RIGHT); //click right mouse button 
    }
      spr.createSprite(TFT_HEIGHT,TFT_WIDTH); //create buffer
      spr.setTextSize(3);
      spr.setTextColor(TFT_WHITE);
      spr.fillRect(220,50,100,190,TFT_DARKGREEN);
      spr.drawString("RIGHT",228,130);
      spr.pushSprite(0,0);
      spr.deleteSprite(); //clear buffer
      delay(100);
  }

  else {
    // if the mouse is pressed, release it:
    if (Mouse.isPressed(MOUSE_RIGHT)) {
      Mouse.release(MOUSE_RIGHT);
    }
  }

//mouse scroll up
  if ((digitalRead(WIO_5S_UP) == LOW)) { //detect 5-way switch press and up switch 
    Mouse.move(0, 0, 1); //scroll up
    spr.createSprite(TFT_HEIGHT,TFT_WIDTH); //create buffer
    spr.fillTriangle(160,70,120,120,200,120,TFT_BLUE);
    spr.pushSprite(0,0);
    spr.deleteSprite(); //clear buffer
    delay(50);
  }

//mouse scroll down
  if ((digitalRead(WIO_5S_DOWN) == LOW)) { //detect 5-way switch press and down switch
    Mouse.move(0, 0, -1); //scroll down
    spr.createSprite(TFT_HEIGHT,TFT_WIDTH); //create buffer
    spr.fillTriangle(160,210,120,160,200,160,TFT_BLUE);
    spr.pushSprite(0,0);
    spr.deleteSprite(); //clear buffer
    delay(50);
  }

  if (digitalRead(WIO_5S_LEFT) == LOW) { // Detect left button press
    // Perform "Back" action here
    // For example:
    Keyboard.press(KEY_LEFT_ALT); // Simulate pressing Ctrl key
    Keyboard.press(KEY_LEFT_ARROW); // Simulate pressing left arrow key
    delay(50); // Delay for stability
    Keyboard.releaseAll(); // Release all keys
    delay(100); // Delay to avoid repeated actions
  }

  // Check for analog stick right button press for "Forward" action
  if (digitalRead(WIO_5S_RIGHT) == LOW) { // Detect right button press
    // Perform "Forward" action here
    // For example:
    Keyboard.press(KEY_LEFT_ALT); // Simulate pressing Ctrl key
    Keyboard.press(KEY_RIGHT_ARROW); // Simulate pressing right arrow key
    delay(50); // Delay for stability
    Keyboard.releaseAll(); // Release all keys
    delay(100); // Delay to avoid repeated actions
  }

  // Check if the center button of the analog stick is pressed
  if (digitalRead(WIO_5S_PRESS) == LOW) {
    // Adjust sensitivity when the center button is pressed
    adjustSensitivity();
    delay(200); // Debounce delay
  }
  //double click 
  // A 버튼의 상태를 읽어옴
  bool aButtonState = digitalRead(WIO_KEY_A);
  
  // A 버튼이 눌렸고, 이전에 눌리지 않은 상태라면
  if (aButtonState == LOW && !aButtonPressed) {
    // A 버튼이 눌린 시간 기록
    unsigned long currentMillis = millis();
    if (currentMillis - lastAButtonClickTime > DEBOUNCE_DELAY) {
      // A 버튼이 눌린 상태로 설정
      aButtonPressed = true;
      // 마지막 A 버튼 클릭 시간 기록
      lastAButtonClickTime = currentMillis;
      // 클릭 동작 수행
      performSingleClickAction();
    }
  }
  
  // A 버튼이 눌려있지 않은 상태이고, 이전에 눌린 상태라면
  if (aButtonState == HIGH && aButtonPressed) {
    // A 버튼이 눌려있지 않은 상태로 설정
    aButtonPressed = false;
  }
  }

// 클릭 동작 수행 함수 (단일 클릭)
void performSingleClickAction() {
  // 여기에 클릭 시 수행할 동작을 추가합니다.
  // 예를 들어, 마우스 클릭 등의 작업을 수행할 수 있습니다.
  Mouse.click();
  delay(50); // 디바운싱을 위한 지연시간
  Mouse.click();
}

