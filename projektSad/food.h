#ifndef FOOD_H
#define FOOD_H

#include <QPainter>

class Food
{
public:
    static const int Size = 25;

    Food(int x, int y);
    void draw(QPainter& painter) const;

private:
    int m_x;
    int m_y;
};

#endif // FOOD_H
