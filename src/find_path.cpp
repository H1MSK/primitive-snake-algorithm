#include <QList>
#include <QPoint>
#include <array>
#include <concepts>
#include "common.hpp"
#include "map.hpp"

struct State {
  int penalty;
  int step;
  QPoint pos;
  bool reverse;
  State() = default;
  constexpr State(const State&) = default;
  constexpr State(State&&) = default;
  constexpr State& operator=(const State&) = default;
  constexpr State(int distance,
                  int step,
                  QPoint pos,
                  bool reverse,
                  int position_penalty)
      : penalty((step + distance) * 4 + position_penalty),
        step(step),
        pos(pos),
        reverse(reverse) {}
  bool operator<(const State& other) const {
    if (penalty != other.penalty)
      return penalty > other.penalty;
    return step > other.step;
  }
};

State heap_mem[MAP_W * MAP_H + 5];
struct MapState {
  DIRECTION fw_from;
  DIRECTION bw_from;
} map_state[MAP_H][MAP_W];

static QList<DIRECTION> generatePath(State s,
                                     const QPoint& from,
                                     const QPoint& to) {
  QList<DIRECTION> path;
  QPoint pos = s.pos;
  constexpr static QPoint rev_dir[] = {
      QPoint(0, 0),   // UNKNOWN
      QPoint(0, -1),  // UP
      QPoint(-1, 0),  // LEFT
      QPoint(1, 0),   // RIGHT
      QPoint(0, 1)    // DOWN
  };
  while (pos != from) {
    auto& ms = map_state[pos.y()][pos.x()];
    Q_ASSERT(ms.fw_from >= UP && ms.fw_from <= DOWN);
    pos += rev_dir[ms.fw_from];
    path.push_front(DIRECTION(5 - ms.fw_from));
  }
  pos = s.pos;
  while (pos != to) {
    auto& ms = map_state[pos.y()][pos.x()];
    Q_ASSERT(ms.bw_from >= UP && ms.bw_from <= DOWN);
    pos += rev_dir[ms.bw_from];
    path.push_back(ms.bw_from);
  }
  return path;
}

int search_count;
template <std::invocable<const QPoint&> GridPenaltyFunc>
static QList<DIRECTION> findPath(const PrintableMap<MAP_W, MAP_H>& map,
                                 const QPoint& from,
                                 const QPoint& to,
                                 GridPenaltyFunc penalty) {
  constexpr static struct {
    QPoint delta;
    DIRECTION from;
  } deltas[] = {{QPoint(-1, 0), RIGHT},
                {QPoint(0, -1), DOWN},
                {QPoint(1, 0), LEFT},
                {QPoint(0, 1), UP}};

  int heap_size = 2;
  int initial_dis = (from - to).manhattanLength();
  bool found = false;
  if (initial_dis == 1) {
    QPoint d = to - from;
    search_count = 1;
    for (auto p : deltas) {
      if (d == p.delta)
        return {p.from};
    }
    Q_ASSERT(false);
    return {};
  } else {
    heap_mem[0] = State(initial_dis, 0, from, false, 0);
    heap_mem[1] = State(initial_dis, 0, to, true, 0);
    std::make_heap(heap_mem, heap_mem + heap_size);
    memset(map_state, 0, sizeof(map_state));
    // map_state[from.y()][from.x()].fw_from = direction;
    // map_state[to.y()][to.x()].bw_from = direction;
    State s;
    while (heap_size != 0) {
      ++search_count;
      std::pop_heap(heap_mem, heap_mem + heap_size);
      s = heap_mem[heap_size - 1];
      --heap_size;

      auto& ms = map_state[s.pos.y()][s.pos.x()];
      if (ms.fw_from != UNKNOWN && ms.bw_from != UNKNOWN) {
        found = true;
        break;
      }
      DIRECTION& cur_from = (s.reverse ? ms.bw_from : ms.fw_from);
      // qInfo("%d, %d, %d, (%d, %d)", s.reverse, s.penalty, s.step, s.pos.x(),
      // s.pos.y());
      for (int i = 0; i < 4; ++i) {
        auto& delta = deltas[i];
        QPoint new_p = s.pos + delta.delta;
        if (new_p.y() < 0 || new_p.y() >= MAP_H || new_p.x() < 0 ||
            new_p.x() >= MAP_W || map.at(new_p) != 0)
          continue;
        auto& nms = map_state[new_p.y()][new_p.x()];
        DIRECTION& pos_from = (s.reverse ? nms.bw_from : nms.fw_from);
        if (pos_from != UNKNOWN)
          continue;
        pos_from = delta.from;
        heap_mem[heap_size] =
            State((new_p - (s.reverse ? from : to)).manhattanLength(),
                  s.step + 1, new_p, s.reverse, penalty(new_p));
        ++heap_size;
        std::push_heap(heap_mem, heap_mem + heap_size);
      }
    }
    if (found)
      return generatePath(s, from, to);
    else
      return {};
  }
}

QList<DIRECTION> findPath(PrintableMap<MAP_W, MAP_H>& map) {
  search_count = 0;
  QList<DIRECTION> rescue_path =
      findPath(map, map.food_pos(), map.snake().tail(), [&](const QPoint& p) {
        return -(map.snake().head() - p).manhattanLength();
      });
  if (rescue_path.empty())
    qWarning() << "No rescue path found!";
  map.reserveRescuePath(rescue_path);
  QList<DIRECTION> target_path =
      findPath(map, map.snake().head(), map.food_pos(), [&](const QPoint& p) {
        return -(map.snake().tail() - p).manhattanLength();
      });
  if (target_path.empty()) {
    qInfo() << "Unable to safely direct. Try chase tail.";
    target_path = findPath(map, map.snake().head(), map.snake().tail(),
                           [&](const QPoint& p) {
                             return (map.food_pos() - p).manhattanLength();
                           });
  }

  qInfo("Operations: %d, path len: %d", search_count, target_path.size());
  map.reserveTargetPath(target_path);
  map.print_map();
  map.clearTargetPath(target_path);
  map.clearRescuePath(rescue_path);
  return target_path;
}
