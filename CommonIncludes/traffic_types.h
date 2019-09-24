#pragma once

typedef enum {
    false,
    true
} bool;

typedef enum {
    INITIAL,
    ALL_RED,
    FAULT,
    NS_S_G,
    NS_S_Y,
    EW_R_G,
    EW_R_Y,
    EW_S_G,
    EW_S_Y,
    NS_R_G,
    NS_R_Y
} State;

typedef enum {
    PEAK,
    OFF_PEAK
} Mode;