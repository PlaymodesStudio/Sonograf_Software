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

    InitWindow(screenWidth, screenHeight, "Sonograf");

    SetTargetFPS(60);
    EnableCursor();
    SetMouseCursor(4);
    SetTraceLogLevel(100);
    //--------------------------------------------------------------------------------------
    imageCapture capture(screenWidth, screenHeight);
    capture.setup();

    imageRead liveReader;
    
    scManager supercollider;
    supercollider.setup("127.0.0.1", 57110);
    supercollider.initialize();

    //i2c
    physicalControls controls;
    
    //Variables
    int currentScaleSize = 720;
    float currentPosition = 0;
    std::vector<Image> images;
    Texture2D textures[2];

    images.resize(1);
    images[0].width = screenWidth;
    images[0].height = screenHeight;
    images[0].format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
    images[0].mipmaps = 1;
    capture.captureNewFrame();
    while(!capture.isFrameNew());
    images[0].data = capture.getFrame().data;
    // Main loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if(capture.isFrameNew()){
            capture.captureNewFrame();
        }
        
        
        controls.readValues();
        
        currentPosition += controls.getSpeed();
        if(currentPosition >= screenWidth) currentPosition -= screenWidth;
        
        currentScaleSize = supercollider.getScaleSize(controls.getScale());
        
        if(capture.isFrameNew()){
            images[0].data = (void *)(capture.getFrame().data);
        }
        
        liveReader.update(images[0], currentScaleSize, (int)currentPosition);
        
        supercollider.setScale(controls.getScale(), controls.getTranspose());
        supercollider.sendAmps(liveReader.getValues(), currentScaleSize);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(WHITE);
        textures[0] = LoadTextureFromImage(images[0]);
        DrawTextureEx(textures[0], (Vector2){0, 0}, 0.0f, 1.0f, WHITE);
        liveReader.draw(supercollider.getScaleSize(controls.getScale()));
        capture.drawGui();
        controls.drawGui();
        
        
        
        //TODO: Better mouse drawing if in linux
#if __linux__
        DrawCircle(GetMousePosition().x, GetMousePosition().y, 10, WHITE);
#endif
        DrawFPS(screenWidth - 30, screenHeight - 30);
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
