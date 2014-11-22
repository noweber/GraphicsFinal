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
using namespace std;

bool saveOutput = false;
float timePast = 0;

//SJG: Store the object coordinates
//You should have a representation for the state of each object
float objx=0, objy=0, objz=0;
float colR=1, colG=1, colB=1;
/// Camera, object, and map variables
// Corresponds to the game map's XZ-plane
int map[14][10] =
{

    {'W','W','W','W','W','W','W','W','W','W'},
    {'W',0,0,'W','W','W','W',0,0,'W'},
    {'W',0,0,'W','W','W','W',0,0,'W'},
    {'W','W','W','W','W','W','W','W','W','W'},
    {'W','W','W','W','W','W','W','W','W','W'},
    {'W',0,0,'W','W','W','W',0,0,'W'},
    {'W','W',0,0,0,0,0,0,'W','W'},
    {'W','W','W',0,0,0,0,'W','W','W'},
    {'W','W','W','W','W','W','W','W','W','W'},
    {'W','W','W','W','W','W','W','W','W','W'},
    {'W','W','W','W','W','W','W','W','W','W'},
    {'W','W','W','W','Z','W','W','W','W','W'},
    {'W','W','W','W','Z','W','W','W','W','W'},
    {0,0,0,0,0,0,0,0,0,0},
};

int mapWidth = 14;
int mapHeight = 10;
/// TODO - make multiple maps and allow a button to load new ones

/// Camera and Position variables
int playerX = 3;    // Stores the player's x coordinate on this map
int playerZ = -1;    // Stores the player's y coordinate on this map
int playerY = 0;

float viewX = -1.0f;
float viewY = 0.0f;
float viewZ = 0.0f;

float xzRotation = 0;
int rotateCt = 0;

int playerMinX = 0; int playerMaxX = mapWidth - 1;
int playerMinZ = 0; int playerMaxZ = mapHeight - 1;

float lookX = 4.0f;
float lookY = 0.0f;
float lookZ = 4.0f;

bool isJumping = false;
bool isFalling = false;
float jumpHeight = 0.0f;
float jumpMult = 1.0f;

/// Key and Door Variables
bool openA = false;
bool openB = false;
bool openC = false;
bool openD = false;
bool openE = false;

bool aKey = false;
bool bKey = false;
bool cKey = false;
bool dKey = false;
bool eKey = false;


// For door animations
float offsetA = 0.0f;
float offsetB = 0.0f;
float offsetC = 0.0f;
float offsetD = 0.0f;
float offsetE = 0.0f;
int offsetCt = 10;

bool finA = false;
bool finB = false;
bool finC = false;
bool finD = false;
bool finE = false;

/// Goal Variables
bool hasWon = false;
bool finWin = false;
int winCt = 100;
float winOffset = 0.0f;




bool DEBUG_ON = true;
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
bool fullscreen = false;
void Win2PPM(int width, int height);

//srand(time(NULL));
float rand01(){
	return rand()/(float)RAND_MAX;
}

void drawGeometry(int shaderProgram, int numVerts1, int numVerts2);

