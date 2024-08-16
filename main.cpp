#include <Windows.h>
#include <QGuiApplication>
#include <QImage>
#include <QPixmap>
#include <QScreen>
#include <QtLogging>
#include <QList>
#include <QDateTime>
#include "common.hpp"

DIRECTION direction = RIGHT;
struct {
  int x, y, w, h;
} region = {0, 0, 800, 600};

constexpr int TILE_W = 16;
constexpr int TILE_H = 16;

constexpr QRgb BORDER_COLOR = qRgb(0, 255, 0);
constexpr QRgb BODY_COLOR = qRgb(255, 0, 0);
constexpr QRgb HEAD_COLOR = qRgb(255, 255, 0);
constexpr QRgb FOOD_COLOR = qRgb(0, 0, 255);

QGuiApplication *app;
MAP_TYPE map[MAP_H][MAP_W];
char map_str[MAP_H + 2][MAP_W + 3];
QList<QPoint> path;

QPoint food_pos, head_pos;

void sendDirectionKey() {
  WORD key = ' ';
  switch (direction) {
    case UP:    key = 'W'; break;
    case LEFT:  key = 'A'; break;
    case RIGHT: key = 'D'; break;
    case DOWN:  key = 'S'; break;
    default: Q_ASSERT(false);
  }
  INPUT inputs[2] = {};
  inputs[0].type = INPUT_KEYBOARD;
  inputs[0].ki.wVk = key;
  inputs[1].type = INPUT_KEYBOARD;
  inputs[1].ki.wVk = key;
  inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
  UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
  Q_ASSERT(uSent == ARRAYSIZE(inputs));
}

void setDirection(DIRECTION new_dir) {
  if (new_dir == direction)
    return;
  Q_ASSERT(new_dir + direction != INVALID);
  direction = new_dir;
  sendDirectionKey();
}

QImage screenshot() {
  qreal dpi = app->primaryScreen()->devicePixelRatio();
  return app->primaryScreen()->grabWindow(0, region.x / dpi, region.y / dpi, region.w / dpi, region.h / dpi).toImage();
}

bool updateMap() {
  QImage img = screenshot();
  bool updated = false;
  QPoint dummy;

  // MAP_TYPE->QPoint*
  QPoint *pos_lut[] = {
    &dummy, &dummy, &food_pos,
    nullptr, nullptr, &head_pos
  };
  for (int y = 0; y < MAP_H; ++y) {
    for (int x = 0; x < MAP_W; ++x) {
          auto pix = ((QRgb*)img.constScanLine(y * TILE_H + TILE_H * 3 / 2))[x * TILE_W + TILE_W * 3 / 2];
      MAP_TYPE t;
      switch (pix) {
      case FOOD_COLOR:  t = TYPE_FOOD;  break;
      case BODY_COLOR:  t = TYPE_SNAKE; break;
      case HEAD_COLOR:  t = TYPE_HEAD;  break;
      default:          t = TYPE_EMPTY; break;
      }
      *pos_lut[t] = QPoint(x, y);
      t = (MAP_TYPE)(t & 0x03);
      updated |= map[y][x] != t;
      map[y][x] = t;
      map_str[y + 1][x + 1] = CH_LUT[t];
    }
  }
  return updated;
}

void directToLargestArea() {
  Q_ASSERT(false);
}

void followPath() {
  QPoint p0 = path.at(0), p1 = path.at(1);
  if (p0.x() != p1.x()) {
    setDirection(p1.x() < p0.x() ? LEFT : RIGHT);
    path.pop_front();
    map_str[p0.y() + 1][p0.x() + 1] = CH_LUT[map[p0.y()][p0.x()]];
  }
  if (p0.y() != p1.y()) {
    setDirection(p1.y() < p0.y() ? UP : DOWN);
    path.pop_front();
    map_str[p0.y() + 1][p0.x() + 1] = CH_LUT[map[p0.y()][p0.x()]];
  }
}

void direct() {
  if (path.size() < 2) {
    qWarning("Direction lost! Searching for largest area...");
    directToLargestArea();
  } else {
    followPath();
  }
}

bool findRegion() {
  QImage img = screenshot();
  for (int r = 0; r < img.height(); r += 4) {
    auto line = (QRgb*)img.constScanLine(r);
    auto next_line = (QRgb*)img.constScanLine(r + 1);
    for (int c = 0; c < img.width(); c += 4) {
      if (line[c] == BORDER_COLOR && next_line[c + 1] == BORDER_COLOR) {
        region.x = c;
        region.y = r;
        qInfo("New region: (%d, %d)-(%d, %d)",
              region.x, region.y, region.w, region.h);
        return true;
      }
    }
  }
  return false;
}

void storeBoard() {
  QImage img = screenshot();
  for (int y = 0; y < MAP_H; ++y) {
    for (int x = 0; x < MAP_W; ++x) {
      img.setPixel(x * TILE_W + TILE_W * 3 / 2, y * TILE_H + TILE_H * 3 / 2, qRgb(255, 255, 255));
    }
  }
  img.save("board.png");
}

void relocateRegion() {
  while (!findRegion()) {
    qWarning("Region not found!");
    Sleep(0);
  }
  storeBoard();
  qInfo("Region located at (%d, %d)-(%d, %d)", region.x, region.y, region.w,
        region.h);
}

int main(int argc, char* argv[]) {
  app = new QGuiApplication(argc, argv);

//  while (1) {
//    for (int i = 1; i <= 4; ++i) {
//      setDirection((DIRECTION)i);
//      qInfo("DIR: %c", " ULRD"[direction]);a
//      Sleep(1000);
//    }
//  }

  for (int r = 0; r < MAP_H + 2; r += MAP_H + 1) {
    map_str[r][0] = '+';
    for (int i = 1; i <= MAP_W; ++i)
      map_str[r][i] = '-';
    map_str[r][MAP_W + 1] = '+';
    map_str[r][MAP_W + 2] = 0;
  }
  for (int r = 1; r <= MAP_H; ++r) {
    map_str[r][0] = '|';
    for (int i = 1; i <= MAP_W; ++i)
      map_str[r][i] = ' ';
    map_str[r][MAP_W + 1] = '|';
    map_str[r][MAP_W + 2] = 0;
  }

  relocateRegion();
  auto window = GetForegroundWindow();

  // auto start_time = QDateTime::currentMSecsSinceEpoch();

  // uint64_t cnt_op = 0, cnt_cv = 0;
  int pass = 0;
  while (1) {
    while (GetForegroundWindow() != window) {
      qInfo("Window not focused.");
      Sleep(50);
    }
    // ++cnt_cv;
    if (updateMap()) {
      // ++cnt_op;
      if (path.size() < 2 || head_pos != path.front() || ++pass == 4){
        findPath();
        pass = 0;
      }
      direct();
      // auto now = QDateTime::currentMSecsSinceEpoch();
      // float unit = 1000.0f / (now - start_time);
      // qInfo("DIR=%c FPS(CV)=%6.3f FPC(OP)=%6.3f\r", " ULRD"[direction], cnt_cv * unit, cnt_op * unit);
    }
  }

  return app->exec();
}
