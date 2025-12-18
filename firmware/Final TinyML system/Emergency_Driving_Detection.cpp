#include "Particle.h"
#include "ADXL343.h"

extern "C" {
  #include "features.h"
  #include "rf10_goertzel_model.h"
}

SYSTEM_THREAD(ENABLED);

// =====================================================
// KONFIGURATION
// =====================================================
#define FS_HZ           100
#define WINDOW_SIZE     200
#define STEP_SIZE       100

#define LED1 D2
#define LED2 D3
#define LED3 D4

#define PRED_DEBOUNCE   3   // 3 ens predictions i træk

// =====================================================
// GLOBALER
// =====================================================
ADXL343 accel;

static float acc_window[WINDOW_SIZE][3];
static int sample_index = 0;

static int pred_history[PRED_DEBOUNCE];
static int pred_idx = 0;
static bool udrykning_active = false;

// =====================================================
// LED-FUNKTIONER
// =====================================================
void ledsOff() {
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
}

void blinkUdrykning() {
  static bool state = false;
  state = !state;

  digitalWrite(LED1, state);
  digitalWrite(LED2, !state);
  digitalWrite(LED3, state);
}

// =====================================================
// DEBOUNCE AF PREDICTION
// =====================================================
bool debouncePrediction(int pred) {
  pred_history[pred_idx++] = pred;
  if (pred_idx >= PRED_DEBOUNCE) pred_idx = 0;

  for (int i = 0; i < PRED_DEBOUNCE; i++) {
    if (pred_history[i] != pred) return false;
  }
  return true;
}

// =====================================================
// SETUP
// =====================================================
void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  ledsOff();

  delay(2000);

  accel.begin();

  for (int i = 0; i < PRED_DEBOUNCE; i++) {
    pred_history[i] = 0;
  }
}

// =====================================================
// LOOP
// =====================================================
void loop() {
  static unsigned long last_sample = 0;
  static unsigned long last_blink  = 0;
  unsigned long now = millis();

  // ---------------------------------------------
  // Sample accelerometer @ 100 Hz
  // ---------------------------------------------
  if (now - last_sample >= 1000 / FS_HZ) {
    last_sample = now;

    float x, y, z;
    accel.readAccelerationG(&x, &y, &z);

    acc_window[sample_index][0] = x;
    acc_window[sample_index][1] = y;
    acc_window[sample_index][2] = z;

    sample_index++;

    // ---------------------------------------------
    // Når vinduet er fyldt → inference
    // ---------------------------------------------
    if (sample_index >= WINDOW_SIZE) {
      float features[NUM_FEATURES];
      extract_features(acc_window, features);

      int pred = rf10_goertzel_model_predict(features, NUM_FEATURES);

      if (debouncePrediction(pred)) {
        udrykning_active = (pred == 1);
      }

      // Sliding window: 50 % overlap
      for (int i = 0; i < WINDOW_SIZE - STEP_SIZE; i++) {
        acc_window[i][0] = acc_window[i + STEP_SIZE][0];
        acc_window[i][1] = acc_window[i + STEP_SIZE][1];
        acc_window[i][2] = acc_window[i + STEP_SIZE][2];
      }

      sample_index = WINDOW_SIZE - STEP_SIZE;
    }
  }

  // ---------------------------------------------
  // LED-logik
  // ---------------------------------------------
  if (udrykning_active) {
    if (now - last_blink >= 300) {   // blink-hastighed
      last_blink = now;
      blinkUdrykning();
    }
  } else {
    ledsOff();
  }
}
