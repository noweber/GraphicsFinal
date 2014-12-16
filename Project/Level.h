#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <vector>
#include <cstddef>  // For NULL identifier     // Added 12/14/2014 @ 3:57 PM
#include "GameLane.h"       // Added 12/14/2014 @ 3:43 PM
//#include "Player.h"   // Added 12/13/2014 @ 8:32 PM -- DO NOT USE... causes circular inclusions.
#include<stdlib.h>      // srand(), rand(), and more
#include <time.h>
#include <iostream> // Added 12/15/2014 @ 7:38 PM
#include <math.h>   // Added 12/15/2014 @ 8:11 PM

class Level {

public:
    // Level Methods
    Level(int laneWidth, int numberOfLanes);
    void update(float dt);
    bool checkCollisions();
    void clearLane(GameLane *cLane);        // Arguments: The lane to clear, the lane's number
    void regenerateLane(GameLane *cLane);   // Arguments: The lane to clear, the lane's number
    void movePowerUps();

    // Store the number of lanes that the player has passed
    int frontLane;

    // The amount of time passed since the last collision check.  Each time the
    //float checkTimer;

    // The velocity of the lanes... determines how fast they are swapped
    float zVelocity;

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

    int numberOfLanesDrawn;
    // The offset for the lanes towards the player
    float laneDZ;       // offset since last update
    float dzSinceSwap;  // offset since last front lane change
    float zOffset;

    // Whether or not the player needs to check for collisions
    bool mustCheckCollisions;

    // The factor affecting distance between lanes
    int laneSpacing;

    // The scale that the level is scaled along the x-axis when drawn
    float xDrawingScale;

    // Has the level been failed by the player?
    bool hasFailed;
    // The number of collisions remaining before failure
    int hitPoints;

    int playerPosXOnLevel;
    float playerFloatX;

    // Used to account for FrontLane starting at 0 instead of -1 within Level::update()
    bool passedLaneOne;

    // Height of floating power ups
    float powerUpY;
    int moveCt;
    bool pUpsGoUp;

private:
protected:

};

#endif // LEVEL_H_INCLUDED
