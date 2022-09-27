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
#include "rlgl.h"

#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "opencv2/opencv.hpp"

#if __linux__
    #include "i2c.h"
#endif

#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"

#define ADDRESS "192.168.1.117"
#define PORT 7000

#define OUTPUT_BUFFER_SIZE 1024

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 1280;
    int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);
    EnableCursor();
    SetMouseCursor(4);
    SetTraceLogLevel(100);
    //--------------------------------------------------------------------------------------
    
    //--- INITIALIZE VIDEOCAPTURE
    cv::VideoCapture capture;
    int deviceID = 0;        // 0 = open default camera
    #if __APPLE__
        int apiID = cv::CAP_ANY; // 0 = autodetect default API
    #else
        int apiID = cv::CAP_V4L2; //RPI
    #endif
    capture.open(deviceID, apiID);
    if (!capture.isOpened())
    {
        std::cout << "ERROR! Unable to open camera\n";
        return -1;
    }
    capture.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, 720);

    Texture2D texture;

    float scrolling = 0;
    bool flip = true;
    int sliderValue = 0;
    bool drawWarp = false;

    Vector2 drag;
    Vector2 prevMouse{0, 0};

    std::vector<Vector2> anchorPoints(4);
    anchorPoints[0] = (Vector2){100, 100};
    anchorPoints[1] = (Vector2){1220, 100};
    anchorPoints[2] = (Vector2){1220, 680};
    anchorPoints[3] = (Vector2){100, 680};

    std::vector<cv::Point2f> srcPoints(4);
    srcPoints[0] = cv::Point2f(100, 100);
    srcPoints[1] = cv::Point2f(1820, 100);
    srcPoints[2] = cv::Point2f(1820, 980);
    srcPoints[3] = cv::Point2f(100, 980);

    int anchorSelectIdx = -1;
    int anchorDragIdx = -1;

    std::vector<cv::Point2f> dstPoints(4);
    dstPoints[0] = cv::Point2f(0, 0);
    dstPoints[1] = cv::Point2f(1280, 0);
    dstPoints[2] = cv::Point2f(1280, 720);
    dstPoints[3] = cv::Point2f(0, 720);

    //Networking
    UdpTransmitSocket transmitSocket( IpEndpointName( ADDRESS, PORT ) );
    
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
    
    p << osc::BeginBundleImmediate
        << osc::BeginMessage( "/test1" ) 
            << true << 23 << (float)3.1415 << "hello" << osc::EndMessage
        << osc::BeginMessage( "/test2" ) 
            << true << 24 << (float)10.8 << "world" << osc::EndMessage
        << osc::EndBundle;
    
    transmitSocket.Send( p.Data(), p.Size() );


    //i2c
    #if __linux__
    I2c * bus;
    bus = new I2c("/dev/i2c-1");
    bus->addressSet(0x08);
    #endif

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        drag = Vector2Subtract(GetMousePosition(), prevMouse);
	    prevMouse = GetMousePosition();

        scrolling += 10;
        if (scrolling > 1280)
        {
            scrolling = 0;
        }

        cv::Mat frame;
        cv::Mat framergb;
        cv::Mat out;
        Image image_raylib;

        capture.read(frame);

        cv::cvtColor(frame, framergb, cv::COLOR_BGR2RGB);

        for(int i = 0; i < anchorPoints.size(); i++) {
			srcPoints[i].x = anchorPoints[i].x;
			srcPoints[i].y = anchorPoints[i].y;
		}
        cv::Mat transform = getPerspectiveTransform(&srcPoints[0], &dstPoints[0]);
        warpPerspective(framergb, out, transform, out.size(), cv::INTER_LINEAR);

        if (GetKeyPressed() == ' ')
        {
            drawWarp = !drawWarp;
        }

        bool someClick = false;
        for (int i = 0; i < 4; i++)
        {
            auto bulletPosition = anchorPoints[i];
            auto mouseToBulletDistance = Vector2Distance(GetMousePosition(), bulletPosition);
            auto bulletSize = 20;
            if (mouseToBulletDistance < bulletSize)
            {
                if (IsMouseButtonPressed(0))
                {
                    anchorDragIdx = i;
                    anchorSelectIdx = i;
                    someClick = true;
                }
            }
        }
        if (IsMouseButtonPressed(0) && !someClick)
        {
            anchorSelectIdx = -1;
        }
        if (IsMouseButtonReleased(0))
            anchorDragIdx = -1;

        if (anchorDragIdx != -1)
        {
            anchorPoints[anchorDragIdx] = Vector2Add(anchorPoints[anchorDragIdx], drag);
        }

        if (anchorSelectIdx != -1)
        {
            if (IsKeyDown(KEY_LEFT))
                anchorPoints[anchorSelectIdx] = Vector2Add(anchorPoints[anchorSelectIdx], (Vector2){-1.0f, 0.0f});
            if (IsKeyDown(KEY_RIGHT))
                anchorPoints[anchorSelectIdx] = Vector2Add(anchorPoints[anchorSelectIdx], (Vector2){1.0f, 0.0f});
            if (IsKeyDown(KEY_UP))
                anchorPoints[anchorSelectIdx] = Vector2Add(anchorPoints[anchorSelectIdx], (Vector2){0.0f, -1.0f});
            if (IsKeyDown(KEY_DOWN))
                anchorPoints[anchorSelectIdx] = Vector2Add(anchorPoints[anchorSelectIdx], (Vector2){0.0f, 1.0f});
        }

        if (drawWarp)
        {
            image_raylib.width = out.cols;
            image_raylib.height = out.rows;
            image_raylib.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
            image_raylib.mipmaps = 1;
            image_raylib.data = (void *)(out.data);
        }
        else
        {
            image_raylib.width = framergb.cols;
            image_raylib.height = framergb.rows;
            image_raylib.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
            image_raylib.mipmaps = 1;
            image_raylib.data = (void *)(framergb.data);
        }

        texture = LoadTextureFromImage(image_raylib);

        Vector2 points[720];
        float vals[720];

        for (int i = 0; i < 720; i++)
        {
            vals[i] = GetImageColor(image_raylib, scrolling, i).r;
            points[i].x = 960 + GetImageColor(image_raylib, scrolling, i).r;
            points[i].y = i;
        }

	//i2c
    #if __linux__
	std::vector<uint8_t> data;
	data.resize(12);
	for(int i = 0; i < 6; i++){
				bus->writeByte(0x20+i, 0x01);
				bus->readBlock(0x20+i, 2, data.data() + (i*2));
			}
	std::cout << (data[1]<<8|data[0]) << " - " <<(data[3]<<8|data[2]) << " - " << (data[5]<<8|data[4]) << " - " << (data[7]<<8|data[6]) << " - " << (data[9]<<8|data[8]) << " - " << (data[11]<<8|data[10]) << std::endl;
    #endif

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        //DrawTextureEx(texture, (Vector2){0, 0}, 0.0f, 1.0f, WHITE);
        DrawTextureEx(texture, (Vector2){(float)screenWidth, (float)screenHeight}, 180.0f, 1.0f, WHITE);

        rlSetLineWidth(3);
        DrawLineStrip(points, 720, WHITE);

        // Gui Warping
        if (!drawWarp)
        {
            for (int i = 0; i < 4; i++)
            {
                Color color = GREEN;
                if (i == anchorSelectIdx)
                    color = RED;

                DrawCircle(anchorPoints[i].x, anchorPoints[i].y, 20, color);
            }
            //DrawCircle(GetMousePosition().x, GetMousePosition().y, 10, WHITE);
        }

        EndDrawing();
        UnloadTexture(texture);
        //----------------------------------------------------------------------------------
    }

    //UnloadTexture(image1);
    //UnloadTexture(image2);
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
