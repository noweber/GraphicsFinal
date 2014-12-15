#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <vector>
#include <cstddef>  // For NULL identifier     // Added 12/14/2014 @ 3:57 PM
#include "GameLane.h"       // Added 12/14/2014 @ 3:43 PM
//#include "Player.h"   // Added 12/13/2014 @ 8:32 PM -- DO NOT USE... causes circular inclusions.
#include<stdlib.h>      // srand(), rand(), and more
#include <time.h>

class Level {

public:
    // Level Methods
    Level(int laneWidth, int numberOfLanes);
    void update(float dt);
    bool checkCollisions();
    void clearLane(GameLane *cLane);        // Arguments: The lane to clear, the lane's number
    void regenerateLane(GameLane *cLane);   // Arguments: The lane to clear, the lane's number

    // Store the number of lanes that the player has passed
    int lanesPassed;

    // The velocity of the lanes... determines how fast they are swapped
    float laneVelocity;
    // The offset of the lanes as they approach the player
    float zOffset;


    // The index of the front most lane to the player
    //int frontLane;

    // Number of textures available for lane creation
    //int cTextureCt;

    // Sizes of the level
    int lWidth;         // This is how many paths will be in each lane
        float xWidth;   // The width of the "ground plane" -- This is relative to lWidth
    int nLanes;
        float zWidth;   // The depth of the "ground plane" -- This is relative to nLanes

    int defaultStartX;  // this should be slightly in front of where we want the first lane to generate
    int defaultStartZ;

    // Store the lanes within the level
    std::vector<GameLane> lanes;


private:
protected:

};

#endif // LEVEL_H_INCLUDED
