#include "Level.h"

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

    // Create the lanes
    GameLane *tLane = NULL;
    for(int i = 0; i < nLanes; i++) {
        tLane = new GameLane(laneWidth);
        lanes.push_back(*tLane);
    }

    // Seed the random number generation
    //srand (time(NULL));
    // Randomly generate the content of the lanes
    for(int j = 0; j < nLanes; j++) {
        regenerateLane(&lanes[j]);
    }

}

///-- TODO:
void Level::regenerateLane(GameLane *cLane) {
    cLane->isPassed = false;

    // Generate a random object within each path slot
    int objType = 0;
    for(int i = 0; i < cLane->nPaths; i++) {
        objType = rand() % 4;   // Random number between 0 and 3
        // objType 0 will be an open path
        cLane->paths[i] = objType;
    }
    // Ensure there is at least one empty slot
    bool hasPath = false;

    for(int j = 0; j < cLane->nPaths; j++) {
        // objType 0 will be an open path
        if(cLane->paths[j] == 0) {
            // If we find an open path at least once, then is hasPath
            hasPath = true;
        }
    }
    if(!hasPath) {
        int pSlot;
        pSlot = rand() % cLane->nPaths;
        // Place a random open path
        cLane->paths[pSlot] = 0;
    }
    return;
}

///-- TODO:
void Level::clearLane(GameLane *cLane) {
    return;
}

///-- TODO:
bool Level::checkCollisions() {
    return false;
}

