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
    supercollider.setup("192.168.1.111", 57110);
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

    capture.captureNewFrame(displayImages[0],  readImages[0]);
    while(!capture.isFrameNew());
    //displayImages[0].data = capture.getFrame().data;
    textures[0] = LoadTextureFromImage(displayImages[0]);
    // Main loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
	if(!isCapturing)
        	controls.readValues();

        capture.update();
        if(capture.isFrameNew()){
		       if(controls.isCapturePressed()){
            std::cout << "Begin Capture" << std::endl;
            capture.captureNewFrame(displayImages[0], readImages[0]);
            loadNextImage = true;
	    isCapturing = true;
	    }
        }
        
        
       if(!controls.isFreezePressed()) 
        	currentPosition += (GetFrameTime() * controls.getSpeed() * 100);
       
        if(currentPosition >= screenWidth) currentPosition -= screenWidth;
        
        currentScaleSize = supercollider.getScaleSize(controls.getScale());
        
        if(capture.isFrameNew() && loadNextImage){
            //images[0].data = (void *)(capture.getFrame().data);
            std::cout << "Load Image" << std::endl;
            UnloadTexture(textures[0]);
            textures[0] = LoadTextureFromImage(displayImages[0]);
            loadNextImage = false;
	    isCapturing = false;
        }

        if(IsKeyPressed(KEY_D)){
            UnloadTexture(textures[0]);
            textures[0] = LoadTextureFromImage(readImages[0]);
        }
        
        liveReader.update(readImages[0], 720, (int)currentPosition);
        supercollider.setScale(controls.getScale(), controls.getTranspose());
        supercollider.sendAmps(liveReader.getValues(), 360);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(WHITE);
        //textures[0] = LoadTextureFromImage(images[0]);
        DrawTextureEx(textures[0], (Vector2){0, 0}, 0.0f, 1.0f, WHITE);
        liveReader.draw(720);//supercollider.getScaleSize(controls.getScale()));
        capture.drawGui();
        controls.drawGui();
        
        
        //TODO: Better mouse drawing if in linux
#if __linux__
        DrawCircle(GetMousePosition().x, GetMousePosition().y, 10, BLACK);
#endif
        DrawFPS(screenWidth - 100, screenHeight - 30);
        EndDrawing();
        //std::cout << "Loop " << GetFPS() << std::endl;
        //----------------------------------------------------------------------------------
    }
    
    supercollider.close();

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
