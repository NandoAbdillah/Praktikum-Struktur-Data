#include "imgui.h"
#include "imgui_internal.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>

#include "assets/fonts/IconsFontAwesome7.h"

using namespace std;

// ==============================================================================
// SISTEM FONT GLOBAL
// ==============================================================================
ImFont *fontSmall = nullptr;   // 13px (Label, Badge)
ImFont *fontRegular = nullptr; // 16px (Text biasa, Input)
ImFont *fontMedium = nullptr;  // 18px (Sub-judul, Menu, Header Tabel)
ImFont *fontTitle = nullptr;   // 24px (Judul Halaman Utama)
ImFont *fontHuge = nullptr;    // 36px (Angka Statistik)

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

struct Toast
{
    std::string message;
    float timer;
    bool active;
};
Toast currentToast = {"", 0.0f, false};

// Fungsi pencari file Font yang aman (Mencegah Crash)
string FindFontPath(const std::string &filename)
{
    const char *search_paths[] = {
        "",
        "font/",
        "assets/fonts/",
        "../font/",
        "../assets/fonts/",
        "../../font/",
        "../../assets/fonts/"};

    for (const char *path : search_paths)
    {
        std::string full_path = std::string(path) + filename;
        std::ifstream f(full_path.c_str());
        if (f.good())
            return full_path;
    }
    return "";
}

void ShowToast(const std::string &msg)
{
    currentToast.message = msg;
    currentToast.timer = 3.0f; // Tampil selama 3 detik
    currentToast.active = true;
}

void RenderToast()
{
    if (!currentToast.active)
        return;

    currentToast.timer -= ImGui::GetIO().DeltaTime;
    if (currentToast.timer <= 0.0f)
    {
        currentToast.active = false;
        return;
    }

    // Animasi Fade In/Out
    float alpha = 1.0f;
    if (currentToast.timer < 0.5f)
        alpha = currentToast.timer / 0.5f;
    else if (currentToast.timer > 2.5f)
        alpha = (3.0f - currentToast.timer) / 0.5f;

    // Pastikan alpha tidak keluar dari batas 0.0 - 1.0
    if (alpha < 0.0f)
        alpha = 0.0f;
    if (alpha > 1.0f)
        alpha = 1.0f;

    ImGuiViewport *viewport = ImGui::GetMainViewport();
    // Posisikan di bawah tengah, agak naik sedikit agar tidak mepet taskbar
    ImVec2 pos = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x / 2, viewport->WorkPos.y + viewport->WorkSize.y - 70);

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    // HAPUS NoInputs agar tombol bisa diklik!
    // Tambah NoFocusOnAppearing agar saat toast muncul, dia tidak merebut fokus ketikan user
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                             ImGuiWindowFlags_NoNav;

    // Gaya ala Bootstrap
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);                // Sudut kotak membulat (bukan pil)
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 12.0f)); // Ruang napas yang lega

    // Background Dark ala Bootstrap (Abu-abu kehitaman) dengan mengikuti efek alpha
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.13f, 0.14f, 0.15f, alpha * 0.95f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, alpha));

    if (ImGui::Begin("Toast", nullptr, flags))
    {
        ImGui::PushFont(fontMedium);

        // 1. IKON (Bisa diganti ICON_FA_CHECK_CIRCLE atau sesuai kebutuhan)
        // Beri warna hijau atau biru pada ikon agar manis
        ImGui::PushFont(fontMedium);
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, alpha), "%s", ICON_FA_CIRCLE_INFO);
        ImGui::PopFont();
        ImGui::SameLine(0, 10.0f); // Jarak 10px dari ikon ke teks

        // 2. PESAN TOAST
        // Posisikan agar teks rata tengah secara vertikal dengan tombol
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", currentToast.message.c_str());

        // 3. JARAK PEMISAH KE TOMBOL SILANG
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(30.0f, 0.0f)); // Dorong tombol silang agak ke kanan
        ImGui::SameLine();

        // 4. TOMBOL SILANG (X)
        // Kita buat tombol transparan agar menyatu dengan background
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.5f)); // Abu-abu saat di-hover
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, alpha)); // Warna X agak redup

        // Catatan: Jika FontAwesome 5, gunakan ICON_FA_TIMES. Jika FA6, gunakan ICON_FA_XMARK
        if (ImGui::Button(ICON_FA_XMARK))
        {
            currentToast.active = false; // Tutup toast langsung jika diklik
        }
        ImGui::PopStyleColor(4);

        ImGui::PopFont();
    }
    ImGui::End();

    // Jangan lupa Pop semuanya sesuai jumlah Push
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
}

