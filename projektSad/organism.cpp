#include "organism.h"

Organism::Organism(){
    time = 0;
    direction = generateRandomDirection();
    foodCounter = 0;
    size = 50;
}

QPoint Organism::generateRandomDirection()
{
    int dx = rand() % 3 - 1; // Losowa wartość -1, 0 lub 1
    int dy = rand() % 3 - 1;
    return QPoint(dx, dy);
}

void Organism::paintOrganism(QPainter& painter)
{
    if (color == Qt::red) {
        painter.setBrush(Qt::red);
    } else {
        painter.setBrush(Qt::black);
    }
    painter.drawEllipse(QRect(position.x(), position.y(), size, size));
}

void Organism::directOrganism(int width, int height)
{
    // Losowa zmiana kierunku co 1% szansy
    if (rand() % 100 == 0) {
        direction = generateRandomDirection();
    }

    // Przesunięcie organizmu
    position += direction;

    // Sprawdzenie kolizji z lewą i prawą ścianą
    if (position.x() <= 0 || position.x() >= width - size) {
        // Wygenerowanie losowego kierunku dla odbicia
        direction.setX(-direction.x());
    }

    // Sprawdzenie kolizji z górną i dolną ścianą
    if (position.y() <= 0 || position.y() >= height - size) {
        // Wygenerowanie losowego kierunku dla odbicia
        direction.setY(-direction.y());
    }
}
