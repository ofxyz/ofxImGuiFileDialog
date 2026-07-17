#include "ofApp.h"
#include "IconsFontAwesome5.h"
#include "imgui_internal.h"

void ofApp::setup() {
    ofSetWindowTitle("ofxImGuiFileDialog — File Picker Demo");
    ofBackground(24, 24, 28);
    ofSetFrameRate(60);

    ImGuiConfigFlags flags = ImGuiConfigFlags_DockingEnable;
#ifndef TARGET_OPENGLES
    flags |= ImGuiConfigFlags_ViewportsEnable;
#endif
    gui.setup(nullptr, false, flags, true);

    if (ImFont* f = ImFonts::LoadDefaultFonts(ImGui::GetIO().Fonts, 15.0f))
        gui.setDefaultFont(f);
    gui.rebuildFontsTexture();

    ImTheme::Setup(currentThemeId);
    uiScale = ImTheme::UIScale();
}

void ofApp::update() {}

void ofApp::draw() {
    gui.begin();

    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_O)) openFileDialog();
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_S)) openSaveDialog();

    drawDockspace();
    drawFilePanel();
    drawPreviewPanel();
    drawDialogs();

    if (showThemePanel) {
        ImGui::SetNextWindowSize(ImVec2(360, 420), ImGuiCond_FirstUseEver);
        if (ImGui::Begin(ICON_FA_PALETTE " Theme", &showThemePanel)) {
            if (ImTheme::ShowSelector(currentThemeId))
                uiScale = ImTheme::UIScale();

            ImGui::SeparatorText("Scale");
            ImGui::SetNextItemWidth(-1);
            if (ImGui::SliderFloat("##scale", &uiScale, 0.75f, 2.5f, "%.2fx"))
                ImTheme::SetUIScale(uiScale);
        }
        ImGui::End();
    }

    if (showDemoWindow)
        ImGui::ShowDemoWindow(&showDemoWindow);

    gui.end();
    gui.draw();
}

// ─────────────────────────────────────────────────────────────────────────────

void ofApp::drawDockspace() {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    ImGui::SetNextWindowViewport(vp->ID);

    ImGuiWindowFlags hostFlags =
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##Host", nullptr, hostFlags);
    ImGui::PopStyleVar(3);

    drawMenuBar();

    const float statusH = ImGui::GetFrameHeight();
    ImGuiID dockId = ImGui::GetID("##FilePickerDock");
    ImGui::DockSpace(dockId, ImVec2(0, -statusH), ImGuiDockNodeFlags_PassthruCentralNode);

    if (!defaultDockLayoutBuilt) {
        buildDefaultDockLayout(dockId);
        defaultDockLayoutBuilt = true;
    }

    drawStatusBar();

    ImGui::End();
}

void ofApp::buildDefaultDockLayout(ImGuiID dockspaceId) {
    ImGui::DockBuilderRemoveNode(dockspaceId);
    ImGui::DockBuilderAddNode(dockspaceId,
        ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->WorkSize);

    ImGuiID left = 0;
    ImGuiID center = dockspaceId;
    ImGui::DockBuilderSplitNode(center, ImGuiDir_Left, 0.34f, &left, &center);

    ImGui::DockBuilderDockWindow(ICON_FA_FOLDER_OPEN " Files", left);
    ImGui::DockBuilderDockWindow(ICON_FA_IMAGE " Preview", center);
    ImGui::DockBuilderFinish(dockspaceId);
}

void ofApp::drawMenuBar() {
    if (!ImGui::BeginMenuBar())
        return;

    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open File...", "Ctrl+O"))
            openFileDialog();
        if (ImGui::MenuItem(ICON_FA_SAVE " Save As...", "Ctrl+S"))
            openSaveDialog();
        ImGui::Separator();
        if (ImGui::MenuItem(ICON_FA_TIMES " Exit"))
            ofExit();
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
        ImGui::MenuItem(ICON_FA_PALETTE " Theme", nullptr, &showThemePanel);
        ImGui::MenuItem("ImGui Demo", nullptr, &showDemoWindow);
        ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
}

