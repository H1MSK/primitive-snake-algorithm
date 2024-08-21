#include <QGuiApplication>
#include <QImage>
#include <QPixmap>
#include <QScreen>
#include "common.hpp"
#include "map.hpp"

bool updateMap(PrintableMap<MAP_W, MAP_H>& map) {
  QImage img = screenshot();
  int tail_removed = 0, body_extended = 0;
  QPoint dummy;
  QPoint food_pos, head_pos;

  // MAP_TYPE->QPoint*
  QPoint* pos_lut[] = {&dummy, &dummy, &food_pos, nullptr, nullptr, &head_pos};
  for (int y = 0; y < MAP_H; ++y) {
    for (int x = 0; x < MAP_W; ++x) {
      auto pix = ((QRgb*)img.constScanLine(
          y * TILE_H + TILE_H * 3 / 2))[x * TILE_W + TILE_W * 3 / 2];
      MAP_TYPE t;
      switch (pix) {
        case FOOD_COLOR:
          t = TYPE_FOOD;
          break;
        case BODY_COLOR:
          t = TYPE_SNAKE;
          break;
        case HEAD_COLOR:
          t = TYPE_HEAD;
          break;
        default:
          t = TYPE_EMPTY;
          break;
      }
      *pos_lut[t] = QPoint(x, y);
      t = (MAP_TYPE)(t & 0x03);
      tail_removed += (map.at(x, y) > 0 && t != TYPE_SNAKE);
      body_extended += (map.at(x, y) == 0 && t == TYPE_SNAKE);
    }
  }
  if (body_extended > 1 || tail_removed > 1)
    qCritical() << "Snake move too fast! body_extended =" << body_extended
                << ", tail_removed =" << tail_removed;
  QPoint delta = head_pos - map.snake().head();

  if (delta == QPoint(0, 0))
    return false;

  bool keep_tail = tail_removed == 0;
  if (delta == QPoint(-1, 0))
    map.move(LEFT, keep_tail);
  else if (delta == QPoint(0, -1))
    map.move(UP, keep_tail);
  else if (delta == QPoint(1, 0))
    map.move(RIGHT, keep_tail);
  else if (delta == QPoint(0, 1))
    map.move(DOWN, keep_tail);
  else
    qCritical() << "Unknown head move direction! delta =" << delta;

  if (food_pos != map.food_pos()) {
    map.food_pos() = food_pos;
  }

  return true;
}
