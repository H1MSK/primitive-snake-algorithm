#include <QList>
#include <QPoint>
#include <array>
#include <functional>
#include "common.hpp"

struct State {
  int penalty;
  int step;
  QPoint pos;
  bool reverse;
  State() = default;
  constexpr State(const State&) = default;
  constexpr State(State&&) = default;
  constexpr State& operator=(const State &) = default;
  constexpr State(int distance,
                  int step,
                  QPoint pos,
                  bool reverse,
                  int direction_penalty)
      : penalty((step + distance) * 4 + direction_penalty),
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

static void prependToPath(QPoint pos) {
  path.push_front(pos);
  map_str[pos.y() + 1][pos.x() + 1] = '%';
}

static void appendToPath(QPoint pos) {
  path.push_back(pos);
  map_str[pos.y() + 1][pos.x() + 1] = '%';
}

static void clearPath() {
  while (path.length()) {
    QPoint& pos = path.front();
    map_str[pos.y() + 1][pos.x() + 1] = CH_LUT[map[pos.y()][pos.x()]];
    path.pop_front();
  }
}

static void fixHeadAndFood() {
  map_str[food_pos.y() + 1][food_pos.x() + 1] = 'O';
  map_str[head_pos.y() + 1][head_pos.x() + 1] = 'S';
}

static void generatePath(State s) {
  QPoint pos = s.pos;
  appendToPath(pos);
  constexpr static QPoint rev_dir[] = {
      QPoint(0, 0),   // UNKNOWN
      QPoint(0, -1),  // UP
      QPoint(-1, 0),  // LEFT
      QPoint(1, 0),   // RIGHT
      QPoint(0, 1)    // DOWN
  };
  while (pos != head_pos) {
    auto& ms = map_state[pos.y()][pos.x()];
    Q_ASSERT(ms.fw_from >= UP && ms.fw_from <= DOWN);
    pos += rev_dir[ms.fw_from];
    prependToPath(pos);
  }
  pos = s.pos;
  while (pos != food_pos) {
    auto& ms = map_state[pos.y()][pos.x()];
    Q_ASSERT(ms.bw_from >= UP && ms.bw_from <= DOWN);
    pos += rev_dir[ms.bw_from];
    appendToPath(pos);
  }
}

void printPath() {
  for (int r = 0; r <= MAP_H + 1; ++r)
    qInfo("%s", map_str[r]);
}

void findPath() {
  int heap_size = 2;
  int initial_dis = (head_pos - food_pos).manhattanLength();
  bool found = false;
  int search_count = 0;
  if (initial_dis == 1) {
    clearPath();
    appendToPath(head_pos);
    appendToPath(food_pos);
    found = true;
    search_count = 0;
  } else {
    heap_mem[0] = State{initial_dis, 0, head_pos, false, 0};
    heap_mem[1] = State{initial_dis, 0, food_pos, true, 0};
    std::make_heap(heap_mem, heap_mem + heap_size);
    memset(map_state, 0, sizeof(map_state));
    // map_state[head_pos.y()][head_pos.x()].fw_from = direction;
    // map_state[food_pos.y()][food_pos.x()].bw_from = direction;
    State s;
    constexpr static uint8_t penalty_map[5][5] = {
        //  U  L  R  D = next
        {0, 0, 0, 0, 0},  // cur=UNKNOWN
        {0, 0, 1, 1, 2},  // cur=UP
        {0, 1, 0, 2, 1},  // cur=LEFT
        {0, 1, 2, 0, 1},  // cur=RIGHT
        {0, 2, 1, 1, 0},  // cur=DOWN
    };
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

      constexpr static struct {
        QPoint delta;
        DIRECTION from;
      } deltas[] = {{QPoint(-1, 0), RIGHT},
                    {QPoint(0, -1), DOWN},
                    {QPoint(1, 0), LEFT},
                    {QPoint(0, 1), UP}};
      DIRECTION& cur_from = (s.reverse ? ms.bw_from : ms.fw_from);
      // qInfo("%d, %d, %d, (%d, %d)", s.reverse, s.penalty, s.step, s.pos.x(), s.pos.y());
      constexpr static uint8_t iterate_start[] = {0, 3, 2, 0, 1};
      for (int i = 0; i < 4; ++i) {
        auto& delta = deltas[(iterate_start[cur_from] + i) & 3];
        QPoint new_p = s.pos + delta.delta;
        if (new_p.y() < 0 || new_p.y() >= MAP_H ||
            new_p.x() < 0 || new_p.x() >= MAP_W ||
            map[new_p.y()][new_p.x()] == TYPE_SNAKE)
          continue;
        auto& nms = map_state[new_p.y()][new_p.x()];
        DIRECTION& from = (s.reverse ? nms.bw_from : nms.fw_from);
        if (from != UNKNOWN)
          continue;
        from = delta.from;
        heap_mem[heap_size] =
            State{(new_p - (s.reverse ? head_pos : food_pos)).manhattanLength(),
                  s.step + 1,
                  new_p,
                  s.reverse,penalty_map[cur_from][from]};
        ++heap_size;
        std::push_heap(heap_mem, heap_mem + heap_size);
      }
    }
    clearPath();
    if (found)
      generatePath(s);
  }
  fixHeadAndFood();
  qInfo("Operations: %d, path len: %d", search_count, found ? path.size() : -1);
  printPath();
}
