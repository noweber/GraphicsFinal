#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

//#include "Player.h" // Added 12/13/2014 @ 8:32 PM -- DO NOT USE... causes circular inclusions.

class Level {

public:

    Level(int laneWidth, int numberOfLanes);
    void update(float dt);
    bool checkCollisions();

    // Store the number of lanes that the player has passed
    int lanesPassed;

    // A pointer to the current player
    //Player *cPlayer;

    // Sizes of the level
    int lWidth;
        float xWidth;   // The width of the "ground plane" -- This is relative to lWidth
    int nLanes;
        float zWidth;   // The depth of the "ground plane" -- This is relative to nLanes

    int defaultStartX;  // this should be slightly in front of where we want the first lane to generate
    int defaultStartZ;

    //int *gameTiles;

    // All of the data for the lane arrays
    int *laneOne;
        float lane1PosX;
        float lane1PosZ;

    int *laneTwo;
        float lane2PosX;
        float lane2PosZ;
    int *laneThree;
        float lane3PosX;
        float lane3PosZ;
    int *laneFour;
        float lane4PosX;
        float lane4PosZ;



private:
protected:

};

#endif // LEVEL_H_INCLUDED
