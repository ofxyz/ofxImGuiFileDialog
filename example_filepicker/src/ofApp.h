#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "ofxImGuiFileDialog.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;

private:
    void drawMenuBar();
    void drawFilePanel();
    void drawPreviewPanel();
    void drawDialogs();
    void loadPreview(const std::string& path);

    ofxImGui::Gui gui;

    // Dialog state
    std::string openedFilePath;
    std::string savedFilePath;
    std::string statusMessage = "No file selected.";

    // Preview
    ofImage previewImage;
    bool hasPreview = false;
    bool isImageFile(const std::string& path);

    // History
    std::vector<std::string> recentFiles;
    static constexpr int MAX_RECENT = 8;

    // UI state
    bool showDemoWindow = false;
};
