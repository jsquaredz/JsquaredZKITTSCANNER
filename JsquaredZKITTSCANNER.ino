/*
  JsquaredZ KITT Scanner
  ----------------------
  Knight Rider style LED scanner using the SoftPWM library.

  Wiring (Arduino Uno or similar):
    LEDs  -> digital pins 2..10 (through resistors), left to right
    A0    -> pot: maximum brightness (40% .. 100%)
    A1    -> pot: scan speed, time the head spends on each LED (25 .. 400 ms)

  How the effect works:
    Every frame, every LED's brightness decays by a fixed factor, then the
    head is painted at full brightness on top. That leaves a trail that
    fades out behind the head, keeps fading while the head pauses at each
    end, and always follows the direction of travel. The head moves in
    sub-LED steps so it glides instead of hopping, and brightness is
    gamma corrected so the dim tail steps are actually visible.
*/

#include <SoftPWM.h>

// ---------- Hardware ----------
const uint8_t LED_PINS[] = {2, 3, 4, 5, 6, 7, 8, 9, 10};
const uint8_t NUM_LEDS   = sizeof(LED_PINS) / sizeof(LED_PINS[0]);
const uint8_t BRIGHTNESS_POT = A0;
const uint8_t SPEED_POT      = A1;

// ---------- Look and feel (tune these) ----------
const uint8_t SUBSTEPS       = 4;     // frames per LED; higher = smoother glide
const float   TAIL_FALLOFF   = 0.40;  // brightness kept per LED behind the head (0..1)
                                      // 0.3 = short tail, 0.5 = long tail
const float   END_HOLD_RATIO = 0.75;  // pause at each end, as a fraction of time-per-LED
const float   CUTOFF         = 0.02;  // below this an LED is simply off

// ---------- State ----------
float  level[NUM_LEDS];   // current brightness 0..1, before gamma
float  head = 0;          // head position in LED units, 0 .. NUM_LEDS-1
int8_t dir  = 1;          // +1 scanning right, -1 scanning left
float  decayPerFrame;     // derived from TAIL_FALLOFF and SUBSTEPS

void setup()
{
  Serial.begin(9600);

  SoftPWMBegin();
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    SoftPWMSet(LED_PINS[i], 0);
    level[i] = 0;
  }
  // We draw the fades ourselves, so turn off the library's own fading.
  SoftPWMSetFadeTime(ALL, 0, 0);

  // Decay so that one full LED of travel leaves TAIL_FALLOFF of the brightness.
  decayPerFrame = pow(TAIL_FALLOFF, 1.0 / SUBSTEPS);
}

// Pot on A0 -> maximum brightness in percent, 40..100.
uint8_t maxBrightnessPercent()
{
  return map(analogRead(BRIGHTNESS_POT), 0, 1023, 40, 100);
}

// Pot on A1 -> milliseconds the head spends on each LED, 25..400.
uint16_t msPerLed()
{
  return map(analogRead(SPEED_POT), 0, 1023, 25, 400);
}

// Fade every LED a little.
void decayTail()
{
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    level[i] *= decayPerFrame;
  }
}

// Paint the head: full brightness at its position, split across the two
// nearest LEDs when it is between them. Only ever brightens, never dims,
// so the tail that is already there is preserved.
void paintHead()
{
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    float d = fabs((float)i - head);
    if (d < 1.0) {
      float k = 1.0 - d;
      if (k > level[i]) level[i] = k;
    }
  }
}

// Push the buffer out to the LEDs with gamma and the brightness pot applied.
void render(uint8_t maxPct)
{
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    float b = level[i];
    if (b < CUTOFF) b = 0;
    b = b * b;  // gamma 2.0: the eye sees LED brightness roughly logarithmically
    SoftPWMSet(LED_PINS[i], (uint8_t)(b * maxPct * 2.55 + 0.5));
  }
}

// One animation frame.
void frame(uint16_t frameMs)
{
  decayTail();
  paintHead();
  render(maxBrightnessPercent());
  delay(frameMs);
}

void printSettings(uint16_t perLed, uint16_t holdMs)
{
  Serial.println(F("------ KITT scanner ------"));
  Serial.print(F("Time per LED (ms):   ")); Serial.println(perLed);
  Serial.print(F("End hold (ms):       ")); Serial.println(holdMs);
  Serial.print(F("Max brightness (%):  ")); Serial.println(maxBrightnessPercent());
  Serial.print(F("LEDs:                ")); Serial.println(NUM_LEDS);
}

void loop()
{
  uint16_t perLed  = msPerLed();
  uint16_t frameMs = max(perLed / SUBSTEPS, 1);

  // Glide the head one LED in SUBSTEPS small moves.
  for (uint8_t s = 0; s < SUBSTEPS; s++) {
    head += dir * (1.0 / SUBSTEPS);
    frame(frameMs);
  }
  head = round(head);  // kill float drift

  // Reached an end: reverse, and hold while the tail keeps fading.
  if (head <= 0 || head >= NUM_LEDS - 1) {
    head = constrain(head, 0, NUM_LEDS - 1);
    dir  = -dir;

    uint8_t holdFrames = (uint8_t)(SUBSTEPS * END_HOLD_RATIO + 0.5);
    for (uint8_t h = 0; h < holdFrames; h++) {
      frame(frameMs);
    }

    if (head == 0) {  // once per full round trip
      printSettings(perLed, holdFrames * frameMs);
    }
  }
}
