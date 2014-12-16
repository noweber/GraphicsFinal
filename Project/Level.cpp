#include "Level.h"

Level::Level(int laneWidth, int numberOfLanes) {
    ///--TODO: Ensure that the number of lanes created is a multiple of 4 within Level::Level(int laneWidth, int numberOfLanes)...
    // Set the dimensions of the Level
    this->lWidth = laneWidth;
        this->xWidth = 2 * laneWidth;
    this->nLanes = numberOfLanes;
        this->zWidth = 2 * numberOfLanes;

    // Set default values for other members
    frontLane = 0;
    zVelocity = 0.004f;

    // Set the default start position for a player on this level
    this->defaultStartX = 0;
    this->defaultStartX = 0;

    // Number of textures available for lane creation
    //cTextureCt = 0;;

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

    numberOfLanesDrawn = 10;
    laneDZ = 0.0f;
    dzSinceSwap = 0.0f;
    zOffset = 0.0f;
    mustCheckCollisions = false;
    laneSpacing = 10;
    xDrawingScale = 2.56f;

    hasFailed = false;
    hitPoints = 2;

    playerPosXOnLevel = 0;
    passedLaneOne = false;

    powerUpY = 0.0f;
    moveCt = 0;
    pUpsGoUp = false;
}

void Level::movePowerUps() {
    if(!pUpsGoUp) {
        pUpsGoUp = true;
        moveCt = 60;
    }
}

void Level::update(float dt) {
    movePowerUps();

    laneDZ = dt * zVelocity;
    dzSinceSwap += laneDZ;
    zOffset += laneDZ;

    // Check if the front lane needs to be swapped and a collision check needs to occur
    if(dzSinceSwap > laneSpacing) {
        if(passedLaneOne) {
            frontLane += 1;
            mustCheckCollisions = true;
        } else {
            passedLaneOne = true;
        }

        if(frontLane >= nLanes) {
            hasFailed = true;   /// TODO-- make them win
        }
        dzSinceSwap -= laneSpacing;
        // Increase the lane velocity
        zVelocity += 0.00005;
    }

    // Check for collisions
    if(mustCheckCollisions) {
        bool didCollide = true;
        //bool didCollideF = true;
        // Trying to use integer
        //for(int i = 0; i < lanes[frontLane].nPaths; i++) {
        if(playerPosXOnLevel < 0 || playerPosXOnLevel >= lanes[frontLane].nPaths) {
            //hasFailed = true;
        } else {
            if(lanes[frontLane].paths[playerPosXOnLevel] == 0 || lanes[frontLane].paths[playerPosXOnLevel] == 5) {
                didCollide = false;
            }
        }

        // Using floats...
        int collisionForgiveness = 0.0f;
        int check1 = ceil(playerFloatX - collisionForgiveness);
        int check2 = floor(playerFloatX - collisionForgiveness);
        //std::cout << "Check 1: " << check2 << "\n";
        //std::cout << "Check 2: " << check2 << "\n";

        if(check1 >= 0 && check1 <= lWidth) {
            if(lanes[frontLane].paths[check1] == 0 || lanes[frontLane].paths[check1] == 5) {
                //std::cout << "Didn't collide...\n";
                didCollide = false;
            }
        }

        if(check2 >= 0 && check2 <= lWidth) {
            if(lanes[frontLane].paths[check2] == 0 || lanes[frontLane].paths[check2] == 5) {
                //std::cout << "Didn't collide...\n";
                didCollide = false;
            }
        }


        if(didCollide) {
                std::cout << "Player Position On Level: " << playerPosXOnLevel << "\n";
                std::cout << "Object At Path Position: " << lanes[frontLane].paths[playerPosXOnLevel] << "\n";
                std::cout << "Current Lane: ";
                for(int j = 0; j < lanes[frontLane].nPaths; j++) {
                    std::cout << lanes[frontLane].paths[j];
                }
                std::cout << "\n";
                hitPoints -= 1;
                if(hitPoints <= 0) {
                    hasFailed = true;
                }
        }

        mustCheckCollisions = false;
    }

    // Animate the power ups
    if(this->moveCt > 0) {

        if(this->moveCt > 30) {
            this->powerUpY += 0.01;
        } else {
            this->powerUpY -= 0.01;
        }
        this->moveCt -= 1;
        //std::cout << "P Height: " << powerUpY << "\n";;

    } else if (this->moveCt <= 0) {
        /// Reset all values
        this->pUpsGoUp = false;
        this->powerUpY = 0;
        this->moveCt = 0;
    }

}

///-- TODO:
void Level::regenerateLane(GameLane *cLane) {
    cLane->isPassed = false;

    // Generate a random object within each path slot
    int objType = 0;
    int powerChance = 0;
    for(int i = 0; i < cLane->nPaths; i++) {
        objType = rand() % 6;   // Random number between 0 and 5
        if(objType == 5) {
            // Don't keep too many power ups
            powerChance = rand() % 100;
            if(powerChance > 15) {
                objType = rand() % 5;
            }
        }
        // objType 0 will be an open path
        // objType 4 is a power up
        cLane->paths[i] = objType;
    }
    // Assign a random texture number to it.
    /*int texCt = this->cTextureCt;
    if(texCt <= 0) {
        texCt = 1;
    }
    int tTexID;// = rand() % texCt;
    for(int k = 0; k < cLane->nPaths; k++) {
        tTexID = rand() % texCt;
        cLane->pathTxt[k] = tTexID;
    }*/

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

