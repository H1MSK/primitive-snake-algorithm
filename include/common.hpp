#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint>

enum DIRECTION { UNKNOWN, UP = 1, LEFT, RIGHT, DOWN, INVALID };
static_assert(UP + DOWN == INVALID);
static_assert(LEFT + RIGHT == INVALID);

constexpr int MAP_W = 48;
constexpr int MAP_H = 35;


constexpr static char CH_LUT[] = " #O";
enum MAP_TYPE : uint8_t {
  TYPE_EMPTY = 0,
  TYPE_SNAKE = 1,
  TYPE_FOOD  = 2,
  TYPE_HEAD  = 5, // TYPE_SNAKE | 0x04
};


extern MAP_TYPE map[MAP_H][MAP_W];
extern char map_str[MAP_H + 2][MAP_W + 3];
extern DIRECTION direction;

template <typename T> class QList;
class QPoint;
extern QList<QPoint> path;
extern QPoint food_pos, head_pos;

void findPath();

#endif // COMMON_HPP
