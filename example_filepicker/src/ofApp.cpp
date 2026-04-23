#include "ofApp.h"

static const ImVec4 COL_ACCENT   = ImVec4(0.24f, 0.65f, 0.88f, 1.0f);
static const ImVec4 COL_SUCCESS  = ImVec4(0.28f, 0.78f, 0.42f, 1.0f);
static const ImVec4 COL_MUTED    = ImVec4(0.55f, 0.55f, 0.55f, 1.0f);
static const ImVec4 COL_BG_DARK  = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);

// ─────────────────────────────────────────────────────────────────────────────

void ofApp::setup() {
    ofSetWindowTitle("ofxImGuiFileDialog — File Picker Demo");
    ofBackground(22, 22, 28);
    ofSetFrameRate(60);

    gui.setup();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void ofApp::update() {}

void ofApp::draw() {
    gui.begin();

    // Full-screen dockspace
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    ImGui::SetNextWindowViewport(vp->ID);
    ImGuiWindowFlags hostFlags =
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, COL_BG_DARK);
    ImGui::Begin("##Host", nullptr, hostFlags);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    drawMenuBar();

    ImGuiID dockId = ImGui::GetID("##Dock");
    ImGui::DockSpace(dockId, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();

    drawFilePanel();
    drawPreviewPanel();
    drawDialogs();

    if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);

    gui.end();
}

// ─────────────────────────────────────────────────────────────────────────────

void ofApp::drawMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open File...", "Ctrl+O")) {
                IGFD::FileDialogConfig cfg;
                cfg.path = ofToDataPath("", true);
                ImGuiFileDialog::Instance()->OpenDialog(
                    "OpenDlg", "Open File",
                    "Images{.png,.jpg,.jpeg,.gif,.bmp},All files{.*}",
                    cfg);
            }
            if (ImGui::MenuItem("Save As...", "Ctrl+S")) {
                IGFD::FileDialogConfig cfg;
                cfg.path     = ofToDataPath("", true);
                cfg.fileName = "output.png";
                ImGuiFileDialog::Instance()->OpenDialog(
                    "SaveDlg", "Save File As",
                    ".png,.jpg,.jpeg",
                    cfg);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) ofExit();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("ImGui Demo", nullptr, &showDemoWindow);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

// ─────────────────────────────────────────────────────────────────────────────

