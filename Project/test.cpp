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
#include<stdlib.h>
#include <map>
#include <vector>

#include "Player.h"
#include "Camera.h"
#include "Level.h"
#include "modelIndex.h"

using namespace std;


/// //// //// GENERAL TO-DO LIST //// //// ///
//-- Work on lighting and shader files
//-- Generate content
//-- Get a new turtle model made and animated.
//-- Allow the player's turtle to move
//-- Bound the camera above the ground plane
//-- Setup the button 'C' to switch between player movement and camera movement.  This means, I can move my camera freely, then press C to move the turtle... back and forth.
        // This is a state we could store somewhere
//-- Try to add more lights and/or make the the ground light up rather than just that odd circle underneath the camera
//-- Make movements relative to the facing of the camera?  If you press forward while facing a direction, you should move that direction.  This may be hard.
//-- Make event queue loop allow multiple key presses per cycle... currently you can not move forward-left, forward-right, etc.
        // We can do this by storing the states of the keyboard.  Flag the 'holdingRight' state on key press to true... flag it as false on key up.
//-- Add sounds to the game
//-- Update functions should be moved to Game::update()
//-- Change Player::update() to a function of time exactly as Camera::update() is.
//-- Possibly move rendering functions to Game::render() - may not be necessary
//-- Create a texture manager if time permits

/// //// //// END OF TODO LIST //// //// ///