// ==============================================================================
// FUNGSI BANTUAN RENDERING UI ESTETIK
// ==============================================================================

// Menggambar Badge Kapsul (Status)
void DrawBadge(ImDrawList *draw_list, ImVec2 pos, const char *text, ImU32 bgColor, ImU32 txtColor)
{
    ImVec2 textSize = ImGui::CalcTextSize(text);
    ImVec2 padding(10, 4);
    ImVec2 bgMin = pos;
    ImVec2 bgMax = ImVec2(pos.x + textSize.x + padding.x * 2, pos.y + textSize.y + padding.y * 2);

    draw_list->AddRectFilled(bgMin, bgMax, bgColor, 12.0f);

    // PERBAIKAN: Gunakan PushFont dan PopFont
    ImGui::PushFont(fontSmall);
    draw_list->AddText(ImVec2(pos.x + padding.x, pos.y + padding.y), txtColor, text);
    ImGui::PopFont();
}

// Menu Sidebar berbentuk Kapsul (Pill) dengan transisi mulus
bool DrawSidebarMenuPill(const char *icon, const char *label, bool isActive)
{
    ImGui::PushID(label);
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size(230, 48); // Lebih tebal dan panjang

    bool clicked = ImGui::InvisibleButton("##btn", size);
    bool hovered = ImGui::IsItemHovered();

    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    // Transisi warna background kapsul
    if (isActive)
    {
        draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), COLOR_PRIMARY, 12.0f);
    }
    else if (hovered)
    {
        draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(240, 244, 250, 255), 12.0f);
    }

    // Teks Icon & Label
    ImU32 textColor = isActive ? IM_COL32(255, 255, 255, 255) : (hovered ? COLOR_PRIMARY : COLOR_TEXT_MUTED);

    ImGui::PushFont(fontRegular);
    draw_list->AddText(ImVec2(pos.x + 20, pos.y + 14), textColor, icon); // Tempat icon palsu
    draw_list->AddText(ImVec2(pos.x + 55, pos.y + 13), textColor, label);
    ImGui::PopFont();

    ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + size.y + 8)); // Spacing antar menu
    ImGui::PopID();

    return clicked;
}

void BeginCard(const char *id, ImVec2 size, bool noPadding = false, bool noScrollbar = true)
{
    if (!noPadding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30, 30));
    else
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1, 1, 1, 1));

    ImGuiChildFlags child_flags = ImGuiChildFlags_AlwaysUseWindowPadding;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

    if (noScrollbar)
    {
        window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    }

    ImGui::BeginChild(id, size, child_flags, window_flags);
}

