#ifndef FEATURES_H
#define FEATURES_H

#define WINDOW_SIZE 200
#define NUM_AXES 3
#define NUM_FEATURES 10

void extract_features(
    const float acc[WINDOW_SIZE][NUM_AXES],
    float features[NUM_FEATURES]
);

#endif