bool saveOutput = false;
float timePast = 0;
float oldTime = SDL_GetTicks()/1000.f;
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
void drawTurtle(int shaderProgram, int numVerts1, int numVerts2);
void drawLevel(int shaderProgram, int numVerts1, int numVerts2);
void drawGround(int shaderProgram, int numVerts1, int numVerts2);
void drawCubeFriend(int shaderProgram, int numVerts1, int numVerts2);

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

    /// Instantiate game objects
    player = new Player();
    camera = new Camera();
    level = new Level(128, 128);

    if(player == NULL) {
        printf("Error: Failed to create player1\n"); return 1;
    }
    // Set starting position
    player->posZ = 0.0f;

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
    
    setModel("cube.txt", "cube");
    setModel("Rock42Verts.txt", "rock");
    setModel("sphere.txt", "sphere");

	// //Load Model 1
	// ifstream modelFile;
	// modelFile.open("models/sphere.txt");
	// int numLines = 0;
	// modelFile >> numLines;
	// float* model1 = new float[numLines];
	// for (int i = 0; i < numLines; i++){
	// 	modelFile >> model1[i];
	// }	int numVerts1 = numLines/8;
	// modelFile.close();

	// printf("%d\n",numLines);

	// //Load Model 2
	// modelFile.open("models/cube.txt");
	// numLines = 0;
	// modelFile >> numLines;
	// float* model2 = new float[numLines];
	// for (int i = 0; i < numLines; i++){
	// 	modelFile >> model2[i];
	// }
	// printf("%d\n",numLines);
	// int numVerts2 = numLines/8;
	// modelFile.close();

	// //SJG: I load each model in a different array, then concatenate everything in one big array
	// //     There are better options, but this works.
	// //Concatenate model arrays
	// float* modelData = new float[(numVerts1+numVerts2)*8];
	// copy(model1, model1+numVerts1*8, modelData);
	// copy(model2, model2+numVerts2*8, modelData+numVerts1*8);
	// int totalNumVerts = numVerts1+numVerts2;


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
	SDL_Surface* surface3 = SDL_LoadBMP("lightBrown.bmp");
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


	//Allocate memory on the graphics card to store geometry (vertex buffer object)
	GLuint vbo[1];
	glGenBuffers(1, vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	// glBufferData(GL_ARRAY_BUFFER, totalNumVerts*8*sizeof(float), modelData, GL_STATIC_DRAW); //upload vertices to vbo
    glBufferData(GL_ARRAY_BUFFER, modelData.size()*sizeof(float), &modelData[0], GL_STATIC_DRAW); //upload vertices to vbo
	//GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
	//GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used

	int texturedShader = InitShader("vertexTex.glsl", "fragmentTex.glsl");

	//int phongShader = InitShader("vertex.glsl", "fragment.glsl");



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

    /// TODO move to Game::init()
	//Event Loop (Loop forever processing each event as fast as possible)
	SDL_Event windowEvent;
	SDL_WarpMouseInWindow(window, screenWidth/2, screenHeight/2);   // initial mouse warp to center camera
	//Event Loop (Loop forever processing each event as fast as possible)
	bool isRunning = true;
	SDL_Event kbEvent;
    while (isRunning){
        SDL_ShowCursor(SDL_DISABLE);    // hide the cursor
        camera->shouldRecenter = false;

        /// TODO: change the calculation of deltaT
        if (!saveOutput) timePast = SDL_GetTicks()/1000.f;
        if (saveOutput) timePast += .07; //Fix framerate at 14 FPS
        deltaT = timePast - oldTime;

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
                            camera->hasMoved = true;
                            camera->movedForward = true;
                            player->hasMoved = true;
                            player->movedForward = true;
                            break;

                        case SDLK_s:
                            camera->hasMoved = true;
                            camera->movedBackward = true;
                            camera->hasMoved = true;
                            camera->movedBackward = true;
                            break;

                        case SDLK_a:
                            camera->hasMoved = true;
                            camera->movedLeft = true;
                            player->hasMoved = true;
                            player->movedLeft = true;
                            break;

                        case SDLK_d:
                            camera->hasMoved = true;
                            camera->movedRight = true;
                            player->hasMoved = true;
                            player->movedRight = true;
                            break;

                        case SDLK_SPACE:
                            camera->hasMoved = true;
                            camera->movedUp = true;
                            break;

                        case SDLK_x:
                            camera->hasMoved = true;
                            camera->movedDown = true;
                            break;

                        case SDLK_q:

                            break;

                        case SDLK_e:
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

    /// Recenter the mouse if necessary
    if(camera->shouldRecenter) {
        SDL_WarpMouseInWindow(window, screenWidth/2, screenHeight/2);
    }

    /// Make camera movements from the mouse input
    camera->horiAngle += camera->mouseSpeed * deltaT * float(screenWidth/2 - camera->mouseX);
    camera->vertAngle += camera->mouseSpeed * deltaT * float(screenHeight/2 - camera->mouseY);
    //direction = glm::vec3(sin(camera->horiAngle), 0, cos(camera->horiAngle)); //Hori only
    direction = glm::vec3(cos(camera->vertAngle)*sin(camera->horiAngle), sin(camera->vertAngle), cos(camera->vertAngle)*cos(camera->horiAngle)); //Hori and Vert
    rightVector = glm::vec3(sin(camera->horiAngle-3.14f/2.0f), 0, cos(camera->horiAngle-3.14f/2.0f));
    upVector = glm::cross(rightVector, direction);


    /// Clear the screen to default color
    glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(texturedShader);

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


    /// Call Updater Functions
    updateLighting(texturedShader);
    player->update(deltaT);
    camera->update(deltaT);


    /// Call Rendering Functions
    drawGround(texturedShader, getModel("cube").start,getModel("cube").end);
    //drawLevel(texturedShader, numVerts1,numVerts2);
    drawTurtle(texturedShader, getModel("cube").start,getModel("cube").end);
    //drawCubeFriend(texturedShader, numVerts1,numVerts2);

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

void drawTurtle(int shaderProgram, int numVerts1, int numVerts2){

	GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(0.5,0.5,0.5);
    glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
    GLint uniOutline = glGetUniformLocation(shaderProgram, "drawOutline");
    //glm::mat4 model;
    //GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    /// Draw Player
    // Drawing the shell...
    glm::mat4 model;
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    model = glm::scale(model,glm::vec3(1.0f, 0.8f, 1.1f));
    model = glm::translate(model,glm::vec3(0.0f + player->posDX, 0.0f, 0.0f + player->posDZ));   // Draws relative to the camera...
    uniModel = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    //model = glm::rotate(model,timePast * .5f * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
    //model = glm::rotate(model,timePast * .5f * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
    glUniform1i(uniTexID, 0); //Set texture ID to use
    glUniform1i(uniOutline, 1); //Set outline to on
    //uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
    //glUniform3f(uniColor, 1.0f, 1.0f, 0.0f);    // This changes the color of the model with -1 texture
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    modelIndex currentModel = getModel("sphere");
    glDrawArrays(GL_TRIANGLES, currentModel.start, currentModel.end); //(Primitive Type, Start Vertex, End Vertex)
/*
    // Drawing the head...
    //glm::mat4 model;
    //GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    model = glm::scale(model,glm::vec3(0.54f, 0.49f, 0.4f));
    //model = glm::translate(model,glm::vec3(player1->posX, player1->posY + 0.4f, player1->posZ + 1.0f));
    model = glm::translate(model,glm::vec3(camera->posX + direction.x + player1->headOffsetX, camera->posY + direction.y - 3.5f + player1->headOffsetY - player1->shellOffsetY, camera->posZ + direction.z - 9.19f));   // Draws relative to the camera...
    uniModel = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    //model = glm::rotate(model,timePast * .5f * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
    //model = glm::rotate(model,timePast * .5f * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
    glUniform1i(uniTexID, 1); //Set texture ID to use
    //uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
    //glUniform3f(uniColor, 1.0f, 1.0f, 0.0f);    // This changes the color of the model with -1 texture
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, numVerts1); //(Primitive Type, Start Vertex, End Vertex)
    // Undo transformation
    model = glm::translate(model,glm::vec3(-player1->headOffsetX, -player1->headOffsetY, 0));


    /// Left Flipper
    model = glm::scale(model,glm::vec3(1.64f, 0.64f, 0.84f));
    //model = glm::translate(model,glm::vec3(player1->posX, player1->posY + 0.4f, player1->posZ + 1.0f));

    //model = glm::translate(model,glm::vec3(camPosX + direction.x -1.64f - player1->headOffsetX, camPosY + direction.y - 5.0f + player1->leftFootOffsetY - player1->headOffsetY, camPosZ + direction.z - 7.1f));   // Draws relative to the camera...
    model = glm::translate(model,glm::vec3(camPosX + direction.x -0.75f, camPosY + direction.y - 5.0f + player1->leftFootOffsetY, camPosZ + direction.z - 7.1f + 0.19f));   // Draws relative to the camera...

    uniModel = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(uniTexID, 1); //Set texture ID to use
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, numVerts1); //(Primitive Type, Start Vertex, End Vertex)
    // Undo transformation
    model = glm::translate(model,glm::vec3(0, -player1->leftFootOffsetY, 0));

    /// Right Flipper
    //model = glm::scale(model,glm::vec3(0.84f, 0.84f, 0.84f));
    //model = glm::translate(model,glm::vec3(player1->posX, player1->posY + 0.4f, player1->posZ + 1.0f));

    //model = glm::translate(model,glm::vec3(3.25f, 0, 0));   // Draws relative to the camera...
    model = glm::translate(model,glm::vec3(1.49f, player1->rightFootOffsetY, 0));   // Draws relative to the camera...

    uniModel = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(uniTexID, 1); //Set texture ID to use
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, numVerts1); //(Primitive Type, Start Vertex, End Vertex)
*/

}

void drawGround(int shaderProgram, int numVerts1, int numVerts2) {
    GLint uniColor1 = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(0.5,0.5,0.5);
    glUniform3fv(uniColor1, 1, glm::value_ptr(colVec));
    GLint uniTexID1 = glGetUniformLocation(shaderProgram, "texID");
    GLint uniOutline = glGetUniformLocation(shaderProgram, "drawOutline"); //Set it to not rendering outline
    //glm::mat4 model;
    //GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    /// Draw Ground
    glm::mat4 model;
    GLint uniModel1 = glGetUniformLocation(shaderProgram, "model"); 
    model = glm::scale(model,glm::vec3(1.0f * level->xWidth, 1.0f, 1.0f * level->zWidth));
    model = glm::translate(model,glm::vec3(0.0f, -1.0f, 0.0f));   // Draws relative to the camera...
    uniModel1 = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel1, 1, GL_FALSE, glm::value_ptr(model));
    //model = glm::rotate(model,timePast * .5f * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
    //model = glm::rotate(model,timePast * .5f * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
    glUniform1i(uniTexID1, 3); //Set texture ID to use
    glUniform1i(uniOutline, 0); //Set outline to off
    //uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
    //glUniform3f(uniColor, 1.0f, 1.0f, 0.0f);    // This changes the color of the model with -1 texture
    glUniformMatrix4fv(uniModel1, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, numVerts1, numVerts2); //(Primitive Type, Start Vertex, End Vertex)

}

void drawLevel(int shaderProgram, int numVerts1, int numVerts2) {
    /// Very slow FPS - Do not use this function

    GLint uniColor1 = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(0.5,0.5,0.5);
    glUniform3fv(uniColor1, 1, glm::value_ptr(colVec));
    GLint uniTexID1 = glGetUniformLocation(shaderProgram, "texID");

    glm::mat4 model;
    GLint uniModel1 = glGetUniformLocation(shaderProgram, "model");
    /// Draw Ground Portion
    int x, z;
    for(x = -(level->xWidth/2); x < (level->xWidth/2); x++) {
        model = glm::translate(model,glm::vec3(0.0f + x, -1.0f, 0.0f));
        for(z = -(level->zWidth/2); z < (level->zWidth/2); z++) {
            //model = glm::scale(model,glm::vec3(1.0f, 1.0f, 1.0f));
            model = glm::translate(model,glm::vec3(0.0f, 0.0f, 0.0f + z));
            uniModel1 = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(uniModel1, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(uniTexID1, 3); //Set texture ID to use
            glUniformMatrix4fv(uniModel1, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, numVerts1, numVerts2); //(Primitive Type, Start Vertex, End Vertex)
            model = glm::translate(model,glm::vec3(0.0f, 0.0f, 0.0f - z));
        }
        model = glm::translate(model,glm::vec3(0.0f - x, 1.0f, 0.0f));
    }

   /* model = glm::scale(model,glm::vec3(1000.0f, 0.25f, 1000.0f));
    model = glm::translate(model,glm::vec3(0.0f, -1.0f, 0.0f));   // Draws relative to the camera...
    uniModel1 = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel1, 1, GL_FALSE, glm::value_ptr(model));
    //model = glm::rotate(model,timePast * .5f * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
    //model = glm::rotate(model,timePast * .5f * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
    glUniform1i(uniTexID1, 3); //Set texture ID to use
    //uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
    //glUniform3f(uniColor, 1.0f, 1.0f, 0.0f);    // This changes the color of the model with -1 texture
    glUniformMatrix4fv(uniModel1, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, numVerts1, numVerts2); //(Primitive Type, Start Vertex, End Vertex)
*/
}

void drawCubeFriend(int shaderProgram, int numVerts1, int numVerts2){

	/*GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(0.5,0.5,0.5);
    glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
    //glm::mat4 model;
    //GLint uniModel = glGetUniformLocation(shaderProgram, "model");
*/
    /// Draw Cubie
    //glm::mat4 model;
    //GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    //model = glm::scale(model,glm::vec3(1.0f, 0.8f, 1.1f));
    //model = glm::translate(model,glm::vec3(0.0f, 0.0f, 0.0f));   // Draws relative to the camera...
    //uniModel = glGetUniformLocation(shaderProgram, "model");
    //glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    //model = glm::rotate(model,timePast * .5f * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
    //model = glm::rotate(model,timePast * .5f * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
    //glUniform1i(uniTexID, 0); //Set texture ID to use
    //uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
    //glUniform3f(uniColor, 1.0f, 1.0f, 0.0f);    // This changes the color of the model with -1 texture
    //glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    //glDrawArrays(GL_TRIANGLES, numVerts1, numVerts2); //(Primitive Type, Start Vertex, End Vertex)

}

//Model access implementation
void setModel(string fileName, string modelName) {
    //Reading the model
    fstream modelFile;
    modelFile.open("models/" + fileName);
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