void ofApp::drawFilePanel() {
    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(20, 40), ImGuiCond_FirstUseEver);

    ImGui::Begin("File Browser");

    // ── Open button ──────────────────────────────────────────────────────────
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.16f, 0.48f, 0.70f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.24f, 0.60f, 0.88f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.10f, 0.36f, 0.58f, 1.0f));
    if (ImGui::Button("  Open File...  ", ImVec2(-1, 36))) {
        IGFD::FileDialogConfig cfg;
        cfg.path = ofToDataPath("", true);
        ImGuiFileDialog::Instance()->OpenDialog(
            "OpenDlg", "Open File",
            "Images{.png,.jpg,.jpeg,.gif,.bmp},Text{.txt,.md},All files{.*}",
            cfg);
    }
    ImGui::PopStyleColor(3);

    ImGui::Spacing();

    // ── Save button ──────────────────────────────────────────────────────────
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.16f, 0.52f, 0.28f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.22f, 0.68f, 0.38f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.10f, 0.38f, 0.20f, 1.0f));
    if (ImGui::Button("  Save File As...  ", ImVec2(-1, 36))) {
        IGFD::FileDialogConfig cfg;
        cfg.path     = ofToDataPath("", true);
        cfg.fileName = "output.png";
        ImGuiFileDialog::Instance()->OpenDialog(
            "SaveDlg", "Save File As",
            ".png,.jpg,.jpeg",
            cfg);
    }
    ImGui::PopStyleColor(3);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ── Current selection ────────────────────────────────────────────────────
    ImGui::TextColored(COL_MUTED, "Selected File");
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.14f, 0.14f, 0.18f, 1.0f));
    ImGui::BeginChild("##SelectedPath", ImVec2(-1, 52), true);
    if (openedFilePath.empty()) {
        ImGui::TextColored(COL_MUTED, "None");
    } else {
        ImGui::TextWrapped("%s", openedFilePath.c_str());
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::Spacing();

    // ── Save target ──────────────────────────────────────────────────────────
    ImGui::TextColored(COL_MUTED, "Save Target");
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.14f, 0.14f, 0.18f, 1.0f));
    ImGui::BeginChild("##SavePath", ImVec2(-1, 52), true);
    if (savedFilePath.empty()) {
        ImGui::TextColored(COL_MUTED, "None");
    } else {
        ImGui::TextColored(COL_SUCCESS, "%s", savedFilePath.c_str());
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ── Status ───────────────────────────────────────────────────────────────
    ImGui::TextColored(COL_ACCENT, "%s", statusMessage.c_str());

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ── Recent files ─────────────────────────────────────────────────────────
    if (!recentFiles.empty()) {
        ImGui::TextColored(COL_MUTED, "Recent");
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.16f, 1.0f));
        ImGui::BeginChild("##Recent", ImVec2(-1, -1), true);
        for (int i = (int)recentFiles.size() - 1; i >= 0; --i) {
            const auto& f = recentFiles[i];
            std::string label = std::filesystem::path(f).filename().string();
            ImGui::PushID(i);
            if (ImGui::Selectable(label.c_str())) {
                openedFilePath = f;
                loadPreview(f);
                statusMessage = "Loaded from recent: " + label;
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("%s", f.c_str());
            ImGui::PopID();
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    ImGui::End();
}

// ─────────────────────────────────────────────────────────────────────────────

void ofApp::drawPreviewPanel() {
    ImGui::SetNextWindowSize(ImVec2(560, 500), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(440, 40), ImGuiCond_FirstUseEver);

    ImGui::Begin("Preview");

    if (!hasPreview) {
        ImVec2 sz = ImGui::GetContentRegionAvail();
        ImGui::SetCursorPos(ImVec2(sz.x * 0.5f - 80, sz.y * 0.5f - 10));
        ImGui::TextColored(COL_MUTED, "Open an image to preview it here.");
    } else {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        float imgW   = (float)previewImage.getWidth();
        float imgH   = (float)previewImage.getHeight();
        float scale  = std::min(avail.x / imgW, avail.y / imgH);
        ImVec2 drawSz(imgW * scale, imgH * scale);
        ImVec2 cursor(
            ImGui::GetCursorPosX() + (avail.x - drawSz.x) * 0.5f,
            ImGui::GetCursorPosY() + (avail.y - drawSz.y) * 0.5f);
        ImGui::SetCursorPos(cursor);
        ImGui::Image((ImTextureID)(uintptr_t)previewImage.getTexture().getTextureData().textureID,
                     drawSz);

        // Tooltip with metadata
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%d x %d px", (int)imgW, (int)imgH);
            ImGui::Text("%s", std::filesystem::path(openedFilePath).filename().string().c_str());
            ImGui::EndTooltip();
        }
    }

    ImGui::End();
}

// ─────────────────────────────────────────────────────────────────────────────

void ofApp::drawDialogs() {
    ImVec2 minSz(600, 400);
    ImVec2 maxSz(1200, 800);

    // ── Open dialog ──────────────────────────────────────────────────────────
    if (ImGuiFileDialog::Instance()->Display("OpenDlg", ImGuiWindowFlags_NoCollapse, minSz, maxSz)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            openedFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string name = std::filesystem::path(openedFilePath).filename().string();
            statusMessage = "Opened: " + name;
            loadPreview(openedFilePath);

            // Add to recent
            auto it = std::find(recentFiles.begin(), recentFiles.end(), openedFilePath);
            if (it != recentFiles.end()) recentFiles.erase(it);
            recentFiles.push_back(openedFilePath);
            if ((int)recentFiles.size() > MAX_RECENT) recentFiles.erase(recentFiles.begin());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // ── Save dialog ──────────────────────────────────────────────────────────
    if (ImGuiFileDialog::Instance()->Display("SaveDlg", ImGuiWindowFlags_NoCollapse, minSz, maxSz)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            savedFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string name = std::filesystem::path(savedFilePath).filename().string();
            statusMessage = "Save target set: " + name;
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

// ─────────────────────────────────────────────────────────────────────────────

void ofApp::loadPreview(const std::string& path) {
    hasPreview = false;
    if (!isImageFile(path)) return;
    if (previewImage.load(path)) {
        hasPreview = true;
    }
}

bool ofApp::isImageFile(const std::string& path) {
    std::string ext = ofToLower(std::filesystem::path(path).extension().string());
    return ext == ".png" || ext == ".jpg" || ext == ".jpeg" ||
           ext == ".gif" || ext == ".bmp" || ext == ".tga";
}
