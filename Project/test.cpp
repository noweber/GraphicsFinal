#include <GL/glew.h>   //Include order can matter here
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <stdlib.h>      // srand(), rand(), and more
#include <time.h>
#include <map>
#include <vector>
#include <cstddef>  // For NULL identifier


#include "Player.h"
#include "Camera.h"
#include "Level.h"
#include "GameLane.h"
#include "modelIndex.h"



using namespace std;


/// //// //// TO-DO LIST  //// //// ///
///-- TODO: adjust drawLanes() to only draw 4 lanes...
///-- TODO: create helper functions for drawing an instance of each model at a certain location... example: drawRockAt(float x, float y, int texID)
                        // These can be used within the drawLanes() for loop
///-- TODO: ensure that the game objects move the same speed in full screen vs windowed mode
///-- TODO: Work on a UI.  Draw cubes or quads relative to the camera and setup A-Z 0-9 Textures to be able to right different things to it.  Such as number of lanes cleared or points.
///-- TODO: Make palm leaf model to draw at the top of trees.  Trees can will probably have a sphere or cube base with 4-5 leaves.
///-- TODO: Player::canMove()... make this function check for valid moves once Level class is fully implemented.
///-- Finished: Make Camera::adjustForFollowCam() somehow also orientate the camera's view direction to be in line with the player...
                // Created a conditional that fixes the camera horizontal and vertical angles on camera mode 0
///-- Finished: Camera::setPlayer(Player *cP... after cP is assigned as cPlayer, change the position of the camera to right behind the player...
                // Created a helper function for update called adjustForFollowCam() which reset the camera position.  Called this in setPlayer().
///-- TODO: Set correct starting positions for camera and player... should these be contained within each Level?
///-- Finished: Allow some objects to have cel shaded outline and other to not...
                // Used integer within the fragment shader to allow a switch between outlines or no outlines.
///-- TODO: show the FPS
///-- Generate content
///-- TODO: have the texture allocation store a global number of total textures to use here -- use it within Level::regenerateLane() to build with random textures...
///-- TODO: Ensure that the number of lanes created is a multiple of 4 within Level::Level(int laneWidth, int numberOfLanes)...
///-- Get a new turtle model made and animated.
///-- Allow the player's turtle to move
///-- TODO: Make cloud objects out of 3-4 spheres and float them by in the background
///-- Bound the camera above the ground plane during free cam
///-- TODO: Title screen
///-- TODO: Reset button
///-- TODO: Add motion blur to the turtle
///-- Finished: Setup the button 'C' to switch between player movement and camera movement.  This means, I can move my camera freely, then press C to move the turtle... back and forth.
///-- Try to add more lights and/or make the the ground light up rather than just that odd circle underneath the camera
///-- Make movements in free camera mode relative to the facing of the camera?  If you press forward while facing a direction, you should move that direction.  This may be hard.
///-- Make event queue loop allow multiple key presses per cycle... currently you can not move forward-left, forward-right, etc.
        // We can do this by storing the states of the keyboard.  Flag the 'holdingRight' state on key press to true... flag it as false on key up.
///-- Add sounds to the game
///-- Update functions should be moved to Game::update()
///-- Change Player::update() to a function of time exactly as Camera::update() is.
///-- Possibly move rendering functions to Game::render() - may not be necessary
///-- Create a texture manager if time permits

/// //// //// END OF TO-DO LIST //// //// ///

int gTextureCt = 4; // The total number of textures in the game... this should be set elsewhere, but currently is not...
bool saveOutput = false;
float timePast = 0.0f;
float oldTime = 0.0f; //= SDL_GetTicks()/1000.f;
bool DEBUG_ON = true;
bool fullscreen = false;
void Win2PPM(int width, int height);


//srand(time(NULL));
float rand01(){
	return rand()/(float)RAND_MAX;
}
/// Initialization Function Declarations
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
/// Update Function Declarations
void updateLighting(int shaderProgram);
/// Rendering Function Declarations
void drawGround(int shaderProgram, int numVerts1, int numVerts2);

void drawLevel(int shaderProgram, int numVerts1, int numVerts2);
void drawLowLeftWall(int shaderProgram, int numVerts1, int numVerts2);
void drawLeftWall(int shaderProgram, int numVerts1, int numVerts2);
void drawLowRightWall(int shaderProgram, int numVerts1, int numVerts2);
void drawRightWall(int shaderProgram, int numVerts1, int numVerts2);
void drawLanes(int shaderProgram, int numVerts1, int numVerts2);

void drawTurtle(int shaderProgram, int numVerts1, int numVerts2);
void drawTurtleHead(int shaderProgram, int numVerts1, int numVerts2);
void drawTurtleLeft(int shaderProgram, int numVerts1, int numVerts2);
void drawTurtleRight(int shaderProgram, int numVerts1, int numVerts2);
void drawTurtleTail(int shaderProgram, int numVerts1, int numVerts2);


void drawUI(int shaderProgram, int numVerts1, int numVerts2, int xCoord, int yCoord, float scale, int tID);
void drawNumber(int shaderProgram, int xCoord, int yCoord, float scale, int number);

//Model data management
//See modelIndex class for returning the values

//Map to lookup the indices for the model
map<string, modelIndex> modelDict;
vector<float> modelData;

//Getter and setter
void setModel(string fileName, string modelName);
modelIndex getModel(string modelName);

/// Global Game Stuff
Player *player;
Camera *camera;
Level *level;

int screenWidth = 1024;
int screenHeight = 768;
float screenWidthF = 1024.0f;
float screenHeightF = 768.0f;

// TODO: make time of day lighting and add in additional lights to the map
float gLightAdjustment = 0;    // Allows the lights to move around
//int gLightMovementCt = 1000;  // possible light progression as the day goes by.
bool gLightReachedRightMax = false;


// Timing variable
float deltaT;

glm::vec3 direction;
glm::vec3 rightVector;
glm::vec3 upVector;




