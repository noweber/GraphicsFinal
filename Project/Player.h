#ifndef PLAYER_H
#define PLAYER_H

#include "Level.h"  // Added 12/12/2014 @ 10:31 PM
#include <iostream> // Added 12/12/2014 @ 10:34 PM

class Player {
    public:
        Player();
        virtual ~Player();

    void update(float dt);
    bool changeLevel(Level *cL);

    // Can move is called to check whether the player's next move is valid within its current level, cLevel.
    bool canMove();
    //void jump();

    // Stores the position of the player within the game world
    float posX;
    float posY;
    float posZ;

    // Changes in position since last update.  These are used for translated the model by the appropriate amount
    float posDX;
    float posDZ;

    // Stores the position change of the player since the last update
    //float posDX;
    //float posDY;
    //float posDZ;

    // Player's responsiveness WASD
    float velocityX;
    float velocityY;
    float velocityZ;


    // Player's movement states since last update (these reset to false every update
    bool hasMoved;
    bool movedLeft;
    bool movedRight;
    bool movedUp;
    bool movedDown;
    bool movedForward;
    bool movedBackward;

    void movePlayer();


    float shellOffsetY;
    float shellOffsetZ;


    float headOffsetX;
    float headOffsetY;

    float leftFootOffsetY;
    float rightFootOffsetY;

    bool isJumping;
    float jumpOffset;

    bool isMoving;
    int moveCt;

    protected:

    private:
    Level *cLevel;



};

#endif // PLAYER_H
