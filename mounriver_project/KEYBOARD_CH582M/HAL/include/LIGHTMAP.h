/********************************** (C) COPYRIGHT *******************************
* File Name          : LIGHTMAP.h
* Author             : ChnMasterOG
* Version            : V1.0.0
* Date               : 2024/7/18
* Description        : TP78 lightmap
*********************************************************************************
* Copyright (c) 2024 ChnMasterOG
*******************************************************************************/

#ifndef LIGHTMAP_H
#define LIGHTMAP_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "HAL.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LIGHTMAP_WIDTH           304000     /*!< 30.4 cm */
#define LIGHTMAP_HEIGHT          114000     /*!< 11.4 cm */
#define LIGHTMAP_DEPTH           25000      /*!< 2.5 cm */
#define LIGHTMAP_UPDATE_INTERVAL (WS2812_TASK_PERIOD_MS * 1000)

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t intensity;
} __attribute__((packed, aligned(1))) LampColor;

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t z;
} __attribute__((packed, aligned(1))) Position;

/* x, y, z */
static const Position LampPositions[LED_NUMBER] = {
    {9500, 9500, 0}, {33250, 9500, 0}, {52250, 9500, 0}, {71250, 9500, 0}, {90250, 9500, 0}, {114000, 9500, 0}, {133000, 9500, 0}, {152000, 9500, 0}, {171000, 9500, 0}, {194750, 9500, 0}, {213750, 9500, 0}, {232750, 9500, 0}, {251750, 9500, 0}, {275500, 9500, 0},
    {9500, 28500, 0}, {28500, 28500, 0}, {47500, 28500, 0}, {66500, 28500, 0}, {85500, 28500, 0}, {104500, 28500, 0}, {123500, 28500, 0}, {142500, 28500, 0}, {161500, 28500, 0}, {180500, 28500, 0}, {199500, 28500, 0}, {218500, 28500, 0}, {237500, 28500, 0}, {266000, 28500, 0},
    {14250, 47500, 0}, {38000, 47500, 0}, {57700, 47500, 0}, {76000, 47500, 0}, {95000, 47500, 0}, {114000, 47500, 0}, {133000, 47500, 0}, {152000, 47500, 0}, {171000, 47500, 0}, {190000, 47500, 0}, {209000, 47500, 0}, {228000, 47500, 0}, {247000, 47500, 0}, {270750, 47500, 0}, {294500, 28500, 0},
    {16625, 66500, 0}, {42750, 66500, 0}, {61750, 66500, 0}, {80750, 66500, 0}, {99750, 66500, 0}, {118750, 66500, 0}, {137750, 66500, 0}, {156750, 66500, 0}, {175750, 66500, 0}, {194750, 66500, 0}, {213750, 66500, 0}, {232750, 66500, 0}, {263625, 66500, 0}, {294500, 47500, 0},
    {21375, 85500, 0}, {52250, 85500, 0}, {71250, 85500, 0}, {90250, 85500, 0}, {109250, 85500, 0}, {128250, 85500, 0}, {147250, 85500, 0}, {166250, 85500, 0}, {185250, 85500, 0}, {204250, 85500, 0}, {223250, 85500, 0}, {249375, 85500, 0},
    {11875, 104500, 0}, {35625, 104500, 0}, {59375, 104500, 0}, {92625, 104500, 0}, {166250, 104500, 0}, {197125, 104500, 0}, {216125, 104500, 0}, {235125, 104500, 0}, {254125, 104500, 0}, {273125, 104500, 0}, {292125, 104500, 0}, {273125, 85500, 0}, {294500, 85500, 0}, {294500, 66500, 0},
};

typedef struct {
    uint16_t lampCount;

    uint32_t width;
    uint32_t height;
    uint32_t depth;

    uint32_t lampArrayKind;
    uint32_t minUpdateInterval;
} __attribute__((packed, aligned(1))) LampArrayAttributesReport;

typedef struct {
    uint16_t lampId;
} __attribute__((packed, aligned(1))) LampAttributesRequestReport;

#define LAMP_PURPOSE_CONTROL        0x01
#define LAMP_PURPOSE_ACCENT         0x02
#define LAMP_PURPOSE_BRANDING       0x04
#define LAMP_PURPOSE_STATUS         0x08
#define LAMP_PURPOSE_ILLUMINATION   0x10
#define LAMP_PURPOSE_PRESENTATION   0x20

typedef struct {
    uint16_t lampId;

    Position lampPosition;

    uint32_t updateLatency;
    uint32_t lampPurposes;

    uint8_t redLevelCount;
    uint8_t greenLevelCount;
    uint8_t blueLevelCount;
    uint8_t intensityLevelCount;

    uint8_t isProgrammable;
    uint8_t inputBinding;
} __attribute__((packed, aligned(1))) LampAttributesResponseReport;

typedef struct {
    uint8_t lampCount;
    uint8_t flags;
    uint16_t lampIds[8];

    LampColor colors[8];
} __attribute__((packed, aligned(1))) LampMultiUpdateReport;

typedef struct {
    uint8_t flags;
    uint16_t lampIdStart;
    uint16_t lampIdEnd;

    LampColor color;
} __attribute__((packed, aligned(1))) LampRangeUpdateReport;

typedef struct {
    uint8_t autonomousMode;
} __attribute__((packed, aligned(1))) LampArrayControlReport;

#ifdef __cplusplus
}
#endif

#endif