int main(int argc, char *argv[]){
    // Seed the random number generation
    srand (time(0));

    player = NULL;
    camera = NULL;
    level = NULL;

    /// Instantiate game objects
    player = new Player();
    if(player == NULL) {
        std::cout << "Failed: Player creation\n";
        return -1;
    }
    camera = new Camera();
    if(camera == NULL) {
        std::cout << "Failed:: Camera creation\n";
        return -1;
    }

    level = new Level(7, 128);
    //level->cTextureCt = gTextureCt; // Set the number of textures.  This is how the level generator chooses them.
    if(level == NULL) {
        std::cout << "Failed:: Level creation\n";
        return -1;
    }

    /// Setup game object pointers
    // Place the player within the level
    if(!player->changeLevel(level)) {
        cout << "Failed: Player::changeLevel()...\n";
    }
    // Assign the camera to the player
    if(!camera->setPlayer(player)) {
        return -1;
    }

    SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)

    //Ask SDL to get a recent version of OpenGL (3 or greater)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);//

	//Create a window (offsetx, offsety, width, height, flags)
	SDL_Window* window = SDL_CreateWindow("CSci 5607 Final Project", 100, 100, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

	//Create a context to draw in
	SDL_GLContext context = SDL_GL_CreateContext(window);

	//GLEW loads new OpenGL functions
	glewExperimental = GL_TRUE; //Use the new way of testing which methods are supported
	glewInit();

	//Build a Vertex Array Object. This stores the VBO and attribute mappings in one object
	GLuint vao;
	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); //Bind the above created VAO to the current context

    //Load models with new helper function
    //setModel("QuadY.txt", "quad");    // about 40%
    setModel("cube.txt", "cube");
    //setModel("QuadY.txt", "quad");    // About 20%
    setModel("Rock42Verts.txt", "rock");
    setModel("QuadY.txt", "quad");
    //setModel("sphere.txt", "sphere");
    setModel("turtle.txt", "turtle");
    setModel("sphere.txt", "sphere");
    //setModel("QuadY.txt", "quad");
    //setModel("sphere.txt", "sphere");

	/// Allocate Texture 0 ///
	SDL_Surface* surface = SDL_LoadBMP("lightRed.bmp");
	if (surface==NULL) { //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex0;
    glGenTextures(1, &tex0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //How to filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface);
    /// End Allocate Texture


	/// Allocate Texture 1 ///
	SDL_Surface* surface1 = SDL_LoadBMP("lightGreen.bmp");
	if (surface1==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex1;
    glGenTextures(1, &tex1);
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //How to filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface1->w,surface1->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface1->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface1);
    /// End Allocate Texture


    /// Allocate Texture 2 ///
	SDL_Surface* surface2 = SDL_LoadBMP("lightBlue.bmp");
	if (surface2==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex2;
    glGenTextures(1, &tex2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex2);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //How to filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface2->w,surface2->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface2->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface2);
    /// End Allocate Texture

    /// Allocate Texture 2 ///
	SDL_Surface* surface3 = SDL_LoadBMP("lightbrown.bmp");
	if (surface3==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex3;
    glGenTextures(1, &tex3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, tex3);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //How to filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface3->w,surface3->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface3->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface3);
    /// End Allocate Texture

    /// Allocate Texture For turtle shell ///
    SDL_Surface* surface4 = SDL_LoadBMP("turtletexb.bmp");
    if (surface4==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex4;
    glGenTextures(1, &tex4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, tex4);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //How to filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface4->w,surface4->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface4->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface4);
    /// End Allocate Texture

    /// Allocate Texture For Text///
    SDL_Surface* surface5 = SDL_LoadBMP("textTest.bmp");
    if (surface5==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex5;
    glGenTextures(1, &tex5);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, tex5);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //How to filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface5->w,surface5->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface5->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface5);
    /// End Allocate Texture

    /// Allocate Texture For Beach///
    SDL_Surface* surface6 = SDL_LoadBMP("beach.bmp");
    if (surface6==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex6;
    glGenTextures(1, &tex6);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, tex6);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //How to filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface6->w,surface6->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface6->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface6);
    /// End Allocate Texture

    /// Allocate Texture For Water///
    SDL_Surface* surface7 = SDL_LoadBMP("water.bmp");
    if (surface7==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex7;
    glGenTextures(1, &tex7);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, tex7);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //How to filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface7->w,surface7->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface7->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface7);
    /// End Allocate Texture

	//Allocate memory on the graphics card to store geometry (vertex buffer object)
	GLuint vbo[1];
	glGenBuffers(1, vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
    glBufferData(GL_ARRAY_BUFFER, modelData.size()*sizeof(float), &modelData[0], GL_STATIC_DRAW); //upload vertices to vbo
	//GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
	//GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used

	int texturedShader = InitShader("vertexTex.glsl", "fragmentTex.glsl");

	//Tell OpenGL how to set fragment shader input
	GLint posAttrib = glGetAttribLocation(texturedShader, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
	  //Attribute, vals/attrib., type, normalized?, stride, offset
	  //Binds to VBO current GL_ARRAY_BUFFER
	glEnableVertexAttribArray(posAttrib);

	//GLint colAttrib = glGetAttribLocation(phongShader, "inColor");
	//glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
	//glEnableVertexAttribArray(colAttrib);

	GLint normAttrib = glGetAttribLocation(texturedShader, "inNormal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
	glEnableVertexAttribArray(normAttrib);

	GLint texAttrib = glGetAttribLocation(texturedShader, "inTexcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
                       8*sizeof(float), (void*)(3*sizeof(float)));


	glEnable(GL_DEPTH_TEST);

	//Event Loop (Loop forever processing each event as fast as possible)
	SDL_Event windowEvent;
	SDL_WarpMouseInWindow(window, screenWidth/2, screenHeight/2);   // initial mouse warp to center camera
	//Event Loop (Loop forever processing each event as fast as possible)
	bool isRunning = true;
	SDL_Event kbEvent;

	//oldTime = SDL_GetTicks()/1000.f;
    float previousGameTime = SDL_GetTicks()/1.0f;

    while (isRunning){
        SDL_ShowCursor(SDL_DISABLE);    // hide the cursor
        camera->shouldRecenter = false;

        float currenGameTime = SDL_GetTicks() / 1.0f;
        float elapsedGameTime = currenGameTime - previousGameTime;
        deltaT = deltaT + elapsedGameTime;
        previousGameTime = currenGameTime;

        /// TODO: change the calculation of deltaT
        //if (!saveOutput) timePast = SDL_GetTicks()/1000.f;
        //if (saveOutput) timePast += .07; //Fix framerate at 14 FPS

        //deltaT = timePast - oldTime;

        while(SDL_PollEvent(&kbEvent) != 0) {
            switch(kbEvent.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;

                case SDL_MOUSEMOTION:
                    if(!camera->shouldRecenter) {
                        camera->lastMouseX = camera->mouseX;
                        camera->lastMouseY = camera->mouseY;
                        SDL_GetMouseState(&camera->mouseX, &camera->mouseY);
                        camera->mouseDX = camera->mouseX - camera->lastMouseX;
                        camera->mouseDY = camera->mouseY - camera->lastMouseY;
                        camera->shouldRecenter = true;
                    }
                break;

                case SDL_KEYDOWN:
                    switch(kbEvent.key.keysym.sym) {

                        case SDLK_w:
                            if(camera->cMode == 1) {
                                camera->hasMoved = true;
                                camera->movedForward = true;
                            }

                            /*if(camera->cMode != 1) {
                                player->hasMoved = true;
                                player->movedForward = true;
                            }*/

                            break;

                        case SDLK_s:
                            if(camera->cMode == 1) {
                                camera->hasMoved = true;
                                camera->movedBackward = true;
                            }
                            /*if(camera->cMode != 1) {
                                player->hasMoved = true;
                                player->movedBackward = true;
                            }*/
                            break;

                        case SDLK_a:
                            if(camera->cMode == 1) {
                                camera->hasMoved = true;
                                camera->movedLeft = true;
                            }

                            if(camera->cMode != 1) {
                                player->hasMoved = true;
                                player->movedLeft = true;
                            }
                            break;

                        case SDLK_d:
                            if(camera->cMode == 1) {
                                camera->hasMoved = true;
                                camera->movedRight = true;
                            }

                            if(camera->cMode != 1) {
                                player->hasMoved = true;
                                player->movedRight = true;
                            }
                            break;

                        case SDLK_SPACE:
                            if(camera->cMode == 1) {
                                camera->hasMoved = true;
                                camera->movedUp = true;
                            }
                            break;

                        case SDLK_x:
                            if(camera->cMode == 1) {
                                camera->hasMoved = true;
                                camera->movedDown = true;
                            }
                            break;

                        case SDLK_q:

                            break;

                        case SDLK_e:
                            break;


                        case SDLK_c:
                            // Change camera mode
                            camera->changeMode();
                            break;

                        case SDLK_m:
                            // Print out the current level
                            /*for(int p = 0; p < level->nLanes; p ++) {

                            }*/

                            std::cout << "==== LEVEL ====\n";
                            for(int l = 0; l < level->nLanes; l++) {
                                for(int p = 0; p < level->lanes[0].nPaths; p++) {
                                    std::cout << level->lanes[l].paths[p];
                                }
                                std::cout << "\n";
                            }
                            std::cout << "==== ===== ====\n";

                            break;

                        case SDLK_f:
                            fullscreen = !fullscreen;
                            SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
                            break;

                        case SDLK_ESCAPE:
                            isRunning = false;
                            break;

                        default:
                            break;
                        }
                    break;
                default:
                        break;
            }
        } // \while(SDL_PollEvent(&kbEvent) != 0)

    /// //// UPDATE //// ///
    /// //// ////// //// ///
    /// Call Updater Functions
    while(deltaT > 16) {
        if(!level->hasFailed) {
            updateLighting(texturedShader);
            player->update(deltaT);
            level->update(deltaT);
        }
        camera->update(deltaT);
        deltaT -= 16;
    }

    /// Recenter the mouse if necessary
    if(camera->shouldRecenter) {
        SDL_WarpMouseInWindow(window, screenWidth/2, screenHeight/2);
    }

    /// Make camera movements from the mouse input
    if(camera->cMode == 1) { // If it is Free Cam
        camera->horiAngle += camera->mouseSpeed * deltaT * float(screenWidth/2 - camera->mouseX);
        camera->vertAngle += camera->mouseSpeed * deltaT * float(screenHeight/2 - camera->mouseY);
        //std::cout << "Last camera->horiAngle: " << camera->horiAngle << "\n";
        //std::cout << "Last camera->vertAngle: " << camera->vertAngle << "\n";
    } else {
        /// Fix the camera angle to "follow" the player
        // Default values chosen using the couts from above.
        camera->horiAngle = -3.16;
        camera->vertAngle = -0.32;
    }
    /// Restrict look around during follow cam
    //direction = glm::vec3(sin(camera->horiAngle), 0, cos(camera->horiAngle)); //Hori only
    direction = glm::vec3(cos(camera->vertAngle)*sin(camera->horiAngle), sin(camera->vertAngle), cos(camera->vertAngle)*cos(camera->horiAngle)); //Hori and Vert
    rightVector = glm::vec3(sin(camera->horiAngle-3.14f/2.0f), 0, cos(camera->horiAngle-3.14f/2.0f));
    upVector = glm::cross(rightVector, direction);


    /// Clear the screen to default color
    glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(texturedShader);



    /// For Moveable Camera...
    glm::mat4 view = glm::lookAt(
    glm::vec3(camera->posX, camera->posY, camera->posZ),  //Cam Position
    // glm::vec3(0 , 0, -1.0),  //Look at point
    glm::vec3(camera->posX, camera->posY, camera->posZ) + direction,
    // glm::vec3(0.0f, 1.0f, 0.0f)); //Up
    upVector);

    GLint uniView = glGetUniformLocation(texturedShader, "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

    //glm::mat4 proj = glm::perspective(3.14f/4, 1024.0f / 768.0f, 1.0f, 10.0f); //FOV, aspect, near, far
    glm::mat4 proj = glm::perspective(3.14f/4, screenWidthF / screenHeightF, 1.0f, 100.0f); //FOV, aspect, near plane, far plane
    GLint uniProj = glGetUniformLocation(texturedShader, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    // Activate the textures?
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    glUniform1i(glGetUniformLocation(texturedShader, "tex0"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glUniform1i(glGetUniformLocation(texturedShader, "tex1"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex2);
    glUniform1i(glGetUniformLocation(texturedShader, "tex2"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, tex3);
    glUniform1i(glGetUniformLocation(texturedShader, "tex3"), 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, tex4);
    glUniform1i(glGetUniformLocation(texturedShader, "tex4"), 4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, tex5);
    glUniform1i(glGetUniformLocation(texturedShader, "tex5"), 5);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, tex6);
    glUniform1i(glGetUniformLocation(texturedShader, "tex6"), 6);

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, tex7);
    glUniform1i(glGetUniformLocation(texturedShader, "tex7"), 7);



    /// //// RENDER //// ///
    /// //// ////// //// ///
    /// Call Rendering Functions
    // Ground Rendering
    drawGround(texturedShader, getModel("cube").start,getModel("cube").end);

    // Level Rendering
    drawLevel(texturedShader, getModel("cube").start,getModel("cube").end);
    drawLowLeftWall(texturedShader, getModel("cube").start,getModel("cube").end);
    drawLeftWall(texturedShader, getModel("cube").start,getModel("cube").end);
    drawLowRightWall(texturedShader, getModel("cube").start,getModel("cube").end);
    drawRightWall(texturedShader, getModel("cube").start,getModel("cube").end);
    drawLanes(texturedShader, getModel("cube").start,getModel("cube").end);

    // Turtle Rendering
    drawTurtle(texturedShader, getModel("cube").start,getModel("cube").end);
    drawTurtleHead(texturedShader, getModel("cube").start,getModel("cube").end);
    drawTurtleLeft(texturedShader, getModel("cube").start,getModel("cube").end);
    drawTurtleRight(texturedShader, getModel("cube").start,getModel("cube").end);
    drawTurtleTail(texturedShader, getModel("cube").start,getModel("cube").end);

    //UI Rendering

    //Screencoord and scale between 0 to 1 is recommended

    drawUI(texturedShader, getModel("quad").start, getModel("quad").end, 0, 0, 0.05, 2);
    drawNumber(texturedShader, 480, 368, 0.02, 4);
    drawNumber(texturedShader, 440, 368, 0.02, 3);
    drawNumber(texturedShader, 400, 368, 0.02, 2);
    drawNumber(texturedShader, 360, 368, 0.02, 1);

    if (saveOutput) Win2PPM(screenWidth,screenHeight);


      SDL_GL_SwapWindow(window); //Double buffering
	}

	glDeleteProgram(texturedShader);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, &vao);

	//Clean Up
	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}

void updateLighting(int shaderProgram) {
    /*GLint uniLightOffset = glGetUniformLocation(shaderProgram, "lightOffset");
    if(gLightAdjustment < 0.16 && !gLightReachedRightMax) {
        gLightAdjustment += 0.01;
        glUniform1i(uniLightOffset, gLightAdjustment); //Set the new light offset
    } else if(gLightAdjustment >= 0.16) {
        gLightReachedRightMax = true;
    }

    if(gLightAdjustment > -0.16 && gLightReachedRightMax) {
        gLightAdjustment -= 0.01;
        glUniform1i(uniLightOffset, gLightAdjustment); //Set the new light offset
    } else if(gLightAdjustment <= -0.16) {
        gLightReachedRightMax = false;
    }*/
}

void drawNumber(int shaderProgram, int xCoord, int yCoord, float scale, int number) {
    GLint unirenderNumber = glGetUniformLocation(shaderProgram, "renderNumber");
    glUniform1i(unirenderNumber, number); //Set UI Render on
    drawUI(shaderProgram, getModel("quad").start, getModel("quad").end, xCoord, yCoord, scale, 5);
    glUniform1i(unirenderNumber, -1); //Set UI Render on
}

void drawUI(int shaderProgram, int numVerts1, int numVerts2, int xCoord, int yCoord, float scale, int tID) {
    //Clamp xCoord and yCoord
    if (xCoord > screenWidth/2)
    {
        xCoord = screenWidth/2;
    }
    else if (xCoord < -screenWidth/2)
    {
        xCoord = -screenWidth/2;
    }

    if (yCoord > screenHeight/2)
    {
        yCoord = screenHeight/2;
    }
    else if (yCoord < -screenHeight/2)
    {
        yCoord = -screenHeight/2;
    }

    //Convert x and y coord to actual float number
    float xCoordReal;
    float yCoordReal;

    if (xCoord >= 0 && yCoord >= 0)
    {
        xCoordReal = 0.565 * ((float)xCoord/(screenWidth/2));
        yCoordReal = 0.385 * ((float)yCoord/(screenHeight/2));
        cout << "in first if" << endl;
        cout << "x is " << xCoordReal << endl;
        cout << "y is " << yCoordReal << endl;
    }
    else if (xCoord < 0 && yCoord >= 0)
    {
        xCoordReal = 0.548 * ((float)xCoord/(screenWidth/2));
        yCoordReal = 0.385 * ((float)yCoord/(screenHeight/2));
    }
    else if (yCoord < 0 && xCoord >= 0)
    {
        xCoordReal = 0.745 * ((float)xCoord/(screenWidth/2));
        yCoordReal = 0.64 * ((float)yCoord/(screenHeight/2));
    }

    else if (yCoord < 0 && xCoord < 0)
    {
        xCoordReal = 0.72 * ((float)xCoord/(screenWidth/2));
        yCoordReal = 0.625 * ((float)yCoord/(screenHeight/2));
    }

    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
    GLint uniOutline = glGetUniformLocation(shaderProgram, "drawOutline");
    GLint uniUIRender = glGetUniformLocation(shaderProgram, "UIRender");

    glm::mat4 model;
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    model = glm::translate(model,glm::vec3(camera->posX, camera->posY, camera->posZ)+glm::vec3(direction.x+xCoordReal, direction.y+yCoordReal, direction.z-0.14));
    model = glm::scale(model,glm::vec3(scale, scale, scale));
    model = glm::rotate(model, camera->horiAngle, upVector);
    model = glm::rotate(model, camera->vertAngle, rightVector);
    model = glm::rotate(model, (float)3.14, glm::vec3(1,0,0));
    model = glm::rotate(model, (float)1.57, glm::vec3(0,0,1));

    // model = glm::rotate(model,45.f,glm::vec3(0.0f, 1.0f, 0.0f));
    // model = glm::rotate(model,45.f,glm::vec3(0.0f, 0.0f, 1.0f));
    glUniform1i(uniTexID, tID); //Set texture ID to use
    glUniform1i(uniOutline, 0); //Set outline to off
    glUniform1i(uniUIRender, 1); //Set UI Render on
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, numVerts1, numVerts2);
}

void drawTurtle(int shaderProgram, int numVerts1, int numVerts2){
    if(player == NULL) {
        return;
    }
    /*float playerDX = 0;
    float playerDZ = 0;
    playerDX = player->posDX;
    player->posDX = 0;
    playerDZ = player->posDZ;
    player->posDZ = 0;*/
    float playerX = player->posX;
    float playerZ = player->posZ;

	GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(0.5,0.5,0.5);
    glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
    GLint uniOutline = glGetUniformLocation(shaderProgram, "drawOutline");
    GLint uniUIRender = glGetUniformLocation(shaderProgram, "UIRender");
    //glm::mat4 model;
    //GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    /// Draw Player
    // Drawing the shell...
    glm::mat4 model;
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    model = glm::scale(model,glm::vec3(1.0f, 0.8f, 1.1f));
    model = glm::translate(model,glm::vec3(playerX, 0.0f, playerZ));
    uniModel = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(uniTexID, 3); //Set texture ID to use
    glUniform1i(uniOutline, 1); //Set outline to on
    glUniform1i(uniUIRender, 0); //Set UI Render off
    //uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
    //glUniform3f(uniColor, 1.0f, 1.0f, 0.0f);    // This changes the color of the model with -1 texture
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    modelIndex currentModel = getModel("sphere");
    glDrawArrays(GL_TRIANGLES, currentModel.start, currentModel.end); //(Primitive Type, Start Vertex, End Vertex)

}

void drawTurtleHead(int shaderProgram, int numVerts1, int numVerts2) {
    if(player == NULL) {
        return;
    }
    float playerX = player->posX;
    float playerZ = player->posZ;

	GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(0.5,0.5,0.5);
    glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
    GLint uniOutline = glGetUniformLocation(shaderProgram, "drawOutline");
    GLint uniUIRender = glGetUniformLocation(shaderProgram, "UIRender");

    glm::mat4 model;
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    float xScale = 0.7f;
    float xAdjust = 1.0f / xScale;
    model = glm::scale(model,glm::vec3(xScale, 0.64f, 0.64f));
    model = glm::translate(model,glm::vec3(playerX * xAdjust * 1.032f , 0.0f, playerZ - 0.64f));   //1.032f on the x is for effect
    uniModel = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(uniTexID, 1); //Set texture ID to use
    glUniform1i(uniOutline, 1); //Set outline to on
    glUniform1i(uniUIRender, 0); //Set UI Render off
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    modelIndex currentModel = getModel("sphere");
    glDrawArrays(GL_TRIANGLES, currentModel.start, currentModel.end); //(Primitive Type, Start Vertex, End Vertex)


}

void drawTurtleLeft(int shaderProgram, int numVerts1, int numVerts2) {
    if(player == NULL) {
        return;
    }
    float playerX = player->posX;
    float playerZ = player->posZ;

	GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(0.5,0.5,0.5);
    glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
    GLint uniOutline = glGetUniformLocation(shaderProgram, "drawOutline");
    GLint uniUIRender = glGetUniformLocation(shaderProgram, "UIRender");

    glm::mat4 model;
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    float xScale = 0.75f;
    float xAdjust = 1.0f / xScale;
    model = glm::scale(model,glm::vec3(xScale, 0.2f, 0.44f));
    model = glm::translate(model,glm::vec3(playerX * xAdjust -1.0f, player->leftFootOffsetY, playerZ));   //1.032f on the x is for effect
    uniModel = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(uniTexID, 1); //Set texture ID to use
    glUniform1i(uniOutline, 1); //Set outline to on
    glUniform1i(uniUIRender, 0); //Set UI Render off
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    modelIndex currentModel = getModel("sphere");
    glDrawArrays(GL_TRIANGLES, currentModel.start, currentModel.end); //(Primitive Type, Start Vertex, End Vertex)


}

void drawTurtleRight(int shaderProgram, int numVerts1, int numVerts2) {
    if(player == NULL) {
        return;
    }
    float playerX = player->posX;
    float playerZ = player->posZ;

	GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(0.5,0.5,0.5);
    glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
    GLint uniOutline = glGetUniformLocation(shaderProgram, "drawOutline");
    GLint uniUIRender = glGetUniformLocation(shaderProgram, "UIRender");

    glm::mat4 model;
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    float xScale = 0.75f;
    float xAdjust = 1.0f / xScale;
    model = glm::scale(model,glm::vec3(xScale, 0.2f, 0.44f));
    model = glm::translate(model,glm::vec3(playerX * xAdjust + 1.0f, player->rightFootOffsetY, playerZ));   //1.032f on the x is for effect
    uniModel = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(uniTexID, 1); //Set texture ID to use
    glUniform1i(uniOutline, 1); //Set outline to on
    glUniform1i(uniUIRender, 0); //Set UI Render off
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    modelIndex currentModel = getModel("sphere");
    glDrawArrays(GL_TRIANGLES, currentModel.start, currentModel.end); //(Primitive Type, Start Vertex, End Vertex)


}

void drawTurtleTail(int shaderProgram, int numVerts1, int numVerts2) {
    if(player == NULL) {
        return;
    }
    float playerX = player->posX;
    float playerZ = player->posZ;

	GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(0.5,0.5,0.5);
    glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
    GLint uniOutline = glGetUniformLocation(shaderProgram, "drawOutline");
    GLint uniUIRender = glGetUniformLocation(shaderProgram, "UIRender");

    glm::mat4 model;
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    float xScale = 0.16f;
    float xAdjust = 1.0f / xScale;
    model = glm::scale(model,glm::vec3(xScale, 0.04f, 0.64f));
    model = glm::translate(model,glm::vec3( -playerX * -(xAdjust * 1.032f) , 0.0f, playerZ + 0.64f));   //1.032f on the x is for effect
    uniModel = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(uniTexID, 1); //Set texture ID to use
    glUniform1i(uniOutline, 1); //Set outline to on
    glUniform1i(uniUIRender, 0); //Set UI Render off
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    modelIndex currentModel = getModel("sphere");
    glDrawArrays(GL_TRIANGLES, currentModel.start, currentModel.end); //(Primitive Type, Start Vertex, End Vertex)


}

void drawGround(int shaderProgram, int numVerts1, int numVerts2) {
    if(level == NULL) {
        return;
    }

    GLint uniColor1 = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(0.5,0.5,0.5);
    glUniform3fv(uniColor1, 1, glm::value_ptr(colVec));
    GLint uniTexID1 = glGetUniformLocation(shaderProgram, "texID");
    GLint uniOutline = glGetUniformLocation(shaderProgram, "drawOutline"); //Set it to not rendering outline
    GLint uniUIRender = glGetUniformLocation(shaderProgram, "UIRender");
    //glm::mat4 model;
    //GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    /// Draw Ground Plane
    glm::mat4 model;
    GLint uniModel1 = glGetUniformLocation(shaderProgram, "model");
    model = glm::scale(model,glm::vec3(8.0f * level->xWidth * level->xDrawingScale, 1.0f, 4.0f * level->zWidth));
    model = glm::translate(model,glm::vec3(0.0f, -1.4f, 0.0f));   // Draws relative to the camera...
    uniModel1 = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel1, 1, GL_FALSE, glm::value_ptr(model));
    //model = glm::rotate(model,timePast * .5f * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
    //model = glm::rotate(model,timePast * .5f * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
    glUniform1i(uniTexID1, 6); //Set texture ID to use
    glUniform1i(uniOutline, 0); //Set outline to off
    glUniform1i(uniUIRender, 0); //Set UI Render off
    //uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
    //glUniform3f(uniColor, 1.0f, 1.0f, 0.0f);    // This changes the color of the model with -1 texture
    glUniformMatrix4fv(uniModel1, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, numVerts1, numVerts2); //(Primitive Type, Start Vertex, End Vertex)

}

void drawLevel(int shaderProgram, int numVerts1, int numVerts2) {
    if(level == NULL) {
        return;
    }
    GLint uniColor1 = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(0.5,0.5,0.5);
    glUniform3fv(uniColor1, 1, glm::value_ptr(colVec));
    GLint uniTexID1 = glGetUniformLocation(shaderProgram, "texID");
    GLint uniOutline = glGetUniformLocation(shaderProgram, "drawOutline"); //Set it to not rendering outline
    //glm::mat4 model;
    //GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    /// Draw Secondary Ground Plane
    glm::mat4 model;
    GLint uniModel1 = glGetUniformLocation(shaderProgram, "model");
    model = glm::scale(model,glm::vec3((level->xDrawingScale/2.0f) * level->xWidth, 1.0f, 4.0f * level->zWidth));
    model = glm::translate(model,glm::vec3(0.0f, -1.0f, 0.0f));   // Draws relative to the camera...
    uniModel1 = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel1, 1, GL_FALSE, glm::value_ptr(model));
    //model = glm::rotate(model,timePast * .5f * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
    //model = glm::rotate(model,timePast * .5f * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
    glUniform1i(uniTexID1, 6); //Set texture ID to use
    glUniform1i(uniOutline, 0); //Set outline on / off
    //uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
    //glUniform3f(uniColor, 1.0f, 1.0f, 0.0f);    // This changes the color of the model with -1 texture
    glUniformMatrix4fv(uniModel1, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, numVerts1, numVerts2); //(Primitive Type, Start Vertex, End Vertex

}

void drawLowLeftWall(int shaderProgram, int numVerts1, int numVerts2) {
    if(level == NULL) {
        return;
    }
    GLint uniColor1 = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(0.5,0.5,0.5);
    glUniform3fv(uniColor1, 1, glm::value_ptr(colVec));
    GLint uniTexID1 = glGetUniformLocation(shaderProgram, "texID");
    GLint uniOutline = glGetUniformLocation(shaderProgram, "drawOutline"); //Set it to not rendering outline
    //glm::mat4 model;
    //GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    /// Draw Low Left Wall Plane
    glm::mat4 model;
    GLint uniModel1 = glGetUniformLocation(shaderProgram, "model");
    model = glm::scale(model,glm::vec3(1.0f, 1.0f, 4.0f * level->zWidth));
    model = glm::translate(model,glm::vec3(-(level->lWidth/2.0f) - 7.32f, -1.0f, 0.0f));   // Draws relative to the camera...
    uniModel1 = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel1, 1, GL_FALSE, glm::value_ptr(model));
    //model = glm::rotate(model,timePast * .5f * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
    //model = glm::rotate(model,timePast * .5f * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
    glUniform1i(uniTexID1, 0); //Set texture ID to use
    glUniform1i(uniOutline, 0); //Set outline on / off
    //uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
    //glUniform3f(uniColor, 1.0f, 1.0f, 0.0f);    // This changes the color of the model with -1 texture
    glUniformMatrix4fv(uniModel1, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, numVerts1, numVerts2); //(Primitive Type, Start Vertex, End Vertex


}

void drawLowRightWall(int shaderProgram, int numVerts1, int numVerts2) {
    if(level == NULL) {
        return;
    }
    GLint uniColor1 = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(0.5,0.5,0.5);
    glUniform3fv(uniColor1, 1, glm::value_ptr(colVec));
    GLint uniTexID1 = glGetUniformLocation(shaderProgram, "texID");
    GLint uniOutline = glGetUniformLocation(shaderProgram, "drawOutline"); //Set it to not rendering outline
    //glm::mat4 model;
    //GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    /// Draw Low Right Wall Plane
    glm::mat4 model;
    GLint uniModel1 = glGetUniformLocation(shaderProgram, "model");
    model = glm::scale(model,glm::vec3(1.0f, 1.0f, 4.0f * level->zWidth));
    model = glm::translate(model,glm::vec3((level->lWidth/2.0f) + 7.32f, -1.0f, 0.0f));   // Draws relative to the camera...
    uniModel1 = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel1, 1, GL_FALSE, glm::value_ptr(model));
    //model = glm::rotate(model,timePast * .5f * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
    //model = glm::rotate(model,timePast * .5f * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
    glUniform1i(uniTexID1, 0); //Set texture ID to use
    glUniform1i(uniOutline, 0); //Set outline on / off
    //uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
    //glUniform3f(uniColor, 1.0f, 1.0f, 0.0f);    // This changes the color of the model with -1 texture
    glUniformMatrix4fv(uniModel1, 1, GL_FALSE, glm::value_ptr(model));
    //modelIndex currentModel = getModel("cube");
    glDrawArrays(GL_TRIANGLES, numVerts1, numVerts2); //(Primitive Type, Start Vertex, End Vertex


}

void drawLeftWall(int shaderProgram, int numVerts1, int numVerts2) {
    if(player == NULL) {
            return;
    }
    float playerX = player->posX;
    float playerZ = player->posZ;

    GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(0.5,0.5,0.5);
    glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
    GLint uniOutline = glGetUniformLocation(shaderProgram, "drawOutline");
    //glm::mat4 model;
    //GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    ///-- TODO: create helper functions for drawing an instance of each model at a certain location... example: drawRockAt(float x, float y, int texID)
                        // These can be used within the drawLanes() for loop

    /// Draw Lane Objects
    float offsetZ = 0.0f;
    int lastDrawnLane = level->frontLane + level->numberOfLanesDrawn;
    if(lastDrawnLane > level->nLanes) {
        lastDrawnLane = level->nLanes;
    }

    for(int L = level->frontLane; L < lastDrawnLane; L++) {
        glm::mat4 model;
        GLint uniModel = glGetUniformLocation(shaderProgram, "model");

        //float rockScale = 2.0f* level->xDrawingScale / 3.0f;
        //float spacingAdjust = level->xDrawingScale/1.16f;
        //model = glm::scale(model,glm::vec3(spacingAdjust, 1.0f, 1.1f));
        //model = glm::scale(model,glm::vec3(1.0f, 1.0f, 1.1f));
        offsetZ = playerZ - level->laneSpacing*L - 2.0f + level->zOffset;
        model = glm::translate(model,glm::vec3( (-(level->lWidth/2.0f))*2.0f - 4.0f, 0.64f, offsetZ));
        uniModel = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        //model = glm::rotate(model,timePast * .5f * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
        //model = glm::rotate(model,timePast * .5f * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));

        ///-- TODO: use a random texture
        //int texNum = rand() % 4;    ///-- TODO: have the texture allocation store a global number of total textures to use here
        glUniform1i(uniTexID, 2); //Set texture ID to use
        glUniform1i(uniOutline, 1); //Set outline to on
        //uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
        //glUniform3f(uniColor, 1.0f, 1.0f, 0.0f);    // This changes the color of the model with -1 texture
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        modelIndex currentModel = getModel("sphere");
        glDrawArrays(GL_TRIANGLES, currentModel.start, currentModel.end); //(Primitive Type, Start Vertex, End Vertex)
    }

}

void drawRightWall(int shaderProgram, int numVerts1, int numVerts2) {
    if(player == NULL) {
            return;
    }
    float playerX = player->posX;
    float playerZ = player->posZ;

    GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(0.5,0.5,0.5);
    glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
    GLint uniOutline = glGetUniformLocation(shaderProgram, "drawOutline");
    //glm::mat4 model;
    //GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    ///-- TODO: create helper functions for drawing an instance of each model at a certain location... example: drawRockAt(float x, float y, int texID)
                        // These can be used within the drawLanes() for loop

    /// Draw Lane Objects
    float offsetZ = 0.0f;
    int lastDrawnLane = level->frontLane + level->numberOfLanesDrawn;
    if(lastDrawnLane > level->nLanes) {
        lastDrawnLane = level->nLanes;
    }

    for(int L = level->frontLane; L < lastDrawnLane; L++) {
        glm::mat4 model;
        GLint uniModel = glGetUniformLocation(shaderProgram, "model");

        //float rockScale = 2.0f* level->xDrawingScale / 3.0f;
        //float spacingAdjust = level->xDrawingScale/1.16f;
        //model = glm::scale(model,glm::vec3(spacingAdjust, 1.0f, 1.1f));
        //model = glm::scale(model,glm::vec3(1.0f, 1.0f, 1.1f));
        offsetZ = playerZ - level->laneSpacing*L - 2.0f + level->zOffset;
        model = glm::translate(model,glm::vec3( (level->lWidth/2.0f)*2.0f + 4.0f, 0.64f, offsetZ));
        uniModel = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        //model = glm::rotate(model,timePast * .5f * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
        //model = glm::rotate(model,timePast * .5f * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));

        ///-- TODO: use a random texture
        //int texNum = rand() % 4;    ///-- TODO: have the texture allocation store a global number of total textures to use here
        glUniform1i(uniTexID, 2); //Set texture ID to use
        glUniform1i(uniOutline, 1); //Set outline to on
        //uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
        //glUniform3f(uniColor, 1.0f, 1.0f, 0.0f);    // This changes the color of the model with -1 texture
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        modelIndex currentModel = getModel("sphere");
        glDrawArrays(GL_TRIANGLES, currentModel.start, currentModel.end); //(Primitive Type, Start Vertex, End Vertex)
    }
}

void drawLanes(int shaderProgram, int numVerts1, int numVerts2){
    if(player == NULL) {
            return;
    }
    float playerX = player->posX;
    float playerZ = player->posZ;

    GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(0.5,0.5,0.5);
    glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
    GLint uniOutline = glGetUniformLocation(shaderProgram, "drawOutline");
    //glm::mat4 model;
    //GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    ///-- TODO: create helper functions for drawing an instance of each model at a certain location... example: drawRockAt(float x, float y, int texID)
                        // These can be used within the drawLanes() for loop

    /// Draw Lane Objects
    float offsetZ = 0.0f;
    int lastDrawnLane = level->frontLane + level->numberOfLanesDrawn;
    if(lastDrawnLane > level->nLanes) {
        lastDrawnLane = level->nLanes;
    }

    for(int L = level->frontLane; L < lastDrawnLane; L++) {    ///-- TODO: adjust drawLanes() to only draw 4 lanes...
        for(int i = -1; i < level->lanes[L].nPaths + 1; i++) {


            // Draw edge cubes
            if(i == -1) {
                    /*glm::mat4 model;
                    GLint uniModel = glGetUniformLocation(shaderProgram, "model");

                    //float rockScale = 2.0f* level->xDrawingScale / 3.0f;
                    float spacingAdjust = level->xDrawingScale/1.16f;
                    model = glm::scale(model,glm::vec3(spacingAdjust, 1.0f, 1.1f));
                    //model = glm::scale(model,glm::vec3(1.0f, 1.0f, 1.1f));
                    offsetZ = playerZ - level->laneSpacing*L - 2.0f + level->zOffset;
                    model = glm::translate(model,glm::vec3( (-(level->lWidth/2)) - 1.0f, 0.2f, offsetZ));
                    uniModel = glGetUniformLocation(shaderProgram, "model");
                    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

                    ///-- TODO: use a random texture
                    //int texNum = rand() % 4;    ///-- TODO: have the texture allocation store a global number of total textures to use here
                    glUniform1i(uniTexID, 3); //Set texture ID to use
                    glUniform1i(uniOutline, 0); //Set outline to on
                    //uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
                    //glUniform3f(uniColor, 1.0f, 1.0f, 0.0f);    // This changes the color of the model with -1 texture
                    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                    modelIndex currentModel = getModel("sphere");
                    glDrawArrays(GL_TRIANGLES, currentModel.start, currentModel.end); //(Primitive Type, Start Vertex, End Vertex)
                    */
            }

            if(i > -1 && i < level->lanes[L].nPaths) {
                if(level->lanes[L].paths[i] != 0) { // 0 would mean the path space within the lane is empty
                    glm::mat4 model;
                    GLint uniModel = glGetUniformLocation(shaderProgram, "model");

                    //float rockScale = 2.0f* level->xDrawingScale / 3.0f;
                    float spacingAdjust = level->xDrawingScale/1.16f;
                    model = glm::scale(model,glm::vec3(spacingAdjust, 1.0f, 1.1f));
                    //model = glm::scale(model,glm::vec3(1.0f, 1.0f, 1.1f));
                    offsetZ = playerZ - level->laneSpacing*L - 2.0f + level->zOffset -(4*level->laneSpacing/5) ;
                    model = glm::translate(model,glm::vec3( (-(level->lWidth/2)) + i, 0.2f, offsetZ));
                    uniModel = glGetUniformLocation(shaderProgram, "model");
                    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                    //model = glm::rotate(model,timePast * .5f * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
                    //model = glm::rotate(model,timePast * .5f * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));

                    ///-- TODO: use a random texture
                    int texNum = level->lanes[L].paths[i];    //rand() % 4;    ///-- TODO: have the texture allocation store a global number of total textures to use here
                    if(texNum == 3) {   // make brown to red
                        texNum = 0;
                    }
                   /* if(texNum == 1) {   // make green to blue
                        texNum = 2;
                    }*/
                    glUniform1i(uniTexID, texNum); //Set texture ID to use
                    glUniform1i(uniOutline, 0); //Set outline to on
                    //uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
                    //glUniform3f(uniColor, 1.0f, 1.0f, 0.0f);    // This changes the color of the model with -1 texture
                    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                    modelIndex currentModel = getModel("rock");


                    if(i > 0 && i != 1) {
                        if(level->lanes[L].paths[i-1] == 0) {
                            currentModel = getModel("cube");
                            //model = glm::scale(model,glm::vec3(0.64f, 1.0f, 1.0f));
                        }
                    } else {
                        //currentModel = getModel("rock");
                    }

                    if(i < level->lanes[L].nPaths && i != level->lanes[L].nPaths - 1) {
                        if(level->lanes[L].paths[i+1] == 0) {
                            currentModel = getModel("cube");
                            //model = glm::scale(model,glm::vec3(0.64f, 1.0f, 1.0f));
                        }
                    } else {
                        //currentModel = getModel("rock");
                    }

                    glDrawArrays(GL_TRIANGLES, currentModel.start, currentModel.end); //(Primitive Type, Start Vertex, End Vertex)
                }
            }
        }
    }
}

//Model access implementation
void setModel(string fileName, string modelName) {
    //Reading the model
    fstream modelFile;
    string path = "models/" + fileName;
    modelFile.open(path.c_str());
    int numLines = 0;
    modelFile >> numLines;
    int numVerts = numLines/8;
    int startIndex;
    if (modelData.empty())
    {
        startIndex = 0;
    }
    else
    {
        startIndex = modelData.size()/8;
    }

    for (int i = 0; i < numLines; i++){
        float current;
        modelFile >> current;
        modelData.push_back(current);
    }
    modelFile.close();
    int endIndex = numVerts;
    assert(startIndex != endIndex);

    modelIndex temp(startIndex, endIndex);
    modelDict[modelName] = temp;
}

modelIndex getModel(string modelName) {
    modelIndex returnIndex = modelDict[modelName];
    return returnIndex;
}


// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile)
{
	FILE *fp;
	long length;
	char *buffer;

	// open the file containing the text of the shader code
	fp = fopen(shaderFile, "r");

	// check for errors in opening the file
	if (fp == NULL) {
		printf("can't open shader source file %s\n", shaderFile);
		return NULL;
	}

	// determine the file size
	fseek(fp, 0, SEEK_END); // move position indicator to the end of the file;
	length = ftell(fp);  // return the value of the current position

	// allocate a buffer with the indicated number of bytes, plus one
	buffer = new char[length + 1];

	// read the appropriate number of bytes from the file
	fseek(fp, 0, SEEK_SET);  // move position indicator to the start of the file
	fread(buffer, 1, length, fp); // read all of the bytes

	// append a NULL character to indicate the end of the string
	buffer[length] = '\0';

	// close the file
	fclose(fp);

	// return the string
	return buffer;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName)
{
	GLuint vertex_shader, fragment_shader;
	GLchar *vs_text, *fs_text;
	GLuint program;

	// check GLSL version
	printf("GLSL version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Create shader handlers
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	// Read source code from shader files
	vs_text = readShaderSource(vShaderFileName);
	fs_text = readShaderSource(fShaderFileName);

	// error check
	if (vs_text == NULL) {
		printf("Failed to read from vertex shader file %s\n", vShaderFileName);
		exit(1);
	} else if (DEBUG_ON) {
		printf("Vertex Shader:\n=====================\n");
		printf("%s\n", vs_text);
		printf("=====================\n\n");
	}
	if (fs_text == NULL) {
		printf("Failed to read from fragent shader file %s\n", fShaderFileName);
		exit(1);
	} else if (DEBUG_ON) {
		printf("\nFragment Shader:\n=====================\n");
		printf("%s\n", fs_text);
		printf("=====================\n\n");
	}

	// Load Vertex Shader
	const char *vv = vs_text;
	glShaderSource(vertex_shader, 1, &vv, NULL);  //Read source
	glCompileShader(vertex_shader); // Compile shaders

	// Check for errors
	GLint  compiled;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		printf("Vertex shader failed to compile:\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(vertex_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}

	// Load Fragment Shader
	const char *ff = fs_text;
	glShaderSource(fragment_shader, 1, &ff, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);

	//Check for Errors
	if (!compiled) {
		printf("Fragment shader failed to compile\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(fragment_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}

	// Create the program
	program = glCreateProgram();

	// Attach shaders to program
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	// Link and set program to use
	glLinkProgram(program);

	return program;
}

void Win2PPM(int width, int height){
	char outdir[10] = "out/"; //Must be defined!
	int i,j;
	FILE* fptr;
    static int counter = 0;
    char fname[32];
    unsigned char *image;

    /* Allocate our buffer for the image */
    image = (unsigned char *)malloc(3*width*height*sizeof(char));
    if (image == NULL) {
      fprintf(stderr,"ERROR: Failed to allocate memory for image\n");
    }

    /* Open the file */
    sprintf(fname,"%simage_%04d.ppm",outdir,counter);
    if ((fptr = fopen(fname,"w")) == NULL) {
      fprintf(stderr,"ERROR: Failed to open file for window capture\n");
    }

    /* Copy the image into our buffer */
    glReadBuffer(GL_BACK);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);

    /* Write the PPM file */
    fprintf(fptr,"P6\n%d %d\n255\n",width,height);
    for (j=height-1;j>=0;j--) {
      for (i=0;i<width;i++) {
         fputc(image[3*j*width+3*i+0],fptr);
         fputc(image[3*j*width+3*i+1],fptr);
         fputc(image[3*j*width+3*i+2],fptr);
      }
    }

    free(image);
    fclose(fptr);
    counter++;
}
