#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "ofxImGuiFileDialog.h"
#include "ImTheme.h"
#include "ImThemeRegistry.h"
#include "ImFonts.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;

private:
    void drawDockspace();
    void buildDefaultDockLayout(ImGuiID dockspaceId);
    void drawMenuBar();
    void drawStatusBar();
    void drawFilePanel();
    void drawPreviewPanel();
    void drawDialogs();
    void openFileDialog();
    void openSaveDialog();
    void loadPreview(const std::string& path);
    bool isImageFile(const std::string& path);

    ofxImGui::Gui gui;

    std::string currentThemeId = "DarculaDarker";
    float uiScale = 1.0f;
    bool defaultDockLayoutBuilt = false;

    std::string openedFilePath;
    std::string savedFilePath;
    std::string statusMessage = "Pick a file to get started.";

    ofImage previewImage;
    bool hasPreview = false;
    float previewZoom = 1.0f;

    std::vector<std::string> recentFiles;
    static constexpr int MAX_RECENT = 8;

    bool showDemoWindow = false;
    bool showThemePanel = false;
};
