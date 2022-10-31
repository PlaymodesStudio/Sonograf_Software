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

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 1280;
    int screenHeight = 720;

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
    std::vector<Image> displayImages;
    std::vector<Image> readImages;
    Texture2D textures[2];
    bool loadNextImage = false;
    bool isCapturing = false;
    bool calibrate = true;

    displayImages.resize(1);
    displayImages[0].width = screenWidth;
    displayImages[0].height = screenHeight;
    displayImages[0].format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
    displayImages[0].mipmaps = 1;

    readImages.resize(1);
    readImages[0].width = screenWidth;
    readImages[0].height = screenHeight;
    readImages[0].format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
    readImages[0].mipmaps = 1;

    

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


	    if (IsMouseButtonPressed(1))
	    {
		    calibrate = !calibrate;
		    capture.setCalibrate(calibrate);
		    if(capture.isFrameNew()){
			    capture.captureNewFrame(displayImages[0], readImages[0]);
			    loadNextImage = true;
			    isCapturing = true;
		    }
		    float zeros[360] = {0};
		    supercollider.sendAmps(zeros, 360);
	    }

	    capture.update();
	    if(capture.isFrameNew()){
		    if(controls.isCapturePressed()){
			    capture.captureNewFrame(displayImages[0], readImages[0]);
			    loadNextImage = true;
			    isCapturing = true;
		    }
	    }

       if(!calibrate){ 
       if(!controls.isFreezePressed()) 
        	currentPosition += (GetFrameTime() * controls.getSpeed() * 100);
       
        if(currentPosition >= screenWidth) currentPosition -= screenWidth;
        
        currentScaleSize = supercollider.getScaleSize(controls.getScale());
        
        liveReader.update(readImages[0], currentScaleSize, (int)currentPosition);
        supercollider.setScale(controls.getScale(), controls.getTranspose());
        supercollider.sendAmps(liveReader.getValues(), 360);
       }
        if(capture.isFrameNew() && loadNextImage){
            //images[0].data = (void *)(capture.getFrame().data);
            UnloadTexture(textures[0]);
            textures[0] = LoadTextureFromImage(displayImages[0]);
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
        DrawTextureEx(textures[0], (Vector2){0, 0}, 0.0f, 1.0f, WHITE);
	if(!calibrate){
        	liveReader.draw(currentScaleSize);
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
