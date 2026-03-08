#include "imgui.h"
#include "imgui_internal.h"
#include "ui/style.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>

#include "data.h"
#include "utils/helper.h"
#include "assets/fonts/IconsFontAwesome7.h"

enum class AppState
{
    Dashboard,
    Services_All,
    Services_New,
    Services_History,
    AllCustomers,
    CustomerData
};

int HitungTotalServis()
{
    int total = 0;
    data::Customer *c = data::headCustomer;
    while (c != NULL)
    {
        data::Service *s = c->head_service;
        while (s != NULL)
        {
            total++;
            s = s->next_in_customer;
        }
        c = c->next;
    }
    return total;
}

int main()
{
    // 1. INISIALISASI DATA BACKEND LOGNUTS
    data::init();
    if (!data::loadAllCustomers())
        std::cout << "Warning: Gagal membuka customers.csv (Mungkin file belum ada)\n";
    if (!data::loadAllServices())
        std::cout << "Warning: Gagal membuka services.csv (Mungkin file belum ada)\n";

    // 2. INISIALISASI GLFW & OPENGL
    if (!glfwInit())
        return -1;
    GLFWwindow *window = glfwCreateWindow(1366, 768, "Lognuts Workshop Management", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsLight();

    SetupFonts();
    SetupWebStyle();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    AppState currentState = AppState::Dashboard;

    // Buffer Form Servis Baru
    static char in_modelMobil[128] = "";
    static char in_merekMobil[128] = "";
    static char in_kendala[512] = "";
    static int idx_montir = 0;
    const char *list_montir[] = {"Suby", "Farhan", "Dimas", "Fajar"};

    static char in_namaCustomer[128] = "";
    static bool customerFound = false;
    static bool hasCheckedCustomer = false;
    static data::Customer *foundCustomerPtr = nullptr;

    static char in_telpCustomer[14] = "";
    static char in_alamatCustomer[256] = "";
    static int in_umurCustomer = 20;
    static int in_genderCustomer = 0; // 0 = L, 1 = P

    // Filter History Montir
    static int filter_montir_idx = 0;

    // Pointer Navigasi Data Pelanggan (Next/Prev)
    static data::Customer *current_detail_cust = data::headCustomer;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGuiWindowFlags layoutFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus;
        ImGui::Begin("MainLayout", nullptr, layoutFlags);

        // BAGIAN 1: SIDEBAR NAVIGASI (KIRI)
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(ImColor(COLOR_BG_SIDEBAR)));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        ImGui::BeginChild("Sidebar", ImVec2(300, 0), false);

        ImVec2 sbPos = ImGui::GetCursorScreenPos();
        ImDrawList *sbDraw = ImGui::GetWindowDrawList();

        // 1.1 Brand Logo Lognuts
        ImGui::SetCursorPos(ImVec2(30, 40));
        sbDraw->AddRectFilled(ImVec2(sbPos.x + 30, sbPos.y + 40), ImVec2(sbPos.x + 65, sbPos.y + 75), COLOR_PRIMARY, 12.0f);

        // Garis dekorasi di dalam kotak biru
        sbDraw->AddLine(ImVec2(sbPos.x + 40, sbPos.y + 57), ImVec2(sbPos.x + 55, sbPos.y + 57), IM_COL32(255, 255, 255, 255), 4.0f);
        sbDraw->AddLine(ImVec2(sbPos.x + 40, sbPos.y + 50), ImVec2(sbPos.x + 50, sbPos.y + 50), IM_COL32(255, 255, 255, 200), 3.0f);

        sbDraw->AddText(fontTitle, 26.0f, ImVec2(sbPos.x + 80, sbPos.y + 42), COLOR_TEXT_MAIN, "Lognuts");
        sbDraw->AddText(fontSmall, 13.0f, ImVec2(sbPos.x + 82, sbPos.y + 68), COLOR_TEXT_MUTED, "Praktikum Struktur Data");

        ImGui::SetCursorPosY(130);

        // 1.2 Kategori: MAIN
        ImGui::SetCursorPosX(35);
        ImGui::PushFont(fontSmall);
        ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "MAIN MENU");
        ImGui::PopFont();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

        ImGui::SetCursorPosX(30);
        if (DrawSidebarMenuPill(ICON_FA_HOUSE, "Dashboard", currentState == AppState::Dashboard))
        {
            currentState = AppState::Dashboard;
        }

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

        // 1.3 Kategori: SERVICES
        ImGui::SetCursorPosX(35);
        ImGui::PushFont(fontSmall);
        ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "MANAJEMEN SERVIS");
        ImGui::PopFont();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

        ImGui::SetCursorPosX(30);

        if (DrawSidebarMenuPill(ICON_FA_SQUARE_PLUS, "Servis Baru", currentState == AppState::Services_New))
        {
            currentState = AppState::Services_New;
            hasCheckedCustomer = false; // Reset state form
        }

        ImGui::SetCursorPosX(30);
        if (DrawSidebarMenuPill(ICON_FA_CAR_REAR, "Semua Servis", currentState == AppState::Services_All))
        {
            currentState = AppState::Services_All;
        }

        ImGui::SetCursorPosX(30);
        if (DrawSidebarMenuPill(ICON_FA_USER_GEAR, "Riwayat Montir", currentState == AppState::Services_History))
        {
            currentState = AppState::Services_History;
        }

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

        // 1.4 Kategori: CUSTOMERS
        ImGui::SetCursorPosX(35);
        ImGui::PushFont(fontSmall);
        ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "PELANGGAN");
        ImGui::PopFont();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

        ImGui::SetCursorPosX(30);
        if (DrawSidebarMenuPill(ICON_FA_USERS, "Semua Pelanggan", currentState == AppState::AllCustomers))
        {
            currentState = AppState::AllCustomers;
        }

        ImGui::SetCursorPosX(30);
        if (DrawSidebarMenuPill(ICON_FA_USER, "Profil & Detail", currentState == AppState::CustomerData))
        {
            currentState = AppState::CustomerData;
            if (!current_detail_cust)
                current_detail_cust = data::headCustomer; // Reset ke awal jika kosong
        }

        // 1.5 Tombol Exit di bagian bawah
        ImGui::SetCursorPos(ImVec2(30, viewport->WorkSize.y - 80));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0.9f, 0.9f, 1));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.2f, 0.2f, 1));
        if (ImGui::Button("Keluar Aplikasi", ImVec2(230, 45)))
        {
            glfwSetWindowShouldClose(window, true);
        }
        ImGui::PopStyleColor(2);

        ImGui::EndChild(); // End Sidebar
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        ImGui::SameLine(0, 0);

        // ======================================================================
        // BAGIAN 2: AREA KONTEN UTAMA (KANAN)
        // ======================================================================
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(ImColor(COLOR_BG_APP)));
        ImGui::BeginChild("ContentArea", ImVec2(0, 0), false);
        ImGui::PopStyleColor();

        ImDrawList *bg_draw_list = ImGui::GetWindowDrawList();

        // 2.1 HEADER KONTEN (Judul Halaman Dinamis)
        ImGui::SetCursorPos(ImVec2(40, 40));
        ImGui::BeginGroup();
        ImGui::PushFont(fontTitle);

        std::string pageTitle = "";
        std::string pageSub = "";
        switch (currentState)
        {
        case AppState::Dashboard:
            pageTitle = "Dashboard Sistem";
            pageSub = "Ringkasan operasional bengkel Lognuts.";
            break;
        case AppState::Services_New:
            pageTitle = "Registrasi Servis";
            pageSub = "Input data kendaraan dan antrean bengkel.";
            break;
        case AppState::Services_All:
            pageTitle = "Daftar Semua Servis";
            pageSub = "Log lengkap pengerjaan mekanik.";
            break;
        case AppState::Services_History:
            pageTitle = "Riwayat Pekerjaan Montir";
            pageSub = "Lacak kinerja setiap mekanik.";
            break;
        case AppState::AllCustomers:
            pageTitle = "Direktori Pelanggan";
            pageSub = "Database seluruh pelanggan terdaftar.";
            break;
        case AppState::CustomerData:
            pageTitle = "Profil Pelanggan";
            pageSub = "Detail informasi dan riwayat kedatangan.";
            break;
        }

        ImGui::Text("%s", pageTitle.c_str());
        ImGui::PopFont();
        ImGui::PushFont(fontRegular);
        ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "%s", pageSub.c_str());
        ImGui::PopFont();
        ImGui::EndGroup();

        ImGui::Dummy(ImVec2(0, 25.0f));
        ImGui::SetCursorPosX(40.0f);

        // 2.2 KONTEN BERDASARKAN STATE (SWITCH CASE)
        switch (currentState)
        {

        // ------------------------------------------------------------------
        // STATE: DASHBOARD
        // ------------------------------------------------------------------
        case AppState::Dashboard:
        {
            // Widget 1: Total Pelanggan
            ImGui::BeginGroup();
            ImVec2 pCard1 = ImGui::GetCursorScreenPos();
            ImVec2 szCard = ImVec2(350, 180);

            // Gambar custom card biru dengan shadow/deco
            bg_draw_list->AddRectFilled(pCard1, ImVec2(pCard1.x + szCard.x, pCard1.y + szCard.y), COLOR_PRIMARY, 20.0f);
            bg_draw_list->AddCircleFilled(ImVec2(pCard1.x + szCard.x, pCard1.y), 80.0f, IM_COL32(255, 255, 255, 30));

            ImGui::SetCursorScreenPos(ImVec2(pCard1.x + 30, pCard1.y + 30));
            DrawBadge(bg_draw_list, ImGui::GetCursorScreenPos(), "User Database", IM_COL32(255, 255, 255, 255), COLOR_PRIMARY);

            ImGui::SetCursorScreenPos(ImVec2(pCard1.x + 30, pCard1.y + 80));
            ImGui::PushFont(fontRegular);
            ImGui::TextColored(ImColor(255, 255, 255, 200), "Total Pelanggan Terdaftar");
            ImGui::PopFont();

            ImGui::SetCursorScreenPos(ImVec2(pCard1.x + 30, pCard1.y + 105));
            ImGui::PushFont(fontHuge);
            ImGui::TextColored(ImColor(255, 255, 255, 255), "%d Orang", data::countCustomers());
            ImGui::PopFont();

            // Widget 2: Total Servis (Putih)
            ImGui::SetCursorScreenPos(ImVec2(pCard1.x + szCard.x + 30, pCard1.y));
            ImVec2 pCard2 = ImGui::GetCursorScreenPos();

            bg_draw_list->AddRectFilled(pCard2, ImVec2(pCard2.x + szCard.x, pCard2.y + szCard.y), COLOR_BG_CARD, 20.0f);

            ImGui::SetCursorScreenPos(ImVec2(pCard2.x + 30, pCard2.y + 30));
            DrawBadge(bg_draw_list, ImGui::GetCursorScreenPos(), "Operations", COLOR_BADGE_GRN, COLOR_TEXT_GRN);

            ImGui::SetCursorScreenPos(ImVec2(pCard2.x + 30, pCard2.y + 80));
            ImGui::PushFont(fontRegular);
            ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Total Kendaraan Diservis");
            ImGui::PopFont();

            ImGui::SetCursorScreenPos(ImVec2(pCard2.x + 30, pCard2.y + 105));
            ImGui::PushFont(fontHuge);
            ImGui::Text("%d Unit", HitungTotalServis());
            ImGui::PopFont();

            ImGui::EndGroup();

            // Widget Bawah: Welcome Banner
            ImGui::SetCursorPos(ImVec2(40, 380));
            BeginCard("BannerCard", ImVec2(730, 250));
            ImGui::PushFont(fontTitle);
            ImGui::Text("Selamat Datang, Administrator!");
            ImGui::PopFont();
            ImGui::Spacing();
            ImGui::PushFont(fontRegular);
            ImGui::TextWrapped("Sistem ini terhubung langsung dengan core C++ Linked-List. Semua data yang Anda masukkan (Servis Baru, Pelanggan Baru) akan langsung tersimpan di dalam memori dan diekspor ke file CSV secara otomatis. Silakan gunakan menu di sebelah kiri untuk menavigasi aplikasi.");
            ImGui::PopFont();
            EndCard();

            break;
        }

        // ------------------------------------------------------------------
        // STATE: SERVIS BARU (FORM KOMPLEKS)
        // ------------------------------------------------------------------
        case AppState::Services_New:
        {
            BeginCard("FormServisCard", ImVec2(900, 600), false, false);

            ImDrawList *child_draw_list = ImGui::GetWindowDrawList();

            ImGui::PushFont(fontMedium);
            ImGui::Text("Langkah 1: Identifikasi Pelanggan");
            ImGui::PopFont();
            ImGui::Separator();
            ImGui::Spacing();

            // Input Nama Customer
            ImGui::PushFont(fontRegular);
            ImGui::Text("Nama Pelanggan (Pemilik)");
            ImGui::SetNextItemWidth(400);
            ImGui::InputText("##namacust", in_namaCustomer, IM_ARRAYSIZE(in_namaCustomer));
            ImGui::SameLine();

            if (ImGui::Button("Cek Database", ImVec2(150, 42)))
            {
                hasCheckedCustomer = true;
                std::string searchName = in_namaCustomer;
                std::string foundId = data::findCustomerIdByName(searchName);

                if (foundId == "")
                {
                    customerFound = false;
                    foundCustomerPtr = nullptr;
                }
                else
                {
                    customerFound = true;
                    foundCustomerPtr = data::findCustomerById(foundId);
                }
            }

            ImGui::Spacing();
            ImGui::Spacing();

            // Jika sudah di-cek
            if (hasCheckedCustomer)
            {
                if (customerFound && foundCustomerPtr)
                {
                    // Tampilkan Badge Hijau
                    ImVec2 pBadge = ImGui::GetCursorScreenPos();

                    DrawBadge(child_draw_list, pBadge, "Pelanggan Terdaftar Ditemukan!", COLOR_BADGE_GRN, COLOR_TEXT_GRN);
                    ImGui::Dummy(ImVec2(0, 30)); // Spacing manual karena draw_list tidak merubah layout cursor

                    ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Data ID: %s | Telp: %s", foundCustomerPtr->id_customer.c_str(), foundCustomerPtr->nomor_telepon.c_str());
                    ImGui::Spacing();
                    ImGui::Spacing();
                }
                else
                {
                    // Tampilkan Form Registrasi Pelanggan Baru
                    ImVec2 pBadge = ImGui::GetCursorScreenPos();
                    DrawBadge(child_draw_list, pBadge, "Pelanggan Baru - Harap Lengkapi Data", COLOR_BADGE_ORG, COLOR_TEXT_ORG);
                    ImGui::Dummy(ImVec2(0, 30));

                    ImGui::BeginGroup(); // Kiri
                    ImGui::Text("Nomor Telepon");
                    ImGui::SetNextItemWidth(250);
                    ImGui::InputText("##telp", in_telpCustomer, IM_ARRAYSIZE(in_telpCustomer), ImGuiInputTextFlags_CallbackCharFilter, helper::filterHanyaAngka);

                    ImGui::Spacing();
                    ImGui::Text("Umur");
                    ImGui::SetNextItemWidth(150);
                    ImGui::InputInt("##umur", &in_umurCustomer);
                    ImGui::EndGroup();

                    ImGui::SameLine(350);

                    ImGui::BeginGroup(); // Kanan
                    ImGui::Text("Jenis Kelamin");
                    ImGui::RadioButton("Laki-laki (L)", &in_genderCustomer, 0);
                    ImGui::SameLine();
                    ImGui::RadioButton("Perempuan (P)", &in_genderCustomer, 1);

                    ImGui::Spacing();
                    ImGui::Text("Alamat Lengkap");
                    ImGui::SetNextItemWidth(350);
                    ImGui::InputText("##alamat", in_alamatCustomer, IM_ARRAYSIZE(in_alamatCustomer));
                    ImGui::EndGroup();

                    ImGui::Spacing();
                    ImGui::Spacing();
                }

                ImGui::PushFont(fontMedium);
                ImGui::Text("Langkah 2: Data Kendaraan & Keluhan");
                ImGui::PopFont();
                ImGui::Separator();
                ImGui::Spacing();

                // Form Kendaraan (Kiri & Kanan)
                ImGui::BeginGroup();
                ImGui::Text("Merek Mobil");
                ImGui::SetNextItemWidth(250);
                ImGui::InputText("##merek", in_merekMobil, IM_ARRAYSIZE(in_merekMobil));
                ImGui::EndGroup();

                ImGui::SameLine(350);

                ImGui::BeginGroup();
                ImGui::Text("Model Mobil");
                ImGui::SetNextItemWidth(250);
                ImGui::InputText("##model", in_modelMobil, IM_ARRAYSIZE(in_modelMobil));
                ImGui::EndGroup();

                ImGui::Spacing();
                ImGui::Text("Pilih Montir Penanggung Jawab");
                ImGui::SetNextItemWidth(250);
                ImGui::Combo("##montir", &idx_montir, list_montir, IM_ARRAYSIZE(list_montir));

                ImGui::Spacing();
                ImGui::Text("Deskripsi Keluhan / Kendala");
                ImGui::SetNextItemWidth(600);
                ImGui::InputTextMultiline("##kendala", in_kendala, IM_ARRAYSIZE(in_kendala), ImVec2(0, 80));

                ImGui::Spacing();
                ImGui::Spacing();

                // TOMBOL SUBMIT FINAL
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(COLOR_TEXT_GRN)));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.6f, 0.2f, 1.0f));
                if (ImGui::Button("Proses Data Servis", ImVec2(250, 45)))
                {

                    // LOGIKA BACKEND: new_service() versi GUI
                    data::Customer *owner = nullptr;
                    data::Service s;

                    if (!customerFound)
                    {
                        // Buat Customer Baru
                        owner = new data::Customer;
                        owner->nama = in_namaCustomer;
                        owner->nomor_telepon = in_telpCustomer;
                        owner->alamat = in_alamatCustomer;
                        owner->umur = in_umurCustomer;
                        owner->gender = (in_genderCustomer == 0) ? 'L' : 'P';

                        owner->next = NULL;
                        owner->head_service = NULL;

                        int totalCust = data::countCustomers();
                        helper::generateID('C', totalCust, &owner->id_customer);
                        data::insertCustomerBelakang(*owner);
                        data::saveCustomer(*owner);

                        s.id_customer = owner->id_customer;
                    }
                    else
                    {
                        // Pakai Customer Lama
                        owner = foundCustomerPtr;
                        s.id_customer = owner->id_customer;
                    }

                    // Isi Data Servis
                    s.model_mobil = in_modelMobil;
                    s.merek_mobil = in_merekMobil;
                    s.deskripsi_kendala = in_kendala;
                    s.nama_montir = list_montir[idx_montir];

                    int totalServ = data::countServices();
                    helper::generateID('S', totalServ, &s.id_service);

                    s.data_customer = owner;
                    s.next = NULL;

                    // Insert & Save
                    data::insertServiceBelakangToCustomer(owner, s);
                    data::saveService(s);

                    // Reset Form
                    in_namaCustomer[0] = '\0';
                    in_merekMobil[0] = '\0';
                    in_modelMobil[0] = '\0';
                    in_kendala[0] = '\0';
                    hasCheckedCustomer = false;

                    ShowToast("Sukses: Data Servis Berhasil Ditambahkan!");
                }
                ImGui::PopStyleColor(2);
            }

            ImGui::PopFont();
            EndCard();
            break;
        }

            // ------------------------------------------------------------------
            // STATE: SEMUA SERVIS (Tabel Relasional)
            // ------------------------------------------------------------------
        case AppState::Services_All:
        {
            BeginCard("CardAllServices", ImVec2(1000, 600), false, false);

            ImGui::PushFont(fontMedium);
            ImGui::Text("Daftar Riwayat Servis Global");
            ImGui::PopFont();
            ImGui::Separator();
            ImGui::Spacing();

            // Variabel state untuk paginasi
            static int currentPage = 0;
            const int itemsPerPage = 10;

            // Hitung total data terlebih dahulu untuk paginasi
            int totalServices = 0;
            data::Customer *count_c = data::headCustomer;
            while (count_c != NULL)
            {
                data::Service *count_s = count_c->head_service;
                while (count_s != NULL)
                {
                    totalServices++;
                    count_s = count_s->next_in_customer;
                }
                count_c = count_c->next;
            }

            int totalPages = (totalServices + itemsPerPage - 1) / itemsPerPage;
            if (totalPages == 0)
                totalPages = 1; // Minimal 1 halaman walau kosong

            // Pastikan currentPage tidak melebihi batas jika data dihapus
            if (currentPage >= totalPages)
                currentPage = totalPages - 1;
            if (currentPage < 0)
                currentPage = 0;

            // --- BAGIAN TABEL ---
            ImGuiTableFlags table_flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY;

            // Beri sedikit padding sel agar lebih lega
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));

            if (ImGui::BeginTable("TableServices", 6, table_flags, ImVec2(0, 420))) // Kurangi tinggi sedikit untuk tempat tombol paginasi
            {
                ImGui::TableSetupScrollFreeze(0, 1);                                    // Header melayang
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 60.0f); // Singkat judul
                ImGui::TableSetupColumn("Pelanggan", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Kendaraan", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Keluhan", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Montir", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 80.0f);

                ImGui::PushFont(fontMedium);
                ImGui::TableHeadersRow();
                ImGui::PopFont();

                // Aktifkan fontRegular untuk isi tabel
                ImGui::PushFont(fontRegular);

                // Traverse Linked List Customer -> Services dengan Paginasi
                data::Customer *c = data::headCustomer;
                int globalIndex = 0;    // Menghitung dari 0 sampai totalServices
                int displayedCount = 0; // Menghitung berapa baris yang sudah tampil di halaman ini

                int startIndex = currentPage * itemsPerPage;
                int endIndex = startIndex + itemsPerPage;

                while (c != NULL)
                {
                    data::Service *s = c->head_service;
                    while (s != NULL)
                    {
                        // Hanya render jika index masuk dalam rentang halaman saat ini
                        if (globalIndex >= startIndex && globalIndex < endIndex)
                        {
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("%s", s->id_service.c_str());

                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%s", c->nama.c_str());

                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("%s %s", s->merek_mobil.c_str(), s->model_mobil.c_str());

                            ImGui::TableSetColumnIndex(3);
                            ImGui::TextWrapped("%s", s->deskripsi_kendala.c_str());

                            ImGui::TableSetColumnIndex(4);
                            ImGui::TextColored(ImColor(COLOR_PRIMARY), "%s", s->nama_montir.c_str());

                            ImGui::TableSetColumnIndex(5);
                            ImVec2 badgePos = ImGui::GetCursorScreenPos();

                            // PERBAIKAN: Gunakan GetWindowDrawList agar tidak tembus saat di-scroll!
                            DrawBadge(ImGui::GetWindowDrawList(), badgePos, "Selesai", COLOR_BADGE_GRN, COLOR_TEXT_GRN);

                            displayedCount++;
                        }

                        globalIndex++;
                        s = s->next_in_customer;

                        // Optimasi: Jika sudah lewat endIndex, bisa break loop lebih cepat
                        // Tapi karena linked list kita berlapis, biarkan saja looping sampai habis
                    }
                    c = c->next;
                }

                if (totalServices == 0)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Belum ada data servis yang tercatat.");
                }

                ImGui::PopFont(); // Tutup fontRegular
                ImGui::EndTable();
            }
            ImGui::PopStyleVar(); // Tutup CellPadding

            // --- KONTROL PAGINASI ---
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Posisikan tombol paginasi di kanan
            ImVec2 buttonSize(80, 0);
            float availableWidth = ImGui::GetContentRegionAvail().x;

            // Info teks di kiri
            ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Menampilkan %d-%d dari %d servis",
                               (totalServices == 0) ? 0 : (currentPage * itemsPerPage) + 1,
                               ((currentPage + 1) * itemsPerPage > totalServices) ? totalServices : (currentPage + 1) * itemsPerPage,
                               totalServices);

            ImGui::SameLine(availableWidth - (buttonSize.x * 2) - 150);

            // --- PERBAIKAN TOMBOL PREV ---
            bool disablePrev = (currentPage <= 0); // Kunci kondisi saat ini
            if (disablePrev)
                ImGui::BeginDisabled();
            if (ImGui::Button("<< Prev", buttonSize))
            {
                currentPage--;
            }
            if (disablePrev)
                ImGui::EndDisabled(); // Gunakan variabel yang sama

            // Teks Halaman
            ImGui::SameLine();
            ImGui::Text(" Hal %d / %d ", currentPage + 1, totalPages);
            ImGui::SameLine();

            // --- PERBAIKAN TOMBOL NEXT ---
            bool disableNext = (currentPage >= totalPages - 1); // Kunci kondisi saat ini
            if (disableNext)
                ImGui::BeginDisabled();
            if (ImGui::Button("Next >>", buttonSize))
            {
                currentPage++;
            }
            if (disableNext)
                ImGui::EndDisabled(); // Gunakan variabel yang sama

            EndCard();
            break;
        }

        // ------------------------------------------------------------------
        // STATE: RIWAYAT KERJA MONTIR
        // ------------------------------------------------------------------
        case AppState::Services_History:
        {
            BeginCard("CardMontirHistory", ImVec2(1000, 600));

            ImGui::PushFont(fontMedium);
            ImGui::Text("Filter Riwayat Pekerjaan Berdasarkan Montir");
            ImGui::PopFont();
            ImGui::Separator();
            ImGui::Spacing();

            const char *list_montir[] = {"Suby", "Farhan", "Dimas", "Fajar"};
            ImGui::Text("Pilih Nama Montir:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);
            ImGui::Combo("##filterMontir", &filter_montir_idx, list_montir, IM_ARRAYSIZE(list_montir));

            ImGui::Spacing();
            ImGui::Spacing();
            std::string targetMontir = list_montir[filter_montir_idx];

            // Membangun tampilan list UI Card untuk setiap pekerjaan
            data::Customer *c = data::headCustomer;
            bool found = false;

            ImGui::BeginChild("ScrollArea", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

            // AMBIL DRAW LIST MILIK CHILD WINDOW (Bukan bg_draw_list)
            ImDrawList *draw_list = ImGui::GetWindowDrawList();

            while (c != NULL)
            {
                data::Service *s = c->head_service;
                while (s != NULL)
                {
                    if (s->nama_montir == targetMontir)
                    {
                        found = true;

                        // 1. Catat titik awal kursor
                        ImVec2 pJob = ImGui::GetCursorScreenPos();

                        // 2. BOOKING RUANG TERLEBIH DAHULU (Agar scrollbar mengenali tinggi card)
                        ImGui::Dummy(ImVec2(900, 110));

                        // Catat titik akhir setelah card (untuk jarak antar card nanti)
                        ImVec2 nextCardPos = ImGui::GetCursorScreenPos();

                        // 3. GAMBAR BACKGROUND KOTAK
                        draw_list->AddRectFilled(pJob, ImVec2(pJob.x + 900, pJob.y + 110), IM_COL32(248, 250, 252, 255), 10.0f);
                        draw_list->AddRect(pJob, ImVec2(pJob.x + 900, pJob.y + 110), COLOR_BORDER, 10.0f);
                        // Strip biru di kiri
                        draw_list->AddRectFilled(pJob, ImVec2(pJob.x + 8, pJob.y + 110), COLOR_PRIMARY, 10.0f, ImDrawFlags_RoundCornersLeft);

                        // 4. ISI TEKS KE DALAM KOTAK
                        ImGui::SetCursorScreenPos(ImVec2(pJob.x + 30, pJob.y + 15));
                        ImGui::PushFont(fontMedium);
                        ImGui::Text("%s %s", s->merek_mobil.c_str(), s->model_mobil.c_str());
                        ImGui::PopFont();

                        ImGui::SetCursorScreenPos(ImVec2(pJob.x + 30, pJob.y + 45));
                        ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Pemilik: %s | Telp: %s", c->nama.c_str(), c->nomor_telepon.c_str());

                        ImGui::SetCursorScreenPos(ImVec2(pJob.x + 30, pJob.y + 70));
                        ImGui::Text("Keluhan: %s", s->deskripsi_kendala.c_str());

                        ImGui::SetCursorScreenPos(ImVec2(pJob.x + 780, pJob.y + 15));
                        ImGui::TextColored(ImColor(COLOR_PRIMARY), "%s", s->id_service.c_str());

                        // 5. KEMBALIKAN KURSOR KE BAWAH CARD + BERI JARAK
                        ImGui::SetCursorScreenPos(nextCardPos);
                        ImGui::Dummy(ImVec2(0, 5)); // Margin bawah antar card
                    }
                    s = s->next_in_customer;
                }
                c = c->next;
            }

            if (!found)
            {
                ImGui::Dummy(ImVec2(0, 20)); // Tambah jarak sedikit biar rapi
                ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Montir '%s' belum memiliki riwayat pekerjaan.", targetMontir.c_str());
            }

            ImGui::EndChild();
            EndCard();
            break;
        }

        // ------------------------------------------------------------------
        // STATE: SEMUA DATA PELANGGAN
        // ------------------------------------------------------------------
        case AppState::AllCustomers:
        {
            BeginCard("CardAllCustomers", ImVec2(1000, 600));

            ImGui::PushFont(fontMedium);
            ImGui::Text("Database Pelanggan Lognuts");
            ImGui::PopFont();
            ImGui::Separator();
            ImGui::Spacing();

            ImGuiTableFlags table_flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY;
            if (ImGui::BeginTable("TableCustomers", 5, table_flags, ImVec2(0, 480)))
            {
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Nama Lengkap", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Telepon", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ImGui::TableSetupColumn("Gender", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Umur", ImGuiTableColumnFlags_WidthFixed, 60.0f);

                ImGui::PushFont(fontMedium);
                ImGui::TableHeadersRow();
                ImGui::PopFont();

                data::Customer *c = data::headCustomer;
                int rowCount = 0;

                while (c != NULL)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", c->id_customer.c_str());
                    ImGui::TableSetColumnIndex(1);

                    // Gambar icon avatar mini
                    ImVec2 pAv = ImGui::GetCursorScreenPos();
                    DrawAvatar(bg_draw_list, ImVec2(pAv.x + 15, pAv.y + 12), 12.0f, c->nama.c_str(), COLOR_PRIMARY);
                    ImGui::SetCursorScreenPos(ImVec2(pAv.x + 35, pAv.y + 4));
                    ImGui::Text("%s", c->nama.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s", c->nomor_telepon.c_str());
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%s", (c->gender == 'L' || c->gender == 'l') ? "Pria" : "Wanita");
                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text("%d thn", c->umur);

                    rowCount++;
                    c = c->next;
                }

                if (rowCount == 0)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Data pelanggan masih kosong.");
                }

                ImGui::EndTable();
            }

            EndCard();
            break;
        }

        // ------------------------------------------------------------------
        // STATE: DATA PELANGGAN (DETAIL & NAVIGASI NEXT/PREV)
        // ------------------------------------------------------------------
        case AppState::CustomerData:
        {
            if (current_detail_cust == nullptr)
            {
                BeginCard("ErrCard", ImVec2(600, 200));
                ImGui::TextColored(ImColor(COLOR_TEXT_RED), "Database Pelanggan Kosong.");
                EndCard();
                break;
            }

            BeginCard("CardCustomerProfile", ImVec2(900, 600));
            ImDrawList *child_draw_list = ImGui::GetWindowDrawList(); 

            // Bagian Atas: Profil Header
            ImVec2 pHeader = ImGui::GetCursorScreenPos();
            child_draw_list->AddRectFilled(pHeader, ImVec2(pHeader.x + 840, pHeader.y + 120), COLOR_PRIMARY, 16.0f);

            
            // Avatar Besar
            DrawAvatar(child_draw_list, ImVec2(pHeader.x + 70, pHeader.y + 60), 40.0f, current_detail_cust->nama.c_str(), COLOR_PRIMARY_DRK);

            // Info Nama
            ImGui::SetCursorScreenPos(ImVec2(pHeader.x + 130, pHeader.y + 30));
            ImGui::PushFont(fontTitle);
            ImGui::TextColored(ImColor(255, 255, 255, 255), "%s", current_detail_cust->nama.c_str());
            ImGui::PopFont();

            ImGui::SetCursorScreenPos(ImVec2(pHeader.x + 130, pHeader.y + 65));
            ImGui::PushFont(fontRegular);
            ImGui::TextColored(ImColor(255, 255, 255, 220), "ID: %s   |   No. Telp: %s   |   %d Thn (%c)",
                               current_detail_cust->id_customer.c_str(), current_detail_cust->nomor_telepon.c_str(), current_detail_cust->umur, current_detail_cust->gender);
            ImGui::PopFont();

            // Tombol Navigasi Kanan Atas
            ImGui::SetCursorScreenPos(ImVec2(pHeader.x + 630, pHeader.y + 40));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0.2f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));

            if (ImGui::Button("< Prev", ImVec2(80, 40)))
            {
                if (current_detail_cust->prev != NULL)
                    current_detail_cust = current_detail_cust->prev;
                else
                    ShowToast("Ini adalah data pelanggan pertama.");
            }
            ImGui::SameLine();
            if (ImGui::Button("Next >", ImVec2(80, 40)))
            {
                if (current_detail_cust->next != NULL)
                    current_detail_cust = current_detail_cust->next;
                else
                    ShowToast("Ini adalah data pelanggan terakhir.");
            }
            ImGui::PopStyleColor(2);

            ImGui::SetCursorScreenPos(ImVec2(pHeader.x, pHeader.y + 150));

            // Alamat Pelanggan
            ImGui::PushFont(fontMedium);
            ImGui::TextColored(ImColor(COLOR_TEXT_MAIN), "Alamat Lengkap");
            ImGui::PopFont();
            ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "%s", current_detail_cust->alamat.c_str());

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Daftar 3 Servis Terakhir
            ImGui::PushFont(fontMedium);
            ImGui::TextColored(ImColor(COLOR_TEXT_MAIN), "3 Riwayat Servis Terakhir");
            ImGui::PopFont();
            ImGui::Spacing();

            data::Service *s = current_detail_cust->head_service;
            int serviceCount = 0;

            ImGui::BeginChild("HistoryArea", ImVec2(0, 0), false);
            while (s != NULL && serviceCount < 3)
            {
                ImVec2 pHist = ImGui::GetCursorScreenPos();
                child_draw_list->AddRectFilled(pHist, ImVec2(pHist.x + 800, pHist.y + 80), IM_COL32(245, 248, 252, 255), 8.0f);

                // Indikator Garis Kiri
                child_draw_list->AddRectFilled(pHist, ImVec2(pHist.x + 6, pHist.y + 80), COLOR_PRIMARY, 8.0f, ImDrawFlags_RoundCornersLeft);

                ImGui::SetCursorScreenPos(ImVec2(pHist.x + 25, pHist.y + 15));
                ImGui::PushFont(fontMedium);
                ImGui::Text("%s %s", s->merek_mobil.c_str(), s->model_mobil.c_str());
                ImGui::PopFont();

                ImGui::SetCursorScreenPos(ImVec2(pHist.x + 25, pHist.y + 40));
                ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Montir: %s  |  Keluhan: %s", s->nama_montir.c_str(), s->deskripsi_kendala.c_str());

                ImGui::SetCursorScreenPos(ImVec2(pHist.x, pHist.y + 90));
                ImGui::Dummy(ImVec2(0.0f, 0.0f));

                s = s->next_in_customer;
                serviceCount++;
            }

            if (serviceCount == 0)
            {
                ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Pelanggan ini belum memiliki riwayat servis.");
            }
            ImGui::EndChild();

            EndCard();
            break;
        }
        }

        // 2.3 RENDER TOAST NOTIFICATION (Paling Atas)
        RenderToast();

        ImGui::EndChild(); // End Content Area
        ImGui::End();      // End MainLayout

        // ======================================================================
        // RENDERING OPENGL SELESAI
        // ======================================================================
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        ImVec4 clear_color = ImColor(COLOR_BG_APP); // Warna luar aplikasi (jika resize)
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // CLEANUP MEMORY
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}