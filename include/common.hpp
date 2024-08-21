#pragma once

#include <cstdint>
#include <QRgb>

enum DIRECTION { UNKNOWN, UP = 1, LEFT, RIGHT, DOWN, INVALID };
static_assert(UP + DOWN == INVALID);
static_assert(LEFT + RIGHT == INVALID);

constexpr int MAP_W = 48;
constexpr int MAP_H = 35;

constexpr int TILE_W = 16;
constexpr int TILE_H = 16;

constexpr QRgb BORDER_COLOR = qRgb(0, 255, 0);
constexpr QRgb BODY_COLOR = qRgb(255, 0, 0);
constexpr QRgb HEAD_COLOR = qRgb(255, 255, 0);
constexpr QRgb FOOD_COLOR = qRgb(0, 0, 255);

enum MAP_TYPE : uint8_t {
  TYPE_EMPTY = 0,
  TYPE_SNAKE = 1,
  TYPE_FOOD  = 2,
  TYPE_HEAD  = 5, // TYPE_SNAKE | 0x04
};

template <typename T> class QList;
class QPoint;

void relocateRegion();
class QImage;
QImage screenshot();

template <int W, int H>
class PrintableMap;
bool updateMap(PrintableMap<MAP_W, MAP_H>& map);

QList<DIRECTION> findPath(PrintableMap<MAP_W, MAP_H>&map);
