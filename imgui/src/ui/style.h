#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <string>

#include <GLFW/glfw3.h>

// ==============================================================================
// SISTEM FONT GLOBAL
// ==============================================================================
extern ImFont *fontSmall;   // 13px (Label, Badge)
extern ImFont *fontRegular; // 16px (Text biasa, Input)
extern ImFont *fontMedium;  // 18px (Sub-judul, Menu, Header Tabel)
extern ImFont *fontTitle;   // 24px (Judul Halaman Utama)
extern ImFont *fontHuge;    // 36px (Angka Statistik)

struct Toast
{
    std::string message;
    float timer;
    bool active;
};

extern Toast currentToast;

// ==============================================================================
// WARNA TEMA (Material You / Web Dashboard Light)
// ==============================================================================
const ImU32 COLOR_BG_APP = IM_COL32(243, 245, 249, 255);     // Background area kanan
const ImU32 COLOR_BG_SIDEBAR = IM_COL32(255, 255, 255, 255); // Putih bersih
const ImU32 COLOR_BG_CARD = IM_COL32(255, 255, 255, 255);    // Putih bersih
const ImU32 COLOR_PRIMARY = IM_COL32(66, 133, 244, 255);     // Biru Google/Material
const ImU32 COLOR_PRIMARY_DRK = IM_COL32(50, 100, 200, 255); // Biru Gelap (Hover)
const ImU32 COLOR_TEXT_MAIN = IM_COL32(35, 40, 45, 255);     // Hitam keabuan
const ImU32 COLOR_TEXT_MUTED = IM_COL32(130, 140, 150, 255); // Abu-abu teks
const ImU32 COLOR_BORDER = IM_COL32(230, 235, 240, 255);     // Garis batas tipis

// Warna Badge Status
const ImU32 COLOR_BADGE_GRN = IM_COL32(210, 245, 225, 255);
const ImU32 COLOR_TEXT_GRN = IM_COL32(40, 180, 80, 255);
const ImU32 COLOR_BADGE_BLU = IM_COL32(220, 235, 255, 255);
const ImU32 COLOR_TEXT_BLU = IM_COL32(50, 120, 240, 255);
const ImU32 COLOR_BADGE_ORG = IM_COL32(255, 240, 210, 255);
const ImU32 COLOR_TEXT_ORG = IM_COL32(220, 130, 20, 255);
const ImU32 COLOR_TEXT_RED = IM_COL32(220, 80, 80, 255);

std::string FindFontPath(const std::string &filename);
void ShowToast(const std::string &msg);
void RenderToast();
void DrawBadge(ImDrawList *draw_list, ImVec2 pos, const char *text, ImU32 bgColor, ImU32 txtColor);
bool DrawSidebarMenuPill(const char *icon, const char *label, bool isActive);
void BeginCard(const char *id, ImVec2 size, bool noPadding = false, bool noScrollbar = true);
void EndCard();
void DrawAvatar(ImDrawList *draw_list, ImVec2 center, float radius, const char *name, ImU32 bgColor);
void SetupFonts();
void SetupWebStyle();
