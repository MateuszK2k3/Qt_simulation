#ifndef ORGANISM_H
#define ORGANISM_H

#include <QPoint>
#include <QColor>
#include <QList>
#include <QPaintEvent>
#include <QPainter>

class Organism
{
public:
    QPoint position;
    QPoint direction;
    int foodCounter;
    int time;
    int size;
    QColor color;

    Organism();
    QPoint generateRandomDirection();
    void paintOrganism(QPainter& painter);
    void directOrganism(int width, int height);

private:
};

#endif // ORGANISM_H
