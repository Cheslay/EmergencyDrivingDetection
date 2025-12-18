#include "features.h"
#include <math.h>
#include <string.h>

#define PI 3.14159265358979323846f

/* -------------------- helpers -------------------- */

static float rms(const float *v, int n)
{
    float s = 0.0f;
    for (int i = 0; i < n; i++) s += v[i] * v[i];
    return sqrtf(s / n);
}

static float energy(const float *v, int n)
{
    float s = 0.0f;
    for (int i = 0; i < n; i++) s += v[i] * v[i];
    return s;
}

static float vmax(const float *v, int n)
{
    float m = v[0];
    for (int i = 1; i < n; i++)
        if (v[i] > m) m = v[i];
    return m;
}

static float kurtosis(const float *v, int n)
{
    float mean = 0.0f;
    for (int i = 0; i < n; i++) mean += v[i];
    mean /= n;

    float m2 = 0.0f, m4 = 0.0f;
    for (int i = 0; i < n; i++) {
        float d = v[i] - mean;
        float d2 = d * d;
        m2 += d2;
        m4 += d2 * d2;
    }

    m2 /= n;
    m4 /= n;

    // 🔒 Guard against zero variance
    if (m2 < 1e-12f) {
        return 0.0f;
    }

    return (m4 / (m2 * m2)) - 3.0f;
}

static float percentile(float *v, int n, float p)
{
    // Simple full insertion sort (n=200 → OK)
    for (int i = 1; i < n; i++) {
        float key = v[i];
        int j = i - 1;
        while (j >= 0 && v[j] > key) {
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = key;
    }

    int k = (int)(p * (n - 1));
    return v[k];
}

/* -------------------- Goertzel -------------------- */

static float goertzel_band_energy(
    const float *v, int n, float fs, float f1, float f2)
{
    float energy = 0.0f;

    for (int f = (int)f1; f <= (int)f2; f++) {
        int k = (int)(0.5f + (n * f) / fs);
        float w = 2.0f * PI * k / n;
        float coeff = 2.0f * cosf(w);

        float s_prev = 0.0f, s_prev2 = 0.0f;
        for (int i = 0; i < n; i++) {
            float s = v[i] + coeff * s_prev - s_prev2;
            s_prev2 = s_prev;
            s_prev = s;
        }

        float power = s_prev2*s_prev2 + s_prev*s_prev
                      - coeff*s_prev*s_prev2;
        energy += power;
    }

    return energy;
}

/* -------------------- main API -------------------- */

void extract_features(
    const float acc[WINDOW_SIZE][NUM_AXES],
    float features[NUM_FEATURES]
)
{
    float x[WINDOW_SIZE], y[WINDOW_SIZE], z[WINDOW_SIZE];
    float mag[WINDOW_SIZE];

    for (int i = 0; i < WINDOW_SIZE; i++) {
        x[i] = acc[i][0];
        y[i] = acc[i][1];
        z[i] = acc[i][2];
        mag[i] = sqrtf(x[i]*x[i] + y[i]*y[i] + z[i]*z[i]);
    }

    features[0] = goertzel_band_energy(y,   WINDOW_SIZE, 100, 15, 40);
    features[1] = goertzel_band_energy(y,   WINDOW_SIZE, 100, 5, 15);
    features[2] = goertzel_band_energy(mag, WINDOW_SIZE, 100, 15, 40);
    features[3] = goertzel_band_energy(x,   WINDOW_SIZE, 100, 15, 40);
    features[4] = goertzel_band_energy(z,   WINDOW_SIZE, 100, 15, 40);

    features[5] = kurtosis(y, WINDOW_SIZE);
    features[6] = rms(y, WINDOW_SIZE);
    features[7] = energy(y, WINDOW_SIZE);
    features[8] = vmax(y, WINDOW_SIZE);

    float y_copy[WINDOW_SIZE];
    memcpy(y_copy, y, sizeof(y_copy));
    features[9] = percentile(y_copy, WINDOW_SIZE, 0.10f);
}
