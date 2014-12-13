#include "Level.h"
#include <stdlib.h>

Level::Level(int x, int z) {
    this->xWidth = x;
    this->zWidth = z;

    this->defaultStartX = 0;
    this->defaultStartX = 0;

    this->gameTiles = (int *) malloc((this->xWidth * this->zWidth) * sizeof(int));
}
