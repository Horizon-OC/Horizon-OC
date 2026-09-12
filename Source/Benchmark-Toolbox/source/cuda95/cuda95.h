#pragma once

// CUDA-95NX backend.

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CUDA95_NUM_PROFILES 4

#define CUDA95_REPEATS_MIN 1u
#define CUDA95_REPEATS_MAX 256u

#define CUDA95_LOG_LINES 8
#define CUDA95_LOG_COLS 96

// Profiles
// 0 = balanced
// 1 = core
// 2 = dram
// 3 = l2

typedef struct {
    uint64_t batches;
    uint64_t detected;
    uint64_t transient;
    uint64_t persistent;
    uint64_t selfFail;
    uint64_t dataBad;
    uint64_t warnings;
    uint64_t timeouts;

    double dispPerSec;
    double gbps;

    uint32_t goldDigest;
    uint32_t goldMismatch;

    uint64_t elapsedSec;

    int profile;
    unsigned repeats;

    int calibrating;
    int running;
    int error;
    char status[96];

    char log[CUDA95_LOG_LINES][CUDA95_LOG_COLS];
    unsigned logLines;
} cuda95_status_t;

void cuda95_start(int profile, unsigned repeats);
void cuda95_stop(void);
int cuda95_running(void);
void cuda95_get(cuda95_status_t *out);

void cuda95_set_profile(int profile);
void cuda95_set_repeats(unsigned repeats);

const char *cuda95_profile_name(int profile);
unsigned cuda95_profile_alu(int profile);
unsigned cuda95_profile_gather(int profile);

#ifdef __cplusplus
}
#endif

