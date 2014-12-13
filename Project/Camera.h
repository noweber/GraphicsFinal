#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "Player.h" // Added 12/12/2014 @ 11:14 PM
#include <iostream> // Added 12/12/2014 @ 11:17 PM

class Camera {

public:
    Camera();
    void update(float dt);
        void adjustForFollowCam();  // A helper function for update
    bool setPlayer(Player *cP);
    void changeMode();


    // Camera's current mode
    // 0: Follow Cam
    // 1: Free Cam
    int cMode;

    // Pointer to current player
    Player *cPlayer;

    float posX;
    float posY;
    float posZ;

    float followDistance;
    float followHeight;

    // Camera's responsiveness WASD
    float velocityX;
    float velocityY;
    float velocityZ;

    // Camera's movement states since last update (these reset to false every update
    bool hasMoved;
    bool movedLeft;
    bool movedRight;
    bool movedUp;
    bool movedDown;
    bool movedForward;
    bool movedBackward;

    float horiAngle = 3.14f;
    float vertAngle = 0.0f;
    float speed = 0.01f;
    float mouseSpeed = 0.005f;
    int mouseX, mouseY;
    float lastMouseX, lastMouseY, mouseDX, mouseDY;
    bool shouldRecenter;

private:
protected:
};

#endif // CAMERA_H_INCLUDED
