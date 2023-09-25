#ifndef BADORGANISM_H
#define BADORGANISM_H
#include "organism.h"

class BadOrganism : public Organism
{
public:
    BadOrganism();
    void generatePosition(int width, int height);
};

#endif // BADORGANISM_H
