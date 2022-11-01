/*******************************************************************************************
 *
 *   raylib [core] example - Basic window
 *
 *   Welcome to raylib!
 *
 *   To test examples, just press F6 and execute raylib_compile_execute script
 *   Note that compiled executable is placed in the same folder as .c file
 *
 *   You can find all basic examples on C:\raylib\raylib\examples folder or
 *   raylib official webpage: www.raylib.com
 *
 *   Enjoy using raylib. :)
 *
 *   This example has been created using raylib 1.0 (www.raylib.com)
 *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
 *
 *   Copyright (c) 2013-2016 Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

#include "raylib.h"

#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "physicalControls.h"

#include "imageCapture.h"
#include "imageRead.h"
#include "scManager.h"
#include <thread>
#include <chrono>

#define NUM_IMAGES 3
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 1280;
    int screenHeight = 720;
    int dynamicLinePos = screenWidth / 3;

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Sonograf");
    SetTargetFPS(60);

    SetTargetFPS(60);
    EnableCursor();
    SetMouseCursor(4);
    SetTraceLogLevel(100);
    //--------------------------------------------------------------------------------------
    imageCapture capture(screenWidth, screenHeight);
    capture.setup();

    imageRead liveReader;
    
    scManager supercollider;
    //supercollider.setup("192.168.1.121", 57110);
    supercollider.setup("127.0.0.1", 57110);
    supercollider.initialize();

    //i2c
    physicalControls controls;
    
    //Variables
    int currentScaleSize = 720;
    float currentPosition = 0;
    Image displayImages[NUM_IMAGES];
    Image readImages[NUM_IMAGES];
    Texture2D textures[NUM_IMAGES];
    bool loadNextImage = false;
    bool isCapturing = false;
    bool calibrate = true;
    size_t numImages = 3;
    int currentImage = 0;
    int nextImage = 1;
    int standbyImage = 2;
    bool imageJump = false;
    
    enum playMode{
        playMode_static,
        playMode_dynamic
    };
    
    playMode mode = playMode_static;

    for(int i = 0; i < NUM_IMAGES; i++){
        displayImages[i].width = screenWidth;
        displayImages[i].height = screenHeight;
        displayImages[i].format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
        displayImages[i].mipmaps = 1;
        
        readImages[i].width = screenWidth;
        readImages[i].height = screenHeight;
        readImages[i].format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
        readImages[i].mipmaps = 1;
    }
        
    

    Image image = LoadImage("../assets/splash.png");
    Texture2D splashscreen = LoadTextureFromImage(image);
    UnloadImage(image);

    BeginDrawing();
    DrawTexture(splashscreen, 0, 0, WHITE);
    EndDrawing();

    capture.setCalibrate(calibrate);
    capture.captureNewFrame(displayImages[0],  readImages[0]);
    while(!capture.isFrameNew());
    textures[0] = LoadTextureFromImage(displayImages[0]);


//Wait splashScreen
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    // Main loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
	    controls.readValues();

        //Mode changed
        if((int)mode != controls.getMode()){
            mode = (playMode)controls.getMode();
        }

	    if (IsMouseButtonPressed(1))
	    {
		    calibrate = !calibrate;
		    capture.setCalibrate(calibrate);
		    if(capture.isFrameNew()){
                if(mode == playMode_static)
                    capture.captureNewFrame(displayImages[0], readImages[0]);
                else
                    capture.captureNewFrame(displayImages[nextImage], readImages[nextImage]);
			    loadNextImage = true;
			    isCapturing = true;
		    }
		    float zeros[360] = {0};
		    supercollider.sendAmps(zeros, 360);
	    }

	    capture.update();
	    if(capture.isFrameNew()){
		    if(controls.isCapturePressed()){
                if(mode == playMode_static){
                    capture.captureNewFrame(displayImages[0], readImages[0]);
                }else
                    capture.captureNewFrame(displayImages[standbyImage], readImages[standbyImage]);
			    loadNextImage = true;
			    isCapturing = true;
		    }
	    }

       if(!calibrate){ 
           if(!controls.isFreezePressed())
               currentPosition += (GetFrameTime() * controls.getSpeed() * 100);
           
           if(currentPosition >= screenWidth){
               currentPosition -= screenWidth;
               imageJump = false;
           //}
           //if(!imageJump && currentPosition >= dynamicLinePos){
               currentImage = nextImage;
               nextImage = standbyImage;
               standbyImage = (standbyImage+1) % NUM_IMAGES;
               imageJump = true;
           }
           
           currentScaleSize = supercollider.getScaleSize(controls.getScale());
           
           if(mode == playMode_static)
               liveReader.update(readImages[0], currentScaleSize, (int)currentPosition);
           else{
               int shiftedPosition = currentPosition + dynamicLinePos;
               if(shiftedPosition >= screenWidth)
                   liveReader.update(readImages[nextImage], currentScaleSize, (int)shiftedPosition - screenWidth);
               else
                   liveReader.update(readImages[currentImage], currentScaleSize, (int)shiftedPosition);
           }
           supercollider.setScale(controls.getScale(), controls.getTranspose());
           supercollider.sendAmps(liveReader.getValues(), 360);
       }
        if(capture.isFrameNew() && loadNextImage){
            if(mode == playMode_static){
                UnloadTexture(textures[0]);
                
                textures[0] = LoadTextureFromImage(displayImages[0]);
            }else{
                UnloadTexture(textures[standbyImage]);
                textures[standbyImage] = LoadTextureFromImage(displayImages[standbyImage]);
            }
            loadNextImage = false;
            isCapturing = false;
        }
        
        if(IsKeyPressed(KEY_D)){
            UnloadTexture(textures[0]);
            textures[0] = LoadTextureFromImage(readImages[0]);
        }
        
        // Draw
        
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
        ClearBackground(WHITE);
        if(mode == playMode_static){
            DrawTextureEx(textures[0], (Vector2){0, 0}, 0.0f, 1.0f, WHITE);
        }else{
            DrawTextureEx(textures[currentImage], (Vector2){/*dynamicLinePos*/-currentPosition, 0}, 0.0f, 1.0f, WHITE);
            DrawTextureEx(textures[nextImage], (Vector2){/*dynamicLinePos + */screenWidth - currentPosition, 0}, 0.0f, 1.0f, WHITE);
            DrawTextureEx(textures[0], (Vector2){700, 600}, 0.0f, 0.2f, LIGHTGRAY);
            DrawTextureEx(textures[1], (Vector2){900, 600}, 0.0f, 0.2f, LIGHTGRAY);
            DrawTextureEx(textures[2], (Vector2){1100, 600}, 0.0f, 0.2f, LIGHTGRAY);
        }
        if(!calibrate){
            liveReader.draw(currentScaleSize, (int)mode);
        }
        capture.drawGui();
        controls.drawGui();
        
        
        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    
    supercollider.close();

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