void ofApp::drawStatusBar() {
    const float h = ImGui::GetFrameHeight();
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_MenuBarBg));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 0));
    if (ImGui::BeginChild("##StatusBar", ImVec2(0, h), ImGuiChildFlags_None,
                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
        ImGui::AlignTextToFramePadding();

        std::string left;
        if (hasPreview) {
            const std::string name = std::filesystem::path(openedFilePath).filename().string();
            left = ICON_FA_FILE_IMAGE "  " + name + "     " +
                   ofToString((int)previewImage.getWidth()) + " x " +
                   ofToString((int)previewImage.getHeight()) + " px     " +
                   ofToString((int)std::lround(previewZoom * 100.0f)) + "%";
        } else if (!openedFilePath.empty()) {
            left = std::string(ICON_FA_FILE "  ") +
                   std::filesystem::path(openedFilePath).filename().string();
        } else {
            left = ICON_FA_INFO_CIRCLE "  Ready";
        }
        ImGui::TextUnformatted(left.c_str());

        const std::string right = std::string(ICON_FA_PALETTE "  ") + currentThemeId +
                                  "     " + ofToString(ofGetFrameRate(), 0) + " FPS";
        const float rw = ImGui::CalcTextSize(right.c_str()).x;
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - rw);
        ImGui::TextDisabled("%s", right.c_str());
    }
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

// ─────────────────────────────────────────────────────────────────────────────

void ofApp::openFileDialog() {
    IGFD::FileDialogConfig cfg;
    cfg.path = ofToDataPath("", true);
    ImGuiFileDialog::Instance()->OpenDialog(
        "OpenDlg", "Open File",
        "Images{.png,.jpg,.jpeg,.gif,.bmp,.tga},Text{.txt,.md},All files{.*}",
        cfg);
}

void ofApp::openSaveDialog() {
    IGFD::FileDialogConfig cfg;
    cfg.path = ofToDataPath("", true);
    cfg.fileName = "output.png";
    ImGuiFileDialog::Instance()->OpenDialog(
        "SaveDlg", "Save File As",
        ".png,.jpg,.jpeg",
        cfg);
}

void ofApp::drawFilePanel() {
    if (!ImGui::Begin(ICON_FA_FOLDER_OPEN " Files")) {
        ImGui::End();
        return;
    }

    ImGui::TextUnformatted(ICON_FA_FOLDER_OPEN "  File Picker");
    ImGui::TextDisabled("ofxImGuiFileDialog + ofxImGuiStyle");
    ImGui::Separator();
    ImGui::Spacing();

    // Accented primary action, secondary save below.
    const ImVec4 accent = ImGui::GetStyleColorVec4(ImGuiCol_CheckMark);
    auto shade = [](ImVec4 c, float m) { return ImVec4(c.x * m, c.y * m, c.z * m, c.w); };
    ImGui::PushStyleColor(ImGuiCol_Button,        accent);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, shade(accent, 1.15f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  shade(accent, 0.85f));
    if (ImGui::Button(ICON_FA_FOLDER_OPEN "  Open File...", ImVec2(-1, ImGui::GetFrameHeight() * 1.4f)))
        openFileDialog();
    ImGui::PopStyleColor(3);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Ctrl+O");

    ImGui::Spacing();
    if (ImGui::Button(ICON_FA_SAVE "  Save As...", ImVec2(-1, 0)))
        openSaveDialog();
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Ctrl+S");

    ImGui::Spacing();
    ImGui::SeparatorText("Selection");

    ImGui::BeginChild("##SelectedPath", ImVec2(-1, 60), ImGuiChildFlags_Borders);
    if (openedFilePath.empty()) {
        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("No file opened yet");
    } else {
        const std::filesystem::path p(openedFilePath);
        ImGui::TextUnformatted((ICON_FA_FILE_IMAGE "  " + p.filename().string()).c_str());
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
        ImGui::TextWrapped("%s", p.parent_path().string().c_str());
        ImGui::PopStyleColor();
    }
    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::SeparatorText("Save target");
    if (savedFilePath.empty()) {
        ImGui::TextDisabled("None set");
    } else {
        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_CheckMark), ICON_FA_CHECK_CIRCLE);
        ImGui::SameLine();
        ImGui::TextWrapped("%s", savedFilePath.c_str());
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Status");
    ImGui::TextWrapped("%s", statusMessage.c_str());

    ImGui::Spacing();
    ImGui::SeparatorText("Recent");
    if (recentFiles.empty()) {
        ImGui::TextDisabled("Opened files show up here.");
    } else {
        const float clearW = ImGui::CalcTextSize(ICON_FA_TRASH " Clear").x + ImGui::GetStyle().FramePadding.x * 2.0f;
        ImGui::TextDisabled("%d item%s", (int)recentFiles.size(), recentFiles.size() == 1 ? "" : "s");
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - clearW);
        if (ImGui::SmallButton(ICON_FA_TRASH " Clear"))
            recentFiles.clear();

        ImGui::BeginChild("##Recent", ImVec2(-1, -1), ImGuiChildFlags_Borders);
        for (int i = (int)recentFiles.size() - 1; i >= 0; --i) {
            const auto& f = recentFiles[i];
            const std::string label = std::filesystem::path(f).filename().string();
            const bool selected = (f == openedFilePath);
            ImGui::PushID(i);
            if (ImGui::Selectable((ICON_FA_FILE_IMAGE "  " + label).c_str(), selected)) {
                openedFilePath = f;
                loadPreview(f);
                statusMessage = "Loaded from recent: " + label;
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("%s", f.c_str());
            ImGui::PopID();
        }
        ImGui::EndChild();
    }

    ImGui::End();
}

