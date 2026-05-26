meta:
	ADDON_NAME = ofxImGuiFileDialog
	ADDON_DESCRIPTION = ImGuiFileDialog for openFrameworks - File dialog built entirely on Dear ImGui, no system dependencies.
	ADDON_AUTHOR = aiekick (wrapped for OF by ofRasp)
	ADDON_TAGS = "imgui" "dialog" "file"
	ADDON_URL = https://github.com/ofxyz/ImGuiFileDialog

common:
	ADDON_DEPENDENCIES = ofxImGui

	ADDON_INCLUDES = src
	ADDON_INCLUDES += libs/ImGuiFileDialog

	ADDON_SOURCES = libs/ImGuiFileDialog/ImGuiFileDialog.cpp

	# ImGuiFileDialog.cpp must be rebuilt whenever ImGuiFileDialogConfig.h changes
	# (e.g. USE_PLACES_FEATURE). If you see undefined IGFD::PlacesFeature::* refs,
	# delete addons/obj/<platform>/Release/ofxImGuiFileDialog/ and rebuild.

linux64:
linuxarmv6l:
linuxarmv7l:
linuxaarch64:
vs:
osx:
ios:
