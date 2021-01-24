/*
  Rainbow Button MIDI
  Nigel James Brown - January 24th 2021

  Each pin of the Pro-Micro should be connected to one side of a button
  The other side of all the buttons should be connected to the ground pin

  Each of the pins - listed in buttonPinMap are set as inputs with the built in pull-up resistors
  When one of the pins changes state a MIDI Note for press or release will be sent from the buttonKeyMap array

  To expand this - change the NUM_BUTTONS value and increase the size of the three arrays
  buttonKeyMap, buttonPinMap and keyState.

  Note: keyState values should all be initialised to true to match the pull-up value
  
*/

// **** MIDI SETUP ****
#include <MIDIUSB.h>
#define NUM_BUTTONS 3
uint8_t buttonKeyMap[NUM_BUTTONS] = {60, 61, 62}; // Middle C, C# and D
uint8_t buttonPinMap[NUM_BUTTONS] = {2,3,4};
bool keyState[NUM_BUTTONS] = {true, true, true};


// **** LED SETUP ****

#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN    5

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 3
#define MIN_BRIGHTNESS 8
uint8_t pixelBrightness[LED_COUNT] = {MIN_BRIGHTNESS, MIN_BRIGHTNESS, MIN_BRIGHTNESS};

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

void setup() {
  //start serial connection
  // Serial.begin(9600);

  // Set all inputs to internal pullups
  for (int i=0; i<NUM_BUTTONS; i++)
  {
    pinMode(buttonPinMap[i], INPUT_PULLUP);
  }

  // No MIDI setup required

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS to max = 255
}


void loop() {
  //read the pushbutton value into a variable
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    int sensorVal = digitalRead(buttonPinMap[i]);
    if (sensorVal != keyState[i])
    {
      // The value has changed
      if (sensorVal == 0)
      {
        // Key has been pressed
        noteOn(0, buttonKeyMap[i], 127);

        pixelBrightness[i] = 255;

        // delay here
        delay(100);

      }
      else
      {
        // Key has been released
        noteOff(0, buttonKeyMap[i], 0);

        // delay here
        delay(100);

      }
      keyState[i] = sensorVal;
    }
    //print out the value of the pushbutton
    // Serial.println(sensorVal);

    if (pixelBrightness[i] > MIN_BRIGHTNESS)
    {
      pixelBrightness[i]--;
    }
  }

  // Send the latest MIDI events
  MidiUSB.flush();

  // Set up the three buttons as Red, Green and Blue
  strip.setPixelColor(0, strip.Color(pixelBrightness[0],0,0));
  strip.setPixelColor(1, strip.Color(0,pixelBrightness[1],0));
  strip.setPixelColor(2, strip.Color(0,0,pixelBrightness[2]));

  strip.show();
  delay(5);
  
}


// MIDI Functions
// --------------

void noteOn(byte channel, byte pitch, byte velocity) {

  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};

  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {

  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};

  MidiUSB.sendMIDI(noteOff);
}
