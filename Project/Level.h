#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

class Level {

public:

    Level(int x, int z);

    int xWidth;
    int zWidth;

    int defaultStartX;
    int defaultStartZ;

    int *gameTiles;



private:
protected:

};

#endif // LEVEL_H_INCLUDED
