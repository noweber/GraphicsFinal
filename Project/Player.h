#ifndef PLAYER_H
#define PLAYER_H



class Player {
    public:
        Player();
        virtual ~Player();

    void update(float dt);

    // Stores the position of the player within the game world
    float posX;
    float posY;
    float posZ;

    // Stores the position change of the player since the last update
    float posDX;
    float posDY;
    float posDZ;

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

    protected:

    private:
    bool isMoving;
    int moveCt;



};

#endif // PLAYER_H