void EndCard()
{
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

// Menggambar Avatar Lingkaran dengan Inisial
void DrawAvatar(ImDrawList *draw_list, ImVec2 center, float radius, const char *name, ImU32 bgColor)
{
    draw_list->AddCircleFilled(center, radius, bgColor);

    // Ambil inisial dari nama
    char initials[3] = {0};
    if (name && strlen(name) > 0)
    {
        initials[0] = toupper(name[0]);
        // Cari spasi untuk inisial kedua
        const char *space = strchr(name, ' ');
        if (space && *(space + 1) != '\0')
        {
            initials[1] = toupper(*(space + 1));
        }
    }

    ImVec2 tSize = ImGui::CalcTextSize(initials);

    // PERBAIKAN: Gunakan PushFont dan PopFont
    ImGui::PushFont(fontMedium);
    draw_list->AddText(ImVec2(center.x - tSize.x / 2, center.y - tSize.y / 2), IM_COL32(255, 255, 255, 255), initials);
    ImGui::PopFont();
}

void SetupFonts()
{
    ImGuiIO &io = ImGui::GetIO();

    // 3. LOAD FONTS
    std::string pathRegular = FindFontPath("Poppins-Regular.ttf");
    std::string pathBold = FindFontPath("Poppins-Bold.ttf");
    std::string pathIcon = FindFontPath("fa-solid-900.otf");

    if (!pathRegular.empty() && !pathBold.empty() && !pathIcon.empty())
    {
        static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};

        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;

        fontSmall = io.Fonts->AddFontFromFileTTF(pathRegular.c_str(), 18.0f);
        io.Fonts->AddFontFromFileTTF(
            pathIcon.c_str(),
            16.0f,
            &icons_config,
            icon_ranges);
        fontRegular = io.Fonts->AddFontFromFileTTF(pathRegular.c_str(), 24.0f);
        io.Fonts->AddFontFromFileTTF(
            pathIcon.c_str(),
            20.0f,
            &icons_config,
            icon_ranges);
        fontMedium = io.Fonts->AddFontFromFileTTF(pathBold.c_str(), 34.0f);
        io.Fonts->AddFontFromFileTTF(
            pathIcon.c_str(),
            24.0f,
            &icons_config,
            icon_ranges);
        fontTitle = io.Fonts->AddFontFromFileTTF(pathBold.c_str(), 42.0f);
        io.Fonts->AddFontFromFileTTF(
            pathIcon.c_str(),
            34.0f,
            &icons_config,
            icon_ranges);
        fontHuge = io.Fonts->AddFontFromFileTTF(pathBold.c_str(), 56.0f);
        io.Fonts->AddFontFromFileTTF(
            pathIcon.c_str(),
            56.0f,
            &icons_config,
            icon_ranges);
    }
    else
    {
        std::cout << "WARNING: Font Poppins tidak ditemukan!\n";
        fontSmall = fontRegular = fontMedium = fontTitle = fontHuge = io.Fonts->AddFontDefault();
    }
}

// Konfigurasi Standar Style ImGui
void SetupWebStyle()
{
    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;

    colors[ImGuiCol_WindowBg] = ImColor(COLOR_BG_APP);
    colors[ImGuiCol_ChildBg] = ImColor(COLOR_BG_CARD);
    colors[ImGuiCol_Text] = ImColor(COLOR_TEXT_MAIN);
    colors[ImGuiCol_TextDisabled] = ImColor(COLOR_TEXT_MUTED);
    colors[ImGuiCol_Border] = ImColor(COLOR_BORDER);
    colors[ImGuiCol_Button] = ImColor(COLOR_PRIMARY);
    colors[ImGuiCol_ButtonHovered] = ImColor(COLOR_PRIMARY_DRK);
    colors[ImGuiCol_ButtonActive] = ImColor(COLOR_PRIMARY);
    colors[ImGuiCol_FrameBg] = ImVec4(0.94f, 0.95f, 0.96f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.92f, 0.94f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.88f, 0.90f, 0.92f, 1.00f);

    colors[ImGuiCol_Header] = ImColor(COLOR_BADGE_BLU); // Untuk baris tabel
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.85f, 0.90f, 0.98f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.80f, 0.88f, 0.96f, 1.00f);

    style.WindowRounding = 0.0f;
    style.ChildRounding = 16.0f; // Radius melengkung ala Web
    style.FrameRounding = 8.0f;  // Input text rounded
    style.WindowPadding = ImVec2(0, 0);
    style.ItemSpacing = ImVec2(15, 15);
    style.FramePadding = ImVec2(15, 12); // Padding input text yg lega
    style.ChildBorderSize = 1.0f;        // Border super tipis untuk card
}
