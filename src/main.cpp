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
#include <filesystem>

#ifdef __APPLE__
    #include <mach-o/dyld.h>       /* _NSGetExecutablePath */
    #include <limits.h>        /* PATH_MAX */
#endif

#define NUM_IMAGES 3

//Storage code from: https://github.com/raysan5/raylib/blob/master/examples/core/core_storage_values.c
#define STORAGE_DATA_FILE   "/assets/storage.data"   // Storage file
#define SPLASH_SCREEN_FILE  "/assets/splash.png"

// NOTE: Storage positions must start with 0, directly related to file memory layout
typedef enum {
    STORAGE_POSITION_POINT_0    = 0,
    STORAGE_POSITION_POINT_1    = 1,
    STORAGE_POSITION_POINT_2    = 2,
    STORAGE_POSITION_POINT_3    = 3
} StorageData;

// Persistent storage functions
static bool SaveStorageValue(unsigned int position, int value);
static int LoadStorageValue(unsigned int position);
static std::string getCurrentExePath();

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
    cv::Mat displayMat[NUM_IMAGES];
    cv::Mat readMat[NUM_IMAGES];
    bool loadNextImage = false;
    bool isCapturing = false;
    bool calibrate = true;
    size_t numImages = 3;
    int currentImage = 0;
    int nextImage = 1;
    int standbyImage = 2;
    bool imageJump = false;
    int loadingImage = 0;
    bool paused = true;
    
    enum playMode{
        playMode_static,
        playMode_dynamic
    };
    
    playMode mode = playMode_static;
    
    cv::Mat blackDisplayImage(1280, 720, CV_8UC3, cv::Scalar(0, 0, 0));
    cv::Mat blackReadImage(1280, 720, CV_8UC3, cv::Scalar(0, 0, 0));

    for(int i = 0; i < NUM_IMAGES; i++){
        displayMat[i] = cv::Mat(1280, 720, CV_8UC3, cv::Scalar(0, 0, 0));
        readMat[i] = cv::Mat(1280, 720, CV_8UC3, cv::Scalar(0, 0, 0));
        
        displayImages[i].width = screenWidth;
        displayImages[i].height = screenHeight;
        displayImages[i].format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
        displayImages[i].mipmaps = 1;
        displayImages[i].data = (void*)displayMat[i].data;
        
        readImages[i].width = screenWidth;
        readImages[i].height = screenHeight;
        readImages[i].format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
        readImages[i].mipmaps = 1;
        readImages[i].data = (void*)readMat[i].data;
        
        textures[i] = LoadTextureFromImage(displayImages[i]);
    }
    
    Image image = LoadImage((getCurrentExePath() + SPLASH_SCREEN_FILE).c_str());
    Texture2D splashscreen = LoadTextureFromImage(image);
    UnloadImage(image);

    BeginDrawing();
    DrawTexture(splashscreen, 0, 0, WHITE);
    EndDrawing();
    
    
    capture.getAnchorPoints()[0].x = LoadStorageValue(0);
    capture.getAnchorPoints()[0].y = LoadStorageValue(1);
    capture.getAnchorPoints()[1].x = LoadStorageValue(2);
    capture.getAnchorPoints()[1].y = LoadStorageValue(3);
    capture.getAnchorPoints()[2].x = LoadStorageValue(4);
    capture.getAnchorPoints()[2].y = LoadStorageValue(5);
    capture.getAnchorPoints()[3].x = LoadStorageValue(6);
    capture.getAnchorPoints()[3].y = LoadStorageValue(7);

    capture.setCalibrate(calibrate);
    capture.captureNewFrame(displayMat[0],  readMat[0]);
    while(!capture.isFrameNew());
    displayImages[0].data = (void*)displayMat[0].data;
    readImages[0].data = (void*)readMat[0].data;
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
            for(int i = 0; i < NUM_IMAGES; i++){
                displayMat[i] = cv::Mat(1280, 720, CV_8UC3, cv::Scalar(0, 0, 0));
                readMat[i] = cv::Mat(1280, 720, CV_8UC3, cv::Scalar(0, 0, 0));
                displayImages[i].data = (void*)displayMat[i].data;
                readImages[i].data = (void*)readMat[i].data;
                textures[i] = LoadTextureFromImage(displayImages[i]);
            }
            paused = true;
            currentPosition = 0;
        }

	    if (IsMouseButtonPressed(1))
	    {
		    calibrate = !calibrate;
		    capture.setCalibrate(calibrate);
		    if(capture.isFrameNew()){
                if(mode == playMode_static)
                    loadingImage = nextImage;
                else
                    loadingImage = nextImage;
                
                capture.captureNewFrame(displayMat[loadingImage], readMat[loadingImage]);
			    loadNextImage = true;
			    isCapturing = true;
		    }
		    float zeros[360] = {0};
		    supercollider.sendAmps(zeros, 360);
            if(!calibrate){
                SaveStorageValue(0, capture.getAnchorPoints()[0].x);
                SaveStorageValue(1, capture.getAnchorPoints()[0].y);
                SaveStorageValue(2, capture.getAnchorPoints()[1].x);
                SaveStorageValue(3, capture.getAnchorPoints()[1].y);
                SaveStorageValue(4, capture.getAnchorPoints()[2].x);
                SaveStorageValue(5, capture.getAnchorPoints()[2].y);
                SaveStorageValue(6, capture.getAnchorPoints()[3].x);
                SaveStorageValue(7, capture.getAnchorPoints()[3].y);
            }
	    }

	    capture.update();
	    if(capture.isFrameNew()){
		    if(controls.isCapturePressed()){
                if(mode == playMode_static)
                    loadingImage = nextImage;
                else{
                    if(paused)
                        loadingImage = nextImage;
                    else
                        loadingImage = standbyImage;
                }
                
                capture.captureNewFrame(displayMat[loadingImage], readMat[loadingImage]);
			    loadNextImage = true;
			    isCapturing = true;
		    }
	    }

       if(!calibrate){ 
           if(!controls.isFreezePressed() && !paused)
               currentPosition += (GetFrameTime() * screenWidth) / controls.getSpeed();
           
           if(currentPosition >= screenWidth){
               currentPosition -= screenWidth;
               if(mode == playMode_dynamic){
               currentImage = nextImage;
               nextImage = standbyImage;
               standbyImage = (standbyImage+1) % NUM_IMAGES;
               }
           }
           
           currentScaleSize = supercollider.getScaleSize(controls.getScale());
           
           if(mode == playMode_static)
               liveReader.update(readImages[currentImage], currentScaleSize, (int)currentPosition);
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
            UnloadTexture(textures[loadingImage]);
            displayImages[loadingImage].data = (void*)displayMat[loadingImage].data;
            readImages[loadingImage].data = (void*)readMat[loadingImage].data;
            textures[loadingImage] = LoadTextureFromImage(displayImages[loadingImage]);
            
            if(mode == playMode_static){
                currentImage = nextImage;
                nextImage = standbyImage;
                standbyImage = (standbyImage+1) % NUM_IMAGES;
            }
            
            loadNextImage = false;
            isCapturing = false;
            paused = false;
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
            DrawTextureEx(textures[currentImage], (Vector2){0, 0}, 0.0f, 1.0f, WHITE);
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

// Save integer value to storage file (to defined position)
// NOTE: Storage positions is directly related to file memory layout (4 bytes each integer)
bool SaveStorageValue(unsigned int position, int value)
{
    bool success = false;
    unsigned int dataSize = 0;
    unsigned int newDataSize = 0;
    unsigned char *fileData = LoadFileData((getCurrentExePath() + STORAGE_DATA_FILE).c_str(), &dataSize);
    unsigned char *newFileData = NULL;

    if (fileData != NULL)
    {
        if (dataSize <= (position*sizeof(int)))
        {
            // Increase data size up to position and store value
            newDataSize = (position + 1)*sizeof(int);
            newFileData = (unsigned char *)RL_REALLOC(fileData, newDataSize);

            if (newFileData != NULL)
            {
                // RL_REALLOC succeded
                int *dataPtr = (int *)newFileData;
                dataPtr[position] = value;
            }
            else
            {
                // RL_REALLOC failed
                TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to realloc data (%u), position in bytes (%u) bigger than actual file size", STORAGE_DATA_FILE, dataSize, position*sizeof(int));

                // We store the old size of the file
                newFileData = fileData;
                newDataSize = dataSize;
            }
        }
        else
        {
            // Store the old size of the file
            newFileData = fileData;
            newDataSize = dataSize;

            // Replace value on selected position
            int *dataPtr = (int *)newFileData;
            dataPtr[position] = value;
        }

        success = SaveFileData((getCurrentExePath() + STORAGE_DATA_FILE).c_str(), newFileData, newDataSize);
        RL_FREE(newFileData);

        TraceLog(LOG_INFO, "FILEIO: [%s] Saved storage value: %i", STORAGE_DATA_FILE, value);
    }
    else
    {
        TraceLog(LOG_INFO, "FILEIO: [%s] File created successfully", STORAGE_DATA_FILE);

        dataSize = (position + 1)*sizeof(int);
        fileData = (unsigned char *)RL_MALLOC(dataSize);
        int *dataPtr = (int *)fileData;
        dataPtr[position] = value;

        success = SaveFileData((getCurrentExePath() + STORAGE_DATA_FILE).c_str(), fileData, dataSize);
        UnloadFileData(fileData);

        TraceLog(LOG_INFO, "FILEIO: [%s] Saved storage value: %i", STORAGE_DATA_FILE, value);
    }

    return success;
}

// Load integer value from storage file (from defined position)
// NOTE: If requested position could not be found, value 0 is returned
int LoadStorageValue(unsigned int position)
{
    int value = 0;
    unsigned int dataSize = 0;
    unsigned char *fileData = LoadFileData((getCurrentExePath() + STORAGE_DATA_FILE).c_str(), &dataSize);

    if (fileData != NULL)
    {
        if (dataSize < (position*4)) TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to find storage position: %i", STORAGE_DATA_FILE, position);
        else
        {
            int *dataPtr = (int *)fileData;
            value = dataPtr[position];
        }

        UnloadFileData(fileData);

        TraceLog(LOG_INFO, "FILEIO: [%s] Loaded storage value: %i", STORAGE_DATA_FILE, value);
    }

    return value;
}

//From OpenFrameworks / ofFileUtils.cpp
std::string getCurrentExePath(){
#if defined(__APPLE__)
    char path[FILENAME_MAX];
    uint32_t size = sizeof(path);
    if(_NSGetExecutablePath(path, &size) != 0){
        std::cout << "getCurrentExePath(): path buffer too small, need size " <<  size << std::endl;
    }
    return std::filesystem::path(path).parent_path().string() + "/../..";
#elif defined(__linux__)
    char buff[FILENAME_MAX];
    std::filesystem::path p = std::filesystem::canonical("/proc/self/exe");
    return p.parent_path().string() + "/..";
#endif
return "";
}
