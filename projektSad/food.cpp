#include "food.h"

Food::Food(int x, int y) : m_x(x), m_y(y) {}

void Food::draw(QPainter& painter) const
{
    painter.setBrush(Qt::green);
    painter.drawEllipse(m_x, m_y, Size, Size);
}
