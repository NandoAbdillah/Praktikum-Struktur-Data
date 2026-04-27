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

#include "controller/customer.h"
#include "controller/service.h"
#include "controller/mechanic.h"

#include "utils/helper.h"
#include "assets/fonts/IconsFontAwesome7.h"

#include "ui/view/customer_view.h"
#include "ui/view/admin_view.h"

enum class AppState
{
    Dashboard,
    Services_All,
    Services_New,
    Services_History,
    Customer_Services_History,
    AllCustomers,
    CustomerData,
    Services_Queue,
    Finish_Service,
    Customer_New,
    Mechanic_New,
    Services_Booking,
    Services_Cancel,
    Services_Undo_Cancel,
    Reschedule_Service,
    Customer_Review
};

/* int HitungTotalServis()
{
    int total = 0;
    customer::Customer *c = customer::head;
    while (c != NULL)
    {
        service::Service *s = c->head_service;
        while (s != NULL)
        {
            total++;
            s = s->next_cust;
        }
        c = c->next;
    }
    return total;
}
    */

// VARIABEL MODE ADMIN
bool isAdminMode = false;
bool showAdminPopup = false;
char adminPassword[64] = "";
bool loginFailed = false;

//  VARIABEL LOGIN AWAL
bool isLoggedIn = false;
string loggedInID = "";
char loginName[128] = "";
char loginPhone[64] = "";
bool loginUserFailed = false;

