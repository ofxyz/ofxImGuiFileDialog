#include "ofMain.h"
#include "ofApp.h"

int main() {
    ofGLFWWindowSettings settings;
    settings.setSize(1180, 720);
    settings.windowMode = OF_WINDOW;
    settings.title = "ofxImGuiFileDialog — File Picker Demo";
    settings.resizable = true;
    auto window = ofCreateWindow(settings);
    ofRunApp(window, std::make_shared<ofApp>());
    ofRunMainLoop();
}
