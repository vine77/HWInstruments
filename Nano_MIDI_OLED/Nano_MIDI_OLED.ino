// OLED Screen
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
#include <./Fonts/PicoPixel.h>
#include "midi.h"

//-----------OLED Screen---------
//CLK on Pin D13
//MOSI on Pin D11
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to Hardware SPI
#define OLED_DC     7
#define OLED_CS     8
#define OLED_RESET  9
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS);

int scrollOffset = 0;
int xpos = 0;
int screensaverdwell = 0;
#define MS_UNTIL_SCREENSAVER 1000 * 10 * 1
//-----------OLED Screen---------


//--------MIDI OUTPUT--------
//MIDI Center Pin connected to TX
unsigned long BPM = 240;
unsigned long lastMillisMidi = 0;
unsigned long noteDelay = 60000 / BPM;
int note = 0x1E;
int arpUDmin[] = {0, 3, 7, 3};
int arpUmin[] = {0,3,7,12};
int arpUDmaj[] = {0,4,7,4};
int arpUmaj[] = {0, 4, 7, 12};
int* arp;
int arpPos = 0;
int rootNote = 69; //69 is A 440, A4
//Note numbers
//Octave   Note Numbers
//         C  C#   D  D#   E   F  F#   G  G#   A  A#   B
//-1       0   1   2   3   4   5   6   7   8   9  10  11
//0       12  13  14  15  16  17  18  19  20  21  22  23
//1       24  25  26  27  28  29  30  31  32  33  34  35
//2       36  37  38  39  40  41  42  43  44  45  46  47
//3       48  49  50  51  52  53  54  55  56  57  58  59
//4       60  61  62  63  64  65  66  67  68  69  70  71
//5       72  73  74  75  76  77  78  79  80  81  82  83
//6       84  85  86  87  88  89  90  91  92  93  94  95
//7       96  97  98  99 100 101 102 103 104 105 106 107
//8      108 109 110 111 112 113 114 115 116 117 118 119
//9      120 121 122 123 124 125 126 127
//--------MIDI OUTPUT--------


unsigned long currentMillis = 0;
unsigned long lastMillisDisplay = 0;
bool blinkled = 0;

unsigned long lastMillisArp = 0;
bool arpsel = false;

unsigned long lastMillisRead = 0;
int anaIN7 = 0; //pot
int anaIN6 = 0; //ring
int anaIN5 = 0; //middle
int anaIN4 = 0; //index
int anaIN3 = 0; //thumb
int readIndex = 1;