int main()
{
    if (!customer::load())
        std::cout << "Warning: Gagal membuka customers.csv (Mungkin file belum ada)\n";
    if (!service::load())
        std::cout << "Warning: Gagal membuka services.csv (Mungkin file belum ada)\n";
    if (!mechanic::load())
        std::cout << "Warning: Gagal membuka mechanics.csv (Mungkin file belum ada)\n";
    if (!service::loadQueue())
        std::cout << "Warning: Gagal memuat antrian service\n";

    if (!glfwInit())
        return -1;
    GLFWwindow *window = glfwCreateWindow(1366, 768, "Lognuts Bengkel Management", NULL, NULL);
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

        if (!isLoggedIn)
        {
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 cardSize(545, 580);
            ImGui::SetCursorPos(ImVec2((windowSize.x - cardSize.x) * 0.5f, (windowSize.y - cardSize.y) * 0.5f));

            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 16.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30, 30));
            ImGui::BeginChild("LoginCard", cardSize, true, 0);

            ImGui::PushFont(fontTitle);
            ImGui::Text("Selamat Datang di Lognuts!");
            ImGui::PopFont();
            ImGui::Spacing();
            ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Silakan identifikasi diri Anda untuk melanjutkan.");
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::PushFont(fontRegular);
            ImGui::Text("Nama Lengkap / Kode Akses");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##loginName", loginName, IM_ARRAYSIZE(loginName));
            ImGui::Spacing();

            ImGui::Text("Nomor Telepon");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##loginPhone", loginPhone, IM_ARRAYSIZE(loginPhone));
            ImGui::PopFont();
            ImGui::Spacing();
            ImGui::Spacing();

            if (loginUserFailed)
            {
                ImGui::TextColored(ImColor(255, 50, 50), "Login gagal! Nama atau Nomor Telepon tidak terdaftar.");
                ImGui::Spacing();
            }
            else
            {
                ImGui::Dummy(ImVec2(0, ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemSpacing.y));
            }

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(COLOR_PRIMARY)));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(COLOR_PRIMARY_DRK)));
            if (ImGui::Button("Masuk Aplikasi", ImVec2(-1, 45)))
            {
                std::string inputName(loginName);
                std::string inputPhone(loginPhone);

                if (inputName == "adminacces8008")
                {
                    isAdminMode = true;
                    isLoggedIn = true;
                    currentState = AppState::Dashboard;
                    ShowToast("Berhasil masuk melalui jalur khusus Admin!");
                }
                else
                {
                    customer::Customer *c = customer::findByName(inputName);

                    if (c != NULL && c->phone == inputPhone)
                    {
                        isAdminMode = false;
                        isLoggedIn = true;
                        loggedInID = c->id;
                        currentState = AppState::Dashboard;
                        ShowToast("Selamat datang kembali, " + c->name + "!");
                    }
                    else
                    {
                        loginUserFailed = true;
                    }
                }
            }
            ImGui::PopStyleColor(2);

            ImGui::EndChild();
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor();
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(ImColor(COLOR_BG_SIDEBAR)));
            ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
            ImGui::BeginChild("Sidebar", ImVec2(300, 0), false);

            ImVec2 sbPos = ImGui::GetCursorScreenPos();
            ImDrawList *sbDraw = ImGui::GetWindowDrawList();

            ImGui::SetCursorPos(ImVec2(30, 40));
            sbDraw->AddRectFilled(ImVec2(sbPos.x + 30, sbPos.y + 40), ImVec2(sbPos.x + 65, sbPos.y + 75), COLOR_PRIMARY, 12.0f);

            sbDraw->AddLine(ImVec2(sbPos.x + 40, sbPos.y + 57), ImVec2(sbPos.x + 55, sbPos.y + 57), IM_COL32(255, 255, 255, 255), 4.0f);
            sbDraw->AddLine(ImVec2(sbPos.x + 40, sbPos.y + 50), ImVec2(sbPos.x + 50, sbPos.y + 50), IM_COL32(255, 255, 255, 200), 3.0f);

            sbDraw->AddText(fontTitle, 26.0f, ImVec2(sbPos.x + 80, sbPos.y + 42), COLOR_TEXT_MAIN, "Lognuts");
            sbDraw->AddText(fontSmall, 13.0f, ImVec2(sbPos.x + 82, sbPos.y + 68), COLOR_TEXT_MUTED, "Praktikum Struktur Data");

            ImGui::SetCursorPosY(130);

            if (isAdminMode)
            {
                // MAIN
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

                // SERVICES
                ImGui::SetCursorPosX(35);
                ImGui::PushFont(fontSmall);
                ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "MANAJEMEN SERVIS");
                ImGui::PopFont();
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

                ImGui::SetCursorPosX(30);
                if (DrawSidebarMenuPill(ICON_FA_CAR_REAR, "Semua Servis", currentState == AppState::Services_All))
                {
                    currentState = AppState::Services_All;
                }

                ImGui::SetCursorPosX(30);

                if (DrawSidebarMenuPill(ICON_FA_SQUARE_PLUS, "Servis Baru", currentState == AppState::Services_New))
                {
                    currentState = AppState::Services_New;
                }

                ImGui::SetCursorPosX(30);
                if (DrawSidebarMenuPill(ICON_FA_USER_GEAR, "Riwayat Montir", currentState == AppState::Services_History))
                {
                    currentState = AppState::Services_History;
                }

                ImGui::SetCursorPosX(30);
                if (DrawSidebarMenuPill(ICON_FA_CALENDAR_DAY, "Reschedule Servis", currentState == AppState::Reschedule_Service))
                {
                    currentState = AppState::Reschedule_Service;
                }

                ImGui::SetCursorPosX(30);
                if (DrawSidebarMenuPill(ICON_FA_CLIPBOARD_CHECK, "Selesaikan Pekerjaan", currentState == AppState::Finish_Service))
                {
                    currentState = AppState::Finish_Service;
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
                if (DrawSidebarMenuPill(ICON_FA_USER_PLUS, "Pelanggan Baru", currentState == AppState::Customer_New))
                {
                    currentState = AppState::Customer_New;
                }

                ImGui::SetCursorPosX(30);
                if (DrawSidebarMenuPill(ICON_FA_USER, "Profil & Detail", currentState == AppState::CustomerData))
                {
                    currentState = AppState::CustomerData;
                }

                // 1.5 Kategori: MECHANIC
                ImGui::SetCursorPosX(35);
                ImGui::PushFont(fontSmall);
                ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "MONTIR");
                ImGui::PopFont();
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

                ImGui::SetCursorPosX(30);
                if (DrawSidebarMenuPill(ICON_FA_USER_TIE, "Montir Baru", currentState == AppState::Mechanic_New))
                {
                    currentState = AppState::Mechanic_New;
                }
            }
            else
            {
                // MENU CUSTOMER
                // MAIN
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

                // SERVICES
                ImGui::SetCursorPosX(35);
                ImGui::PushFont(fontSmall);
                ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "SERVIS");
                ImGui::PopFont();
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

                ImGui::SetCursorPosX(30);

                if (DrawSidebarMenuPill(ICON_FA_TICKET_SIMPLE, "Antrian Servis", currentState == AppState::Services_Queue))
                {
                    currentState = AppState::Services_Queue;
                }

                ImGui::SetCursorPosX(30);
                if (DrawSidebarMenuPill(ICON_FA_CALENDAR_PLUS, "Booking Servis", currentState == AppState::Services_Booking))
                {
                    currentState = AppState::Services_Booking;
                }

                ImGui::SetCursorPosX(30);
                if (DrawSidebarMenuPill(ICON_FA_CALENDAR_XMARK, "Batalkan Servis", currentState == AppState::Services_Cancel))
                {
                    currentState = AppState::Services_Cancel;
                }

                ImGui::SetCursorPosX(30);
                if (DrawSidebarMenuPill(ICON_FA_CLOCK_ROTATE_LEFT, "Undo Pembatalan Servis", currentState == AppState::Services_Undo_Cancel))
                {
                    currentState = AppState::Services_Undo_Cancel;
                }
                
                ImGui::SetCursorPosX(30);
                if (DrawSidebarMenuPill(ICON_FA_STAR_AND_CRESCENT, "Review & Rating", currentState == AppState::Customer_Review))
                {
                    currentState = AppState::Customer_Review;
                }

                ImGui::SetCursorPosX(30);
                if (DrawSidebarMenuPill(ICON_FA_CAR_REAR, "Riwayat Servis", currentState == AppState::Customer_Services_History))
                {
                    currentState = AppState::Customer_Services_History;
                }


            }

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
            ImGui::SetCursorPos(ImVec2(30, viewport->WorkSize.y - 140));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(COLOR_BADGE_ORG)));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(COLOR_TEXT_ORG)));
            if (ImGui::Button("Logout Akun", ImVec2(230, 45)))
            {
                isAdminMode = false;
                isLoggedIn = false;
                ShowToast("Berhasil keluar dari akun. Sampai jumpa lagi!");
            }
            ImGui::PopStyleColor(2);

            // Exit
            ImGui::SetCursorPos(ImVec2(30, viewport->WorkSize.y - 80));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0.9f, 0.9f, 1));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.2f, 0.2f, 1));
            if (ImGui::Button("Keluar Aplikasi", ImVec2(230, 45)))
            {
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::PopStyleColor(2);

            ImGui::EndChild();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            ImGui::SameLine(0, 0);

            // AREA KONTEN UTAMA
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(ImColor(COLOR_BG_APP)));
            ImGui::BeginChild("ContentArea", ImVec2(0, 0), false);
            ImGui::PopStyleColor();

            ImDrawList *bg_draw_list = ImGui::GetWindowDrawList();

            // HEADER
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

            case AppState::Customer_Services_History:
                pageTitle = "Riwayat Servis Pelanggan";
                pageSub = "Lihat rekam jejak servis kendaraan Anda.";
                break;

            case AppState::AllCustomers:
                pageTitle = "Direktori Pelanggan";
                pageSub = "Database seluruh pelanggan terdaftar.";
                break;

            case AppState::CustomerData:
                pageTitle = "Profil Pelanggan";
                pageSub = "Detail informasi dan riwayat kedatangan.";
                break;

            case AppState::Services_Queue:
                pageTitle = "Antrian Servis";
                pageSub = "Lihat status antrean kendaraan Anda.";
                break;

            case AppState::Finish_Service:
                pageTitle = "Selesaikan Pekerjaan";
                pageSub = "Tandai servis sebagai selesai.";
                break;

            case AppState::Mechanic_New:
                pageTitle = "Registrasi Montir";
                pageSub = "Input data mekanik baru untuk bergabung.";
                break;

            case AppState::Customer_New:
                pageTitle = "Registrasi Pelanggan Baru";
                pageSub = "Input data pelanggan baru untuk bergabung.";
                break;

            case AppState::Services_Booking:
                pageTitle = "Booking Servis";
                pageSub = "Jadwalkan servis kendaraan Anda.";
                break;

            case AppState::Services_Cancel:
                pageTitle = "Batalkan Servis";
                pageSub = "Batalkan jadwal servis yang sudah dibuat.";
                break;

            case AppState::Services_Undo_Cancel:
                pageTitle = "Undo Pembatalan Servis";
                pageSub = "Pulihkan jadwal servis yang batal.";
                break;

            case AppState::Reschedule_Service:
                pageTitle = "Reschedule Servis";
                pageSub = "Atur ulang jadwal servis Anda.";
                break;


            case AppState::Customer_Review:
                pageTitle = "Review & Rating";
                pageSub = "Berikan penilaian untuk layanan kami.";
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

            // KONTEN STATE
            switch (currentState)
            {

            // STATE: DASHBOARD
            case AppState::Dashboard:
            {
                CustomerView::dashboard();
                break;
            }

            // STATE: SERVIS BARU
            case AppState::Services_New:
            {
                AdminView::formNewService();
                break;
            }

            // STATE: SEMUA SERVIS
            case AppState::Services_All:
            {
                AdminView::showAllServices();
                break;
            }

            // STATE: RIWAYAT KERJA MONTIR
            case AppState::Services_History:
            {
                AdminView::showServiceHistory();
                break;
            }

            // STATE: SEMUA DATA PELANGGAN
            case AppState::AllCustomers:
            {
                AdminView::showAllCustomers();
                break;
            }

            case AppState::Finish_Service:
            {
                AdminView::finishService();
                break;
            }

            // STATE: DATA PELANGGAN
            case AppState::CustomerData:
            {
                AdminView::showCustomerData();
                break;
            }

            // STATE: ANTRIAN SERVIS
            case AppState::Services_Queue:
            {
                CustomerView::showServiceQueue();
                break;
            }

            // STATE: RIWAYAT SERVIS PELANGGAN
            case AppState::Customer_Services_History:
            {
                CustomerView::showCustomerServices();
                break;
            }

            // STATE: REGISTRASI MONTIR BARU
            case AppState::Mechanic_New:
            {
                AdminView::formNewMechanic();
                break;
            }

            // STATE: REGISTRASI PELANGGAN BARU
            case AppState::Customer_New:
            {
                AdminView::formNewCustomer();
                break;
            }

             case AppState::Reschedule_Service:
            {
                AdminView::rescheduleService();
                break;
            }

            // STATE: BOOKING SERVIS
            case AppState::Services_Booking:
            {
                CustomerView::bookingService();
                break;
            }

            // STATE: PEMBATALAN SERVIS
            case AppState::Services_Cancel:
            {
                CustomerView::cancelService();
                break;
            }

            // STATE: UNDO PEMBATALAN SERVIS
            case AppState::Services_Undo_Cancel:
            {
                CustomerView::undoCancelService();
                break;
            }

            // STATE: REVIEW & RATING
            case AppState::Customer_Review:
            {
                CustomerView::reviewService();
                break;
            }

            }

            RenderToast();

            ImGui::EndChild(); // End Content Area
        }

        ImGui::End(); // End MainLayout

        // RENDERING OPENGL SELESAI
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        ImVec4 clear_color = ImColor(COLOR_BG_APP);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}