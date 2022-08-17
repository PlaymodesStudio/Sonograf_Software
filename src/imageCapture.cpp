//
//  imageCapture.cpp
//  Sonograf
//
//  Created by Eduard Frigola Bagué on 1/8/22.
//

#include "imageCapture.h"
#include "imgui.h"
#include "ofxCv.h"

void imageCapture::setup(){
    videoGrabber.setDeviceID(0);
    videoGrabber.setDesiredFrameRate(30);
    videoGrabber.initGrabber(1280, 720);
    
    anchorPoints.resize(4);
    anchorPoints[0] = glm::vec2(10, 10);
    anchorPoints[1] = glm::vec2(1200, 10);
    anchorPoints[2] = glm::vec2(1200, 700);
    anchorPoints[3] = glm::vec2(10, 700);

    unwarped.allocate(1280, 720, OF_IMAGE_COLOR);
    calibrate = false;
}

void imageCapture::update(){
    videoGrabber.update();
    std::vector<cv::Point2f> warpPoints = ofxCv::toCv(anchorPoints);
    ofxCv::unwarpPerspective(videoGrabber, unwarped, warpPoints);
    unwarped.update();
}

void imageCapture::draw(float x, float y, float w, float h){
    if(w == -1 || h == -1){
        if(!calibrate){
            unwarped.draw(x, y);
        }else{
            videoGrabber.draw(x, y);
        }
    }else{
        videoGrabber.draw(x, y, w, h);
    }
}

void imageCapture::drawGui(){
    if(ImGui::Begin("Sonograf")){
        ImGui::Separator();
        ImGui::Checkbox("Calibrate", &calibrate);
    }
    ImGui::End();
    
    
    if(calibrate){
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
    bool someClick = false;
    for(int i = 0; i < 4; i++){
        auto bulletPosition = anchorPoints[i];
        auto mouseToBulletDistance = glm::distance(glm::vec2(ImGui::GetIO().MousePos), bulletPosition);
        auto bulletSize = 10;
        ImU32 color = IM_COL32(0, 255, 0, 255);
        if(i == anchorSelectIdx) color = IM_COL32(255, 0, 0, 255);
        
        draw_list->AddCircle(bulletPosition, bulletSize, color);
        draw_list->AddLine(bulletPosition - ImVec2(bulletSize, 0), bulletPosition + ImVec2(bulletSize, 0), color);
        draw_list->AddLine(bulletPosition - ImVec2(0, bulletSize), bulletPosition + ImVec2(0, bulletSize), color);
        if(mouseToBulletDistance < 10){
            if(ImGui::IsMouseClicked(0)){
                anchorDragIdx = i;
                anchorSelectIdx = i;
                someClick = true;
            }
        }
    }
    if(ImGui::IsMouseClicked(0) && !someClick){
        anchorSelectIdx = -1;
    }
    if(ImGui::IsMouseReleased(0)) anchorDragIdx = -1;
    
    if(anchorDragIdx != -1){
        anchorPoints[anchorDragIdx] += glm::vec2(ImGui::GetIO().MouseDelta);
    }
    
    if(anchorSelectIdx != -1){
        if(ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) anchorPoints[anchorSelectIdx] += glm::vec2(-1, 0);
        if(ImGui::IsKeyPressed(ImGuiKey_RightArrow)) anchorPoints[anchorSelectIdx] += glm::vec2(1, 0);
        if(ImGui::IsKeyPressed(ImGuiKey_UpArrow)) anchorPoints[anchorSelectIdx] += glm::vec2(0, -1);
        if(ImGui::IsKeyPressed(ImGuiKey_DownArrow)) anchorPoints[anchorSelectIdx] += glm::vec2(0, 1);
    }
    }
}

ofPixels& imageCapture::getPixels(){
    return unwarped.getPixels();
}
