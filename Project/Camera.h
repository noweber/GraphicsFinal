#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "Player.h" // Added 12/12/2014 @ 11:14 PM
#include <iostream> // Added 12/12/2014 @ 11:17 PM

class Camera {

public:
    // Constructor
    Camera();

    // Methods
    void update(float dt);
    void adjustForFollowCam();
    bool setPlayer(Player *cP);
    void changeMode();


    // Camera's current mode
    // 0: Follow Cam
    // 1: Free Cam
    int cMode;

    // Pointer to current player
    Player *cPlayer;

    // Current Positions
    ///-- To-Do: Use a 3D Vector to store this information.
    float posX;
    float posY;
    float posZ;

    // Camera Distance/Height
    float followDistance;
    float followHeight;

    // Camera's Responsiveness (WASD)
    ///-- To-Do: Use a 3D Vector to store this information.
    float velocityX;
    float velocityY;
    float velocityZ;

    // Camera's movement States (these reset to false every update)
    bool hasMoved;
    bool movedLeft;
    bool movedRight;
    bool movedUp;
    bool movedDown;
    bool movedForward;
    bool movedBackward;

    // Camera Angle Towards Current Player
    float horiAngle = 3.14f;
    float vertAngle = 0.0f;
    float speed = 0.01f;

    // Mouse Data
    float mouseSpeed = 0.005f;
    int mouseX, mouseY;
    float lastMouseX, lastMouseY, mouseDX, mouseDY;
    bool shouldRecenter;

private:
protected:
};

#endif // CAMERA_H_INCLUDED