// ─────────────────────────────────────────────────────────────────────────────

void ofApp::drawPreviewPanel() {
    if (!ImGui::Begin(ICON_FA_IMAGE " Preview")) {
        ImGui::End();
        return;
    }

    // Framed canvas that fills the panel.
    const ImVec2 origin = ImGui::GetCursorScreenPos();
    const ImVec2 avail = ImGui::GetContentRegionAvail();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const ImVec2 p0 = origin;
    const ImVec2 p1 = ImVec2(origin.x + avail.x, origin.y + avail.y);
    const float rounding = ImGui::GetStyle().FrameRounding;

    dl->AddRectFilled(p0, p1, ImGui::GetColorU32(ImGuiCol_FrameBg), rounding);

    if (!hasPreview) {
        dl->AddRect(p0, p1, ImGui::GetColorU32(ImGuiCol_Border), rounding);

        const char* line1 = "Open an image to preview it here";
        const char* line2 = "PNG  JPG  GIF  BMP  TGA";
        const float iconH = ImGui::GetTextLineHeight() * 2.0f;
        const float blockH = iconH + ImGui::GetTextLineHeightWithSpacing() * 2.0f
                           + ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y * 3.0f;

        auto centerNext = [&](float w) {
            ImGui::SetCursorPosX(ImGui::GetCursorStartPos().x + (avail.x - w) * 0.5f);
        };

        ImGui::SetCursorPosY(ImGui::GetCursorStartPos().y + (avail.y - blockH) * 0.5f);

        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
        ImGui::SetWindowFontScale(2.0f);
        centerNext(ImGui::CalcTextSize(ICON_FA_IMAGE).x);
        ImGui::TextUnformatted(ICON_FA_IMAGE);
        ImGui::SetWindowFontScale(1.0f);

        centerNext(ImGui::CalcTextSize(line1).x);
        ImGui::TextUnformatted(line1);
        centerNext(ImGui::CalcTextSize(line2).x);
        ImGui::TextUnformatted(line2);
        ImGui::PopStyleColor();

        ImGui::Spacing();
        const float btnW = 160.0f;
        centerNext(btnW);
        if (ImGui::Button(ICON_FA_FOLDER_OPEN "  Browse...", ImVec2(btnW, 0)))
            openFileDialog();
    } else {
        const float pad = 12.0f;
        const float imgW = (float)previewImage.getWidth();
        const float imgH = (float)previewImage.getHeight();
        const float boxW = std::max(1.0f, avail.x - pad * 2.0f);
        const float boxH = std::max(1.0f, avail.y - pad * 2.0f);
        previewZoom = std::min({boxW / imgW, boxH / imgH, 1.0f});
        const ImVec2 drawSz(imgW * previewZoom, imgH * previewZoom);
        const ImVec2 imgP0(
            origin.x + (avail.x - drawSz.x) * 0.5f,
            origin.y + (avail.y - drawSz.y) * 0.5f);
        const ImVec2 imgP1(imgP0.x + drawSz.x, imgP0.y + drawSz.y);

        // Checkerboard so transparent PNGs read correctly.
        const float tile = 12.0f;
        dl->PushClipRect(imgP0, imgP1, true);
        const ImU32 c0 = IM_COL32(58, 58, 64, 255);
        const ImU32 c1 = IM_COL32(48, 48, 54, 255);
        for (float y = imgP0.y; y < imgP1.y; y += tile) {
            for (float x = imgP0.x; x < imgP1.x; x += tile) {
                const bool even = ((int)((x - imgP0.x) / tile) + (int)((y - imgP0.y) / tile)) % 2 == 0;
                dl->AddRectFilled(ImVec2(x, y),
                                  ImVec2(std::min(x + tile, imgP1.x), std::min(y + tile, imgP1.y)),
                                  even ? c0 : c1);
            }
        }
        dl->PopClipRect();

        dl->AddImage(
            (ImTextureID)(uintptr_t)previewImage.getTexture().getTextureData().textureID,
            imgP0, imgP1);
        dl->AddRect(imgP0, imgP1, ImGui::GetColorU32(ImGuiCol_Border));

        // Zoom badge, top-right of the canvas.
        char badge[32];
        snprintf(badge, sizeof(badge), "%d%%", (int)std::lround(previewZoom * 100.0f));
        const ImVec2 bsz = ImGui::CalcTextSize(badge);
        const ImVec2 bpad(6, 3);
        const ImVec2 bp1(p1.x - 8, p0.y + 8);
        const ImVec2 bp0(bp1.x - bsz.x - bpad.x * 2.0f, bp1.y + bsz.y + bpad.y * 2.0f);
        dl->AddRectFilled(bp0, bp1, IM_COL32(0, 0, 0, 140), 4.0f);
        dl->AddText(ImVec2(bp0.x + bpad.x, bp0.y + bpad.y),
                    ImGui::GetColorU32(ImGuiCol_Text), badge);

        // Invisible hit area for tooltip.
        ImGui::SetCursorScreenPos(imgP0);
        ImGui::InvisibleButton("##img", drawSz);
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%d x %d px", (int)imgW, (int)imgH);
            ImGui::TextUnformatted(
                std::filesystem::path(openedFilePath).filename().string().c_str());
            ImGui::EndTooltip();
        }
    }

    ImGui::End();
}

