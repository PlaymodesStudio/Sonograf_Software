#include "ofApp.h"
#include "ofAppRunner.h"
#include "ofPolyline.h"
#include "ofGraphics.h"

//--------------------------------------------------------------
void ofApp::setup(){
	gui.setup(nullptr, false);
	
	sender.setup("127.0.0.1", 57110);
//	sender.setup("192.168.1.122", 57110);
	
	//Create Supercollider "world"
	ofxOscMessage m2;
	m2.setAddress("/g_new");
	m2.addIntArg(1);
	m2.addIntArg(0);
	m2.addIntArg(0);
	sender.sendMessage(m2);

	//Create Synth	
	ofxOscMessage m;

	int nodeID = 2000; 
	int position = 0;
	int groupID = 1;

	m.setAddress("/s_new");
	m.addStringArg("KlangTest360_2");
	m.addIntArg(nodeID);
	m.addIntArg(position);
	m.addIntArg(groupID);
	sender.sendMessage(m);

	//Set frequency for synth
	ofxOscMessage m3;
	m3.setAddress("/n_setn");
	m3.addIntArg(2000); //Node
	m3.addStringArg("freq"); //Parameter
	m3.addIntArg(360); //Size
	//m3.addIntArg(240); //Size
	for(int i = 0; i < 360; i++){
		m3.addFloatArg(ofMap(i, 0, 359, 108, 48));
	}
	sender.sendMessage(m3);

    capture.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
	currentPosition++;
    capture.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    gui.begin();
    
    if(ImGui::Begin("Sonograf", &p_open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)){
        
        ImGui::Text("Benvinguts al Sonograf");
        ImGui::SliderInt("Image", &currentImage, 0, 5);
        ImGui::Separator();
        static float threshold = 0.3;
        static float slope = 0.3;
        static float mult = 1;
        ImGui::Text("Audio Params");
        if(ImGui::SliderFloat("Threshold", &threshold, 0, 1)){
            ofxOscMessage m;
            m.setAddress("/n_set");
            m.addIntArg(2000);
            m.addStringArg("threshold");
            m.addFloatArg(threshold);
            sender.sendMessage(m);
        }
        if(ImGui::SliderFloat("Slope", &slope, 0, 1)){
            ofxOscMessage m;
            m.setAddress("/n_set");
            m.addIntArg(2000);
            m.addStringArg("slope");
            m.addFloatArg(slope);
            sender.sendMessage(m);
        }
        if(ImGui::SliderFloat("Mult", &mult, 0, 30)){
            ofxOscMessage m;
            m.setAddress("/n_set");
            m.addIntArg(2000);
            m.addStringArg("mult");
            m.addFloatArg(mult);
            sender.sendMessage(m);
        }
        ImGui::Separator();
        ImGui::Text("FPS: %f", ofGetFrameRate());
    }
    ImGui::End();
    
    capture.drawGui();
    gui.end();
    capture.draw();
    
    ofPolyline line;
    
    ofxOscMessage m;
    m.setAddress("/n_setn");
    m.addIntArg(2000); //Node
    m.addStringArg("amp"); //Parameter
    m.addIntArg(360); //Size
    float luminances[720];
    
    for(int i = 0; i < 720; i++){
        auto pixelColor = capture.getPixels().getColor(currentPosition%1280, i);
        float luminance = (0.299 * pixelColor[0]/255.0f + 0.587 * pixelColor[1]/255.0f + 0.114 * pixelColor[2]/255.0f);
        luminance = ofMap(luminance, 0.2, 0.7, 1, 0, true);
        line.addVertex(glm::vec3(currentPosition%1280 - (luminance*100/**headerWidth*opacity*/), i, 0));
        luminances[i] = luminance;
    }
    
    for(int i = 0; i < 360; i++){
        float val = 0;
        if(currentImage < 5){
            val += luminances[(i*2)];
            val += luminances[(i*2)+1];
        }
        val += luminances[(i*2)+2];
        m.addFloatArg(val / 2);
    }
    
    sender.sendMessage(m);
    
    ofPushStyle();
    ofSetColor(ofColor::red);
    ofSetLineWidth(5);
    line.draw();
    ofPopStyle();
    
    if(p_open){
        gui.draw(); // <-- In manual mode, you can choose to render imgui at a given moment in your rendering pipeline
    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key < 5){
        currentImage = key;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if(button == 2){
        p_open = !p_open;
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}
