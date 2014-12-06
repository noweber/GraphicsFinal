#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

class Camera {

public:
    Camera();
    void recenter();

    float posX;
    float posY;
    float posZ;

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
