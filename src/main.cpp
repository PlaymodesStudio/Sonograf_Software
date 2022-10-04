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
    liveReader.setup(screenWidth, screenHeight);
    
    scManager supercollider;
    supercollider.setup("127.0.0.1", 57110);
    supercollider.initialize();

    //i2c
    physicalControls controls;
    
    //Variables
    int currentScaleSize = 720;

    // Main loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        controls.readValues();
        
        currentScaleSize = supercollider.getScaleSize(controls.getScale());
        
        liveReader.update(capture.update(), currentScaleSize, 0);
        
        supercollider.setScale(controls.getScale(), controls.getTranspose());
        supercollider.sendAmps(liveReader.getValues(), currentScaleSize);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);
        liveReader.draw(supercollider.getScaleSize(controls.getScale()));
        capture.drawGui();
        controls.drawGui();
        
        //TODO: Better mouse drawing if in linux
#if __linux__
        DrawCircle(GetMousePosition().x, GetMousePosition().y, 10, WHITE);
#endif
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
