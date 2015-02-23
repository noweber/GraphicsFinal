#include "GameLane.h"
#include <stdlib.h> // For Malloc

GameLane::GameLane(int width) {
    this->nPaths = width;
    isPassed = false;

    // paths: Allocate space for the lane...
    this->paths = (int *) malloc((this->nPaths) * sizeof(int));
}
