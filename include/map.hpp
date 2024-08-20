#pragma once

#include <ranges>
#include "common.hpp"
#include "snake.hpp"

template<int W, int H>
class Map {
    uint32_t map[H][W];
    int timestamp_now;
    Snake snake;
 public:
    uint32_t& operator [](const QPoint& p) { return map[p.y()][p.x()]; }
    uint32_t& at(const QPoint& p) { return operator[](p); }

    void move(DIRECTION dir, bool keep_tail) {
        if (!keep_tail) {
            at(snake.tail()) = 0;
        }
        snake.move(dir, keep_tail);
        at(snake.head()) = ++timestamp_now;
    }
    void move(const std::vector<DIRECTION> directions, int extend) {
        for (auto& dir : directions) {
            if (extend) {
                move(dir, true);
                --extend;
            } else {
                move(dir, false);
            }
        }
    }

    auto neighbors(QPoint p) {
      constexpr static std::array<QPoint, 4> dirs{QPoint(-1, 0), QPoint(0, -1),
                                                  QPoint(1, 0), QPoint(0, 1)};
      return dirs | std::ranges::transform([&p](QPoint d) { return p + d; }) |
             std::ranges::filter([](QPoint p) {
               return p.x() >= 0 && p.x() < W && p.y() >= 0 && p.y() < H;
             });
    }
};