#ifndef PLAYER_H
#define PLAYER_H



class Player {
    public:
        Player();
        virtual ~Player();

    // Stores the position of the player within the game world
    float posX;
    float posY;
    float posZ;

    void movePlayer();
    void update();

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
