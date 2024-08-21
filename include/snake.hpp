#pragma once

#include <list>
#include <QPoint>
#include <QtLogging>
#include <QGuiApplication>
#include <concepts>
#include "common.hpp"

class Snake {
    using item_type = QPoint;
    using container_type = std::list<QPoint>;

    container_type body;

    Snake(int map_w, int map_h) : body({QPoint(map_w / 2, map_h / 2 + 1)}) {}

 public:
    Snake(const Snake& o) = default;
    Snake(Snake&& o) = default;

    static Snake initialSnake(int map_w, int map_h) {
        Snake s(map_w, map_h);
        s.move(RIGHT, true);
        return s;
    }

    size_t length() const { return body.size(); }

    void move(DIRECTION dir, bool keep_tail) {
        QPoint p = head();
        if (!keep_tail)
            body.pop_back();
        switch (dir) {
            case UP:    p.setY(p.y() - 1); break;
            case DOWN:  p.setY(p.y() + 1); break;
            case LEFT:  p.setX(p.x() - 1); break;
            case RIGHT: p.setX(p.x() + 1); break;
            default:
                qCritical() << "Invalid direction" << dir;
                qApp->exit(1);
        }
        body.push_front(p);
    }
    container_type::iterator begin() { return body.begin(); }
    container_type::iterator end() { return body.end(); }

    const QPoint& head() const { return body.front(); }
    const QPoint& tail() const { return body.back(); }
};
