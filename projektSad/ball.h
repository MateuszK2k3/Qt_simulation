#ifndef BALL_H
#define BALL_H

class Ball
{
public:
    Ball(int x, int y, int dx, int dy, int foodCounter);

    int x;
    int y;
    int dx;
    int dy;
    int foodCounter;
};

#endif // BALL_H