// ─────────────────────────────────────────────────────────────────────────────

void ofApp::drawDialogs() {
    const ImVec2 minSz(640, 420);
    const ImVec2 maxSz(1280, 860);

    if (ImGuiFileDialog::Instance()->Display("OpenDlg", ImGuiWindowFlags_NoCollapse, minSz, maxSz)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            openedFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
            const std::string name = std::filesystem::path(openedFilePath).filename().string();
            statusMessage = "Opened: " + name;
            loadPreview(openedFilePath);

            auto it = std::find(recentFiles.begin(), recentFiles.end(), openedFilePath);
            if (it != recentFiles.end())
                recentFiles.erase(it);
            recentFiles.push_back(openedFilePath);
            if ((int)recentFiles.size() > MAX_RECENT)
                recentFiles.erase(recentFiles.begin());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("SaveDlg", ImGuiWindowFlags_NoCollapse, minSz, maxSz)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            savedFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
            const std::string name = std::filesystem::path(savedFilePath).filename().string();
            statusMessage = "Save target set: " + name;
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

// ─────────────────────────────────────────────────────────────────────────────

void ofApp::loadPreview(const std::string& path) {
    hasPreview = false;
    if (!isImageFile(path))
        return;
    if (previewImage.load(path))
        hasPreview = true;
}

bool ofApp::isImageFile(const std::string& path) {
    const std::string ext = ofToLower(std::filesystem::path(path).extension().string());
    return ext == ".png" || ext == ".jpg" || ext == ".jpeg" ||
           ext == ".gif" || ext == ".bmp" || ext == ".tga";
}
