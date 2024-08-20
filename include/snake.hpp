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

 public:
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

    const QPoint& head() { return body.front(); }
    const QPoint& tail() { return body.back(); }
};