int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)

    //Ask SDL to get a recent version of OpenGL (3 or greater)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);//

	//Create a window (offsetx, offsety, width, height, flags)
	SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, 800, 600, SDL_WINDOW_OPENGL);

	//Create a context to draw in
	SDL_GLContext context = SDL_GL_CreateContext(window);

	//GLEW loads new OpenGL functions
	glewExperimental = GL_TRUE; //Use the new way of testing which methods are supported
	glewInit();

	//Build a Vertex Array Object. This stores the VBO and attribute mappings in one object
	GLuint vao;
	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); //Bind the above created VAO to the current context


	//Load Model 1
	ifstream modelFile;
	modelFile.open("models/cube.txt");
	int numLines = 0;
	modelFile >> numLines;
	float* model1 = new float[numLines];
	for (int i = 0; i < numLines; i++){
		modelFile >> model1[i];
	}
	printf("%d\n",numLines);
	int numVerts1 = numLines/8;
	modelFile.close();

	//Load Model 2
	modelFile.open("models/knot.txt");
	numLines = 0;
	modelFile >> numLines;
	float* model2 = new float[numLines];
	for (int i = 0; i < numLines; i++){
		modelFile >> model2[i];
	}
	printf("%d\n",numLines);
	int numVerts2 = numLines/8;
	modelFile.close();

	//SJG: I load each model in a different array, then concatenate everything in one big array
	//     There are better options, but this works.
	//Concatenate model arrays
	float* modelData = new float[(numVerts1+numVerts2)*8];
	copy(model1, model1+numVerts1*8, modelData);
	copy(model2, model2+numVerts2*8, modelData+numVerts1*8);
	int totalNumVerts = numVerts1+numVerts2;


	//// Allocate Texture 0 (Wood) ///////
	SDL_Surface* surface = SDL_LoadBMP("wood.bmp");
	if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex0;
    glGenTextures(1, &tex0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);

    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);


    SDL_FreeSurface(surface);
    //// End Allocate Texture ///////


	//// Allocate Texture 1 (Brick) ///////
	SDL_Surface* surface1 = SDL_LoadBMP("brick.bmp");
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
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface1->w,surface1->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface1->pixels);


    glGenerateMipmap(GL_TEXTURE_2D);

    SDL_FreeSurface(surface1);
    //// End Allocate Texture ///////\

    //// Allocate Texture 2 (door) ///////
	SDL_Surface* surface2 = SDL_LoadBMP("door.bmp");
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
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface2->w,surface2->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface2->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);


    SDL_FreeSurface(surface2);
    //// End Allocate Texture ///////


	//Allocate memory on the graphics card to store geometry (vertex buffer object)
	GLuint vbo[1];
	glGenBuffers(1, vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	glBufferData(GL_ARRAY_BUFFER, totalNumVerts*8*sizeof(float), modelData, GL_STATIC_DRAW); //upload vertices to vbo
	//GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
	//GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used

	int texturedShader = InitShader("vertexTex.glsl", "fragmentTex.glsl");

	int phongShader = InitShader("vertex.glsl", "fragment.glsl");



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
	//Event Loop (Loop forever processing each event as fast as possible)
	bool isRunning = true;
	SDL_Event kbEvent;
    while (isRunning){

        while(SDL_PollEvent(&kbEvent) != 0) {
            switch(kbEvent.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;
                case SDL_KEYDOWN:
                    switch(kbEvent.key.keysym.sym) {
                        case SDLK_d:

                            printf("playerZ %d\n", playerZ);
                            break;
                        case SDLK_a:

                            printf("playerZ %d\n", playerZ);
                            break;

                        case SDLK_w:

                            printf("playerX %d\n", playerX);
                            break;
                        case SDLK_SPACE:

                            break;
                        case SDLK_q:

                            break;
                        /*case SDLK_e:
                            lookZ += 1;
                            printf("xRotation change\n");
                            break;*/
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

      // Clear the screen to default color
      glClearColor(.2f, 0.4f, 0.8f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUseProgram(texturedShader);


      if (!saveOutput) timePast = SDL_GetTicks()/1000.f;
      if (saveOutput) timePast += .07; //Fix framerate at 14 FPS


      /// Camera "view"
    lookX = playerX + viewX;
    lookY = viewY;
    lookZ = playerZ + viewZ;
    glm::mat4 view = glm::lookAt(
    glm::vec3(4.0f + playerX, playerY + jumpHeight*jumpMult, playerZ),  //Cam Position
    glm::vec3(lookX , (lookY + jumpHeight), (lookZ)),  //Look at point
    glm::vec3(0.0f, 1.0f, 0.0f)); //Up
    GLint uniView = glGetUniformLocation(texturedShader, "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 proj = glm::perspective(3.14f/4, 800.0f / 600.0f, 1.0f, 10.0f); //FOV, aspect, near, far
    GLint uniProj = glGetUniformLocation(texturedShader, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    glUniform1i(glGetUniformLocation(texturedShader, "tex0"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glUniform1i(glGetUniformLocation(texturedShader, "tex1"), 1);

      drawGeometry(texturedShader, numVerts1,numVerts2);


      if (saveOutput) Win2PPM(800,600);


      SDL_GL_SwapWindow(window); //Double buffering
	}

	glDeleteProgram(phongShader);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, &vao);

	//Clean Up
	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}

void drawGeometry(int shaderProgram, int numVerts1, int numVerts2){

	GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
      glm::vec3 colVec(colR,colG,colB);
      glUniform3fv(uniColor, 1, glm::value_ptr(colVec));

    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");




    // Draw the floor
    glm::mat4 model;
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    for(int x = -1; x < mapWidth + 1; x++) {
        for( int z = -1; z < mapHeight + 1; z++) {
            for(int y = 0; y < 2; y++) {

                if(y == 0) {
                    /// Draw floor
                   /* glm::mat4 model;
                    model = glm::translate(model,glm::vec3(3.0f + x, 0.0f - 1.0 + y, 0.0f + z));
                    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
                    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                    //glDrawArrays(GL_TRIANGLES, 0, numVerts2);
                    ///glDrawArrays(GL_TRIANGLES, 0, numVerts2); // draws cube
                    ///glDrawArrays(GL_TRIANGLES, numVerts1, numVerts2); // draws sphere
                    glUniform1i(uniTexID, 0); //Set texture ID to use
                    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                    glDrawArrays(GL_TRIANGLES, 0, numVerts2);*/
                }

                if(y == 1) {
                    /// Draw walls
                    /*if(map[x][z] == 'W' || x == -1 || x == mapWidth || z == -1 || z == mapHeight) {
                    /// Draw Cubes for Walls
                    glm::mat4 model;
                    model = glm::translate(model,glm::vec3(3.0f + x, 0.0f - 1.0 + y, 0.0f + z));
                    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
                    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

                    ///glDrawArrays(GL_TRIANGLES, 0, numVerts2); // draws cube
                    ///glDrawArrays(GL_TRIANGLES, numVerts1, numVerts2); // draws sphere
                    glUniform1i(uniTexID, 1); //Set texture ID to use
                    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                    glDrawArrays(GL_TRIANGLES, 0, numVerts2);*/

                }
            }
        }
    }

    // draw the knot
    model = glm::translate(model,glm::vec3(0.0f, 0.0f, 0.0f));

    uniModel = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    model = glm::rotate(model,timePast * .5f * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
            model = glm::rotate(model,timePast * .5f * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
    ///glDrawArrays(GL_TRIANGLES, 0, numVerts2); // draws cube
    ///glDrawArrays(GL_TRIANGLES, numVerts1, numVerts2); // draws sphere
    glUniform1i(uniTexID, 1); //Set texture ID to use
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, numVerts1, numVerts2);

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
