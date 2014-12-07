#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

class Camera {

public:
    Camera();
    void update(float dt);

    float posX;
    float posY;
    float posZ;

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
