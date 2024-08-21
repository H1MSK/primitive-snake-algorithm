#include <Windows.h>
#include <QDateTime>
#include <QGuiApplication>
#include <QImage>
#include <QList>
#include <QPixmap>
#include <QScreen>
#include <QtLogging>
#include "common.hpp"
#include "map.hpp"

QGuiApplication* app;

void sendDirectionKey(DIRECTION direction) {
  WORD key = ' ';
  switch (direction) {
    case UP:
      key = 'W';
      break;
    case LEFT:
      key = 'A';
      break;
    case RIGHT:
      key = 'D';
      break;
    case DOWN:
      key = 'S';
      break;
    default:
      Q_ASSERT(false);
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

void direct(QList<DIRECTION>& path) {
  if (path.size() < 1) {
    qCritical("Direction lost! Searching for largest area...");
  }
  sendDirectionKey(path.front());
  path.pop_front();
}

int main(int argc, char* argv[]) {
  PrintableMap<MAP_W, MAP_H> map = PrintableMap<MAP_W, MAP_H>::initialMap();
  QList<DIRECTION> path;
  app = new QGuiApplication(argc, argv);

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
    if (updateMap(map)) {
      // ++cnt_op;
      if (path.empty() || ++pass == 4) {
        path = findPath(map);
        pass = 0;
      }
      direct(path);
      // auto now = QDateTime::currentMSecsSinceEpoch();
      // float unit = 1000.0f / (now - start_time);
      // qInfo("DIR=%c FPS(CV)=%6.3f FPC(OP)=%6.3f\r", " ULRD"[direction],
      // cnt_cv * unit, cnt_op * unit);
    }
  }

  return app->exec();
}
