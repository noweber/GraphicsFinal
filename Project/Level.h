#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

//#include "Player.h" // Added 12/13/2014 @ 8:32 PM -- DO NOT USE... causes circular inclusions.

class Level {

public:
    // Level Methods
    Level(int laneWidth, int numberOfLanes);
    void update(float dt);
    bool checkCollisions();
    void clearLane(int *cLane, int lNumber);        // Arguments: The lane to clear, the lane's number
    void regenerateLane(int *cLane, int lNumber);   // Arguments: The lane to clear, the lane's number


    // Store the number of lanes that the player has passed
    int lanesPassed;

    // The velocity of the lanes... determines how fast they are swapped
    float laneVelocity;

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
        bool isClearOne;
        float lane1PosX;
        float lane1PosZ;

    int *laneTwo;
        bool isClearTwo;
        float lane2PosX;
        float lane2PosZ;

    int *laneThree;
        bool isClearThree;
        float lane3PosX;
        float lane3PosZ;

    int *laneFour;
        bool isClearFour;
        float lane4PosX;
        float lane4PosZ;



private:
protected:

};

#endif // LEVEL_H_INCLUDED