static const unsigned char PROGMEM  penrose_icon [] = {
0x00, 0x00, 0x80, 0x00, 0x00, 0x01, 0xC0, 0x00, 0x00, 0x03, 0xE0, 0x00, 0x00, 0x06, 0xB0, 0x00,
0x00, 0x0A, 0xA8, 0x00, 0x00, 0x14, 0x94, 0x00, 0x00, 0x24, 0x92, 0x00, 0x00, 0x48, 0x89, 0x00,
0x00, 0x88, 0x88, 0x80, 0x01, 0x10, 0x84, 0x40, 0x02, 0x10, 0x84, 0x20, 0x04, 0x1F, 0xFC, 0x10,
0x08, 0x70, 0x87, 0x08, 0x13, 0xA0, 0x82, 0xC4, 0x2C, 0x20, 0x82, 0x32, 0x70, 0x40, 0x81, 0x0D,
0xFF, 0xFF, 0xFF, 0xFF, 0x70, 0x40, 0x81, 0x07, 0x2C, 0x40, 0x81, 0x1A, 0x13, 0x20, 0x82, 0x64,
0x08, 0xE0, 0x83, 0x88, 0x04, 0x38, 0x8E, 0x10, 0x02, 0x17, 0xF4, 0x20, 0x01, 0x10, 0x84, 0x40,
0x00, 0x88, 0x84, 0x80, 0x00, 0x48, 0x89, 0x00, 0x00, 0x24, 0x92, 0x00, 0x00, 0x14, 0x94, 0x00,
0x00, 0x0A, 0xA8, 0x00, 0x00, 0x05, 0xB0, 0x00, 0x00, 0x03, 0xE0, 0x00, 0x00, 0x01, 0xC0, 0x00
};
static const unsigned char PROGMEM penrose_logo [] = {
0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x06, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x0A, 0xA8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x14, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x24, 0x92, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x48, 0x89, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x88, 0x88, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x01, 0x10, 0x84, 0x40, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x02, 0x10, 0x84, 0x20, 0x01, 0xF8, 0x00, 0x00, 0x03, 0x21, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00,
0x04, 0x1F, 0xFC, 0x10, 0x01, 0x8C, 0x00, 0x00, 0x01, 0x21, 0x8C, 0x00, 0x00, 0x00, 0x00, 0x00,
0x08, 0x70, 0x87, 0x08, 0x01, 0x84, 0x1E, 0x1B, 0xC1, 0xC1, 0x8C, 0x1E, 0x0F, 0x81, 0xE0, 0x00,
0x13, 0xA0, 0x82, 0xC4, 0x01, 0x84, 0x21, 0x1C, 0x63, 0x81, 0x8C, 0x21, 0x18, 0x02, 0x10, 0x00,
0x2C, 0x20, 0x82, 0x32, 0x01, 0x8C, 0x61, 0x98, 0x66, 0x8D, 0x98, 0x61, 0x98, 0x06, 0x18, 0x00,
0x70, 0x40, 0x81, 0x0D, 0x01, 0xF0, 0x7F, 0x98, 0x6C, 0xCD, 0xF0, 0x40, 0x8E, 0x07, 0xF8, 0x00,
0xFF, 0xFF, 0xFF, 0xFF, 0x81, 0x80, 0x60, 0x18, 0x6C, 0x69, 0x90, 0x40, 0x83, 0x86, 0x00, 0x00,
0x70, 0x40, 0x81, 0x07, 0x01, 0x80, 0x60, 0x18, 0x6C, 0x39, 0x88, 0x61, 0x80, 0xC6, 0x00, 0x00,
0x2C, 0x40, 0x81, 0x1A, 0x01, 0x80, 0x30, 0x18, 0x66, 0x39, 0x8C, 0x21, 0x00, 0xC3, 0x00, 0x00,
0x13, 0x20, 0x82, 0x64, 0x01, 0x80, 0x1F, 0x18, 0x63, 0xF9, 0x86, 0x1E, 0x1F, 0x01, 0xF0, 0x00,
0x08, 0xE0, 0x83, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x04, 0x38, 0x8E, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x02, 0x17, 0xF4, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x01, 0x10, 0x84, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x88, 0x84, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x48, 0x89, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x24, 0x92, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x14, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x0A, 0xA8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x05, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM penrose_logo_big [] = {
0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x06, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x0A, 0xA8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x14, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x24, 0x92, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x48, 0x89, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x88, 0x88, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x01, 0x10, 0x84, 0x40, 0x00, 0x00, 0x00, 0x00, 0x01, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x02, 0x10, 0x84, 0x20, 0x07, 0xF0, 0x00, 0x00, 0x01, 0x98, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00,
0x04, 0x1F, 0xFC, 0x10, 0x07, 0xFC, 0x00, 0x00, 0x01, 0x98, 0x7F, 0x80, 0x00, 0x00, 0x00, 0x00,
0x08, 0x70, 0x87, 0x08, 0x06, 0x0C, 0x1E, 0x19, 0xE1, 0xB0, 0x61, 0x83, 0xC0, 0x7E, 0x07, 0x80,
0x13, 0xA0, 0x82, 0xC4, 0x06, 0x0C, 0x7F, 0x1F, 0xF0, 0xE0, 0x61, 0x8F, 0xF0, 0xFE, 0x1F, 0xC0,
0x2C, 0x20, 0x82, 0x32, 0x06, 0x1C, 0x63, 0x9E, 0x31, 0xC0, 0x63, 0x8C, 0x30, 0xC0, 0x18, 0xE0,
0x70, 0x40, 0x81, 0x0D, 0x07, 0xF8, 0xC1, 0x98, 0x33, 0x63, 0x7F, 0x18, 0x18, 0xE0, 0x30, 0x60,
0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0xF0, 0xFF, 0x98, 0x36, 0x73, 0x7E, 0x18, 0x18, 0x7C, 0x3F, 0xE0,
0x70, 0x40, 0x81, 0x07, 0x06, 0x00, 0xFF, 0x98, 0x36, 0x33, 0x67, 0x18, 0x18, 0x1E, 0x3F, 0xE0,
0x2C, 0x40, 0x81, 0x1A, 0x06, 0x00, 0xC0, 0x18, 0x36, 0x1E, 0x63, 0x18, 0x18, 0x03, 0x30, 0x00,
0x13, 0x20, 0x82, 0x64, 0x06, 0x00, 0xE0, 0x98, 0x37, 0x1E, 0x61, 0x8C, 0x30, 0x83, 0x38, 0x20,
0x08, 0xE0, 0x83, 0x88, 0x06, 0x00, 0x7F, 0x98, 0x33, 0xFE, 0x61, 0xCF, 0xF0, 0xFF, 0x1F, 0xE0,
0x04, 0x38, 0x8E, 0x10, 0x06, 0x00, 0x1F, 0x18, 0x31, 0xF7, 0x60, 0xE3, 0xC0, 0x7C, 0x07, 0xC0,
0x02, 0x17, 0xF4, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x01, 0x10, 0x84, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x88, 0x84, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x48, 0x89, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x24, 0x92, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x14, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x0A, 0xA8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x05, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void setup() {

//--------MIDI OUTPUT--------
//  Set MIDI baud rate:
Serial.begin(32150);
//--------MIDI OUTPUT--------
pinMode(A7,INPUT); //pot
pinMode(A3,INPUT); //thumb
pinMode(A4,INPUT); //index
pinMode(A5,INPUT); //middle
pinMode(A6,INPUT); //ring

//  //SPI Initialize
    if(!display.begin(SSD1306_SWITCHCAPVCC)){
    while(1){
      currentMillis = millis();
      if (currentMillis - lastMillisDisplay >1000){
        lastMillisDisplay = currentMillis;
        digitalWrite(LED_BUILTIN,blinkled ^= 1);
      } //end if blink time
    }//end while no screen
  } //end if no screen
  
  display.setTextWrap(false);
  display.clearDisplay();
  display.drawBitmap(0,0,penrose_logo,128,33,1);
  display.setFont(&Picopixel);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(40,27);
  display.print(F("MIDI Arpeggiator"));
  display.setFont(NULL);
  display.display();
  note = rootNote;
  arp = arpUmin;
  delay(4000);
} //end setup

void loop() {
  currentMillis = millis();

//read the analog inputs
  switch(readIndex++){
    case 1:
      anaIN3 = analogRead(A3); // thumb
      //freq low pass
      break;
    case 2:
      anaIN4 = analogRead(A4); //index
      //ADSR release
      break;
    case 3:
      anaIN5 = analogRead(A5); //middle
      //num note in Arpeggio
      break;
    case  4:
      anaIN6 = analogRead(A6); //ring
      int arpSel = map(anaIN6,4,1020,0,3);
      switch(arpSel){
        case 0:
          arp = arpUDmin;
          break;
        case 1:
          arp = arpUmin;
          break;
        case 2:
          arp = arpUDmaj;
          break;
        case 3:
          arp = arpUmaj;
          break;
        default:
          break;
      }
      break;

      case 5:
        anaIN7 = analogRead(A7); //pot
        anaIN7 = 512;
        rootNote = map(anaIN7,465,785,57,69);
        readIndex = 0;
      break;

      default:
        readIndex = 0;
      break;
    
    } //end read index switch case

  
//  if(currentMillis - lastMillisDisplay > 2000){
//    display.clearDisplay();
//    display.drawBitmap(0,0,penrose_logo_big,128,33,1);
//      display.setFont(&Picopixel);
//  display.setTextSize(1);
//  display.setTextColor(SSD1306_WHITE);
//  display.setCursor(40,27);
//  display.print(F("NANO+OLED"));
//  display.print(F("  "));
//  display.print(F("SPI comms"));
//  display.setFont(NULL);
//    display.display();
//    lastMillisDisplay = currentMillis;
//  }

  if(currentMillis - lastMillisMidi > noteDelay){
    int arpLen = sizeof(arp)/sizeof(arp[0]);
    arpLen = 4;
    if (arpPos >= arpLen){arpPos = 0;}
    send_noteoff(note, 0x45, 1);
    note = rootNote + arp[arpPos];
    send_noteon(note, 0x45, 1);
    display.clearDisplay();
    display.setCursor(5,5);
    display.print(F("3:"));
    display.print(anaIN3);
    display.print(F(" ,4:"));
    display.print(anaIN4);
    display.print(F(" ,5:"));
    display.print(anaIN5);
    display.setCursor(5,20);
    display.print(F("6:"));
    display.print(anaIN6);
    display.print(F(" ,7:"));
    display.print(anaIN7);
    display.display();
    arpPos++;
    lastMillisMidi = currentMillis;
  }


 
} // end Main Loop
