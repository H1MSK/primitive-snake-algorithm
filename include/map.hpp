#pragma once

#include <QtLogging>
#include <ranges>
#include "common.hpp"
#include "snake.hpp"

template <int W, int H>
class Map {
  Snake _snake;
  int8_t _map[H][W];

 protected:
  Map() : _snake(Snake::initialSnake(W, H)) {
    memset(_map, 0, sizeof(_map));
  }

 public:
  Map(const Map& o) : _snake(o._snake) {
    memcpy(_map, o._map, sizeof(_map));
  }

  const Snake& snake() const { return _snake; }
  Snake& snake() { return _snake; }

  int8_t& operator[](const QPoint& p) { return _map[p.y()][p.x()]; }
  int8_t operator[](const QPoint& p) const { return _map[p.y()][p.x()]; }
  int8_t& at(const QPoint& p) { return operator[](p); }
  int8_t at(const QPoint& p) const { return operator[](p); }
  int8_t& at(int x, int y) { return _map[y][x]; }

  virtual void move(DIRECTION dir, bool keep_tail) {
    if (!keep_tail) {
        at(snake().tail()) = 0;
    }
    snake().move(dir, keep_tail);
    at(snake().head()) = 1;
  }
  void move(const QList<DIRECTION>& directions, int extend) {
    for (auto& dir : directions) {
      if (extend) {
        move(dir, true);
        --extend;
      } else {
        move(dir, false);
      }
    }
  }
};

template <int W, int H>
class PrintableMap : public Map<W, H> {
  char map_str[H + 2][W + 3];
  QPoint fpos;
  char& chr_at(const QPoint& p) { return map_str[p.y() + 1][p.x() + 1]; }

  PrintableMap() : Map<W, H>(), fpos(-1, -1) { memset(map_str, ' ', sizeof(map_str)); }

 public:
  const QPoint& food_pos() const { return fpos; }
  QPoint& food_pos() { return fpos; }

  void move(DIRECTION dir, bool keep_tail) override {
    if (!keep_tail) {
      this->at(this->snake().tail()) = 0;
      chr_at(this->snake().tail()) = ' ';
    }
    this->snake().move(dir, keep_tail);
    this->at(this->snake().head()) = 1;
    chr_at(this->snake().head()) = '#';
  }

  static PrintableMap initialMap() {
    PrintableMap m;
    for (auto& b : m.snake()) {
      m.at(b) = 1;
      m.chr_at(b) = '#';
    }
    m.map_str[0][0] = '+';
    m.map_str[0][W + 1] = '+';
    m.map_str[0][W + 2] = 0;
    m.map_str[H + 1][0] = '+';
    m.map_str[H + 1][W + 1] = '+';
    m.map_str[H + 1][W + 2] = 0;
    for (int i = 1; i <= W; ++i) {
      m.map_str[0][i] = '-';
      m.map_str[H + 1][i] = '-';
    }
    for (int i = 1; i <= H; ++i) {
      m.map_str[i][0] = '|';
      m.map_str[i][W + 1] = '|';
      m.map_str[i][W + 2] = 0;
    }
    return m;
  }

  void print_map() {
    char ch_foodpos, ch_headpos;
    if (fpos.x() != -1) {
      ch_foodpos = chr_at(fpos);
      chr_at(fpos) = 'S';
    }
    ch_headpos = chr_at(this->snake().head());
    chr_at(this->snake().head()) = 'O';

    for (int i = 0; i <= H + 1; ++i)
      qInfo() << map_str[i];

    chr_at(this->snake().head()) = ch_headpos;
    if (fpos.x() != -1) {
      chr_at(fpos) = ch_foodpos;
    }
  }

  constexpr static QPoint deltas[] = {
    QPoint(), 
    QPoint(0, -1),
    QPoint(-1, 0),
    QPoint(1, 0),
    QPoint(0, 1)
  };
  void reserveRescuePath(const QList<DIRECTION>& path) {
    QPoint from = food_pos();
    for (auto& dir: path) {
      this->at(from) = -1;
      chr_at(from) = '*';
      from += deltas[dir];
    }
    this->at(from) = -1;
    chr_at(from) = '#';
  }
  void clearRescuePath(const QList<DIRECTION>& path) {
    QPoint from = food_pos();
    chr_at(from) = 'S';
    for (auto& dir: path) {
      this->at(from) = 0;
      from += deltas[dir];
      chr_at(from) = ' ';
    }
    this->at(from) = 0;
    chr_at(from) = '#';
  }
  void reserveTargetPath(const QList<DIRECTION>& path) {
    QPoint from = this->snake().head();
    for (auto& dir: path) {
      from += deltas[dir];
      chr_at(from) = 'o';
    }
    if (from == food_pos())
      chr_at(from) = 'S';
  }
  void clearTargetPath(const QList<DIRECTION>& path) {
    QPoint from = this->snake().head();
    for (auto& dir: path) {
      from += deltas[dir];
      chr_at(from) = ' ';
    }
    if (from == food_pos())
      chr_at(from) = 'S';
  }
};
