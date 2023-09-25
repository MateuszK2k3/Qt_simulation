#include "badorganism.h"

BadOrganism::BadOrganism()
{
    size = 60;

}

void BadOrganism::generatePosition(int width, int height)
{
    //losowanie pozycji
    int poz_x = rand() % (width - size);
    int poz_y = rand() % (height - size);
    while(poz_y > height/4 &&  poz_x > width/4 && poz_x < width - (width/4) && poz_y < height - (height/4)){
        poz_x = rand() % (width - size);
    }
    position = QPoint(poz_x, poz_y);
}
