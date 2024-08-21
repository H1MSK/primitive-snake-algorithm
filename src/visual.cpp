#include <QImage>
#include <QThread>
#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>
#include "common.hpp"

struct {
  int x, y, w, h;
} region = {0, 0, 800, 600};

QImage screenshot() {
  qreal dpi = qApp->primaryScreen()->devicePixelRatio();
  return qApp->primaryScreen()
      ->grabWindow(0, region.x / dpi, region.y / dpi, region.w / dpi, region.h / dpi)
      .toImage();
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
        qInfo("New region: (%d, %d)-(%d, %d)", region.x, region.y, region.w,
              region.h);
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
      img.setPixel(x * TILE_W + TILE_W * 3 / 2, y * TILE_H + TILE_H * 3 / 2,
                   qRgb(255, 255, 255));
    }
  }
  img.save("board.png");
}

void relocateRegion() {
  while (!findRegion()) {
    qWarning("Region not found!");
    QThread::sleep(0);
  }
  storeBoard();
  qInfo("Region located at (%d, %d)-(%d, %d)", region.x, region.y, region.w,
        region.h);
}
