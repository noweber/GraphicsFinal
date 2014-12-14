#include "Level.h"
#include <stdlib.h> // For Malloc

Level::Level(int laneWidth, int numberOfLanes) {
    ///--TODO: Ensure that the number of lanes created is a multiple of 4 within Level::Level(int laneWidth, int numberOfLanes)...

    // Set the dimensions of the Level
    this->lWidth = laneWidth;
        this->xWidth = 2 * laneWidth;
    this->nLanes = numberOfLanes;
        this->zWidth = 2 * numberOfLanes;

    // Set default values for other members
    lanesPassed = 0;
    laneVelocity = 0.1f;

    // Set the default start position for a player on this level
    this->defaultStartX = 0;
    this->defaultStartX = 0;

    //this->gameTiles = (int *) malloc((this->lWidth * this->nLanes) * sizeof(int));

    // LANE ONE: Allocate space for the lane and set it's position.  Then generate its content.
    this->laneOne = (int *) malloc((this->lWidth) * sizeof(int));
        isClearOne = false;
        // Clear the lane

        // Regenerate the lane
}

///-- TODO:
void Level::regenerateLane(int *cLane, int lNumber) {
    return;
}

///-- TODO:
void Level::clearLane(int *cLane, int lNumber) {
    return;
}

///-- TODO:
bool Level::checkCollisions() {
    return false;
}

