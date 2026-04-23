#pragma once

// ============================================================================
// ofxImGuiFileDialog
// A full-featured Dear ImGui file dialog - no system dependencies, no zenity,
// no native OS dialogs. Works on Linux, Linux ARM, macOS, Windows.
//
// Wraps: https://github.com/ofrasp/ImGuiFileDialog
//
// Usage:
//   #include "ofxImGuiFileDialog.h"
//
//   // Open:
//   if (ImGui::Button("Open")) {
//       IGFD::FileDialogConfig config;
//       config.path = ".";
//       ImGuiFileDialog::Instance()->OpenDialog("key", "Choose File", ".png,.jpg", config);
//   }
//
//   // Display (call every frame inside your ImGui block):
//   if (ImGuiFileDialog::Instance()->Display("key")) {
//       if (ImGuiFileDialog::Instance()->IsOk()) {
//           std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
//       }
//       ImGuiFileDialog::Instance()->Close();
//   }
// ============================================================================

#include "ImGuiFileDialog.h"
