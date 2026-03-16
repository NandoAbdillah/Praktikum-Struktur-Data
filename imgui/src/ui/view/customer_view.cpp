#include "imgui.h"
#include "imgui_internal.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "ui/style.h"
#include "utils/helper.h"
#include "controller/customer.h"
#include "controller/service.h"
#include "controller/mechanic.h"
#include <iostream>


extern std::string loggedInID;

namespace CustomerView
{
    void dashboard()
    {
        ImDrawList *bg_draw_list = ImGui::GetWindowDrawList();

        // Total Pelanggan
        ImGui::BeginGroup();
        ImVec2 pCard1 = ImGui::GetCursorScreenPos();
        ImVec2 szCard = ImVec2(350, 180);

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
        ImGui::TextColored(ImColor(255, 255, 255, 255), "%d Orang", customer::count());
        ImGui::PopFont();

        // Total Servis (Putih)
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
        // ImGui::Text("%d Unit", HitungTotalServis());
        ImGui::PopFont();

        ImGui::EndGroup();

        ImGui::SetCursorPos(ImVec2(40, 380));
        BeginCard("BannerCard", ImVec2(730, 250));
        ImGui::PushFont(fontTitle);
        ImGui::Text("Selamat Datang, Customer!");
        ImGui::PopFont();
        ImGui::Spacing();
        ImGui::PushFont(fontRegular);
        ImGui::TextWrapped("Sistem ini terhubung langsung dengan core C++ Linked-List. Semua data yang Anda masukkan (Servis Baru, Pelanggan Baru) akan langsung tersimpan di dalam memori dan diekspor ke file CSV secara otomatis. Silakan gunakan menu di sebelah kiri untuk menavigasi aplikasi.");
        ImGui::PopFont();
        EndCard();
    }

    // Real Customer View
    void showServiceQueue()
    {
        BeginCard("CardAllServicesQueue", ImVec2(1000, 600), false, false);

        ImGui::PushFont(fontMedium);
        ImGui::Text("Daftar Antrian Servis Global");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::Spacing();

        // std::string currentUser = loginName;

        int userQueuePositions[20];    
        std::string userCarBrands[20]; 
        int userActiveJobs = 0;

        
        int totalServices = 0;
        service::Service *count_s = service::front_queue;
        while (count_s != NULL)
        {
            totalServices++;

            if (count_s->cust_data != NULL && count_s->cust_data->id == loggedInID)
            {
                if (userActiveJobs < 20)
                {
                    userQueuePositions[userActiveJobs] = totalServices; 
                    userCarBrands[userActiveJobs] = count_s->brand + " " + count_s->model;
                    userActiveJobs++;
                }
            }

            count_s = count_s->next_q;
        }

        static int currentPage = 0;
        const int itemsPerPage = 10;
        int totalPages = (totalServices + itemsPerPage - 1) / itemsPerPage;
        if (totalPages == 0)
            totalPages = 1;

        if (currentPage >= totalPages)
            currentPage = totalPages - 1;
        if (currentPage < 0)
            currentPage = 0;

        ImGuiTableFlags table_flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY;
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));

        
        if (ImGui::BeginTable("TableServicesQueue", 7, table_flags, ImVec2(0, 360)))
        {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("No.", ImGuiTableColumnFlags_WidthFixed, 40.0f);
            ImGui::TableSetupColumn("Mobil", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Keluhan", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Montir", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Pelanggan & Telp", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Tgl Masuk", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 90.0f);

            ImGui::PushFont(fontMedium);
            ImGui::TableHeadersRow();
            ImGui::PopFont();

            ImGui::PushFont(fontRegular);


            service::Service *s = service::front_queue;
            int globalIndex = 0;
            int displayedCount = 0;

            int startIndex = currentPage * itemsPerPage;
            int endIndex = startIndex + itemsPerPage;

            while (s != NULL)
            {
                if (globalIndex >= startIndex && globalIndex < endIndex)
                {
                  
                    bool isMyCar = (s->cust_data != NULL && s->cust_data->id == loggedInID);
                    if (isMyCar)
                    {
                        ImU32 highlightColor = IM_COL32(230, 245, 255, 255); // Warna Biru Lembut
                        ImGui::TableNextRow(ImGuiTableRowFlags_None, 0.0f);
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, highlightColor);
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, highlightColor);
                    }
                    else
                    {
                        ImGui::TableNextRow();
                    }

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%d", globalIndex + 1);

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s %s", s->brand.c_str(), s->model.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextWrapped("%s", s->issue.c_str());

                    ImGui::TableSetColumnIndex(3);
                    ImGui::TextColored(ImColor(COLOR_PRIMARY), "%s", s->mech_data ? s->mech_data->name.c_str() : "-");

                    ImGui::TableSetColumnIndex(4);
                    const char *namaCust = s->cust_data ? s->cust_data->name.c_str() : "Unknown";
                    const char *telpCust = s->cust_data ? s->cust_data->phone.c_str() : "-";

                    // Kalau miliknya, font namanya jadi tebal (warna biru)
                    if (isMyCar)
                        ImGui::TextColored(ImColor(COLOR_PRIMARY), "%s\n(%s)", namaCust, telpCust);
                    else
                        ImGui::Text("%s\n(%s)", namaCust, telpCust);

                    ImGui::TableSetColumnIndex(5);
                    ImGui::Text("%s", s->date_in.c_str());

                    ImGui::TableSetColumnIndex(6);
                    ImVec2 badgePos = ImGui::GetCursorScreenPos();
                    DrawBadge(ImGui::GetWindowDrawList(), badgePos, "DIPROSES", COLOR_BADGE_ORG, COLOR_TEXT_ORG);

                    displayedCount++;
                }

                globalIndex++;
                s = s->next_q;
            }

            if (totalServices == 0)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(1);
                ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Antrian servis kosong.");
            }

            ImGui::PopFont();
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        
        if (userActiveJobs > 0)
        {
            ImGui::PushFont(fontMedium);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(COLOR_PRIMARY)));

            if (userActiveJobs == 1)
            {
                ImGui::TextWrapped("Servis kendaraan %s Anda berada di antrian ke-%d.",
                                   userCarBrands[0].c_str(), userQueuePositions[0]);
            }
            else
            {
                std::string multiText = "Anda memiliki " + std::to_string(userActiveJobs) + " kendaraan dalam antrian:\n";
                for (int i = 0; i < userActiveJobs; i++)
                {
                    multiText += "- " + userCarBrands[i] + " berada di antrian ke-" + std::to_string(userQueuePositions[i]) + "\n";
                }
                ImGui::TextWrapped("%s", multiText.c_str());
            }

            ImGui::PopStyleColor();
            ImGui::PopFont();
        }

    
        ImGui::Spacing();
        ImGui::Spacing();

        ImVec2 buttonSize(80, 0);
        float availableWidth = ImGui::GetContentRegionAvail().x;

        ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Menampilkan %d-%d dari %d antrian",
                           (totalServices == 0) ? 0 : (currentPage * itemsPerPage) + 1,
                           ((currentPage + 1) * itemsPerPage > totalServices) ? totalServices : (currentPage + 1) * itemsPerPage,
                           totalServices);

        ImGui::SameLine(availableWidth - (buttonSize.x * 2) - 150);

        bool disablePrev = (currentPage <= 0);
        if (disablePrev)
            ImGui::BeginDisabled();
        if (ImGui::Button("<< Prev", buttonSize))
            currentPage--;
        if (disablePrev)
            ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::Text(" Hal %d / %d ", currentPage + 1, totalPages);
        ImGui::SameLine();

        bool disableNext = (currentPage >= totalPages - 1);
        if (disableNext)
            ImGui::BeginDisabled();
        if (ImGui::Button("Next >>", buttonSize))
            currentPage++;
        if (disableNext)
            ImGui::EndDisabled();

        EndCard();
    }

    void showCustomerServices()
    {
        BeginCard("CardCustomerServices", ImVec2(1000, 600));

        ImGui::PushFont(fontMedium);
        ImGui::Text("Riwayat Servis Berdasarkan Pelanggan");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::Spacing();

      
        static char in_namaCustomer[128] = "";
        static bool hasCheckedCustomer = false;
        static std::string searchName = "";


        ImGui::PushFont(fontRegular);
        ImGui::Text("Nama Pelanggan (Pemilik)");
        ImGui::SetNextItemWidth(400);
        ImGui::InputText("##namacust", in_namaCustomer, IM_ARRAYSIZE(in_namaCustomer));
        ImGui::SameLine();

        if (ImGui::Button("Cari Riwayat", ImVec2(150, 42)))
        {
            hasCheckedCustomer = true;
            searchName = in_namaCustomer; // Simpan teks agar tidak berubah saat diketik ulang
        }
        ImGui::PopFont();

        ImGui::Spacing();
        ImGui::Spacing();

        if (hasCheckedCustomer && searchName != "")
        {
            // LOGIKA 1: Cari Pelanggan
            customer::Customer *c = customer::head;
            bool found = false;

            while (c != NULL)
            {
                if (c->name == searchName)
                {
                    found = true;
                    break;
                }
                c = c->next;
            }

            if (!found)
            {
                ImGui::TextColored(ImColor(255, 50, 50), "Pelanggan tidak ditemukan.");
            }
            else
            {

                ImGui::BeginChild("ScrollArea", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
                ImDrawList *draw_list = ImGui::GetWindowDrawList();

                // Cek layanan secara keseluruhan
                service::Service *s = c->head_service;
                if (s == NULL)
                {
                    ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Pelanggan belum pernah melakukan servis.");
                    ImGui::Spacing();
                }

                ImGui::PushFont(fontMedium);
                ImGui::Text("Servis yang sedang diproses:");
                ImGui::PopFont();
                ImGui::Spacing();

                service::Service *queueBantu = service::front_queue;

                if (queueBantu == NULL)
                {
                    ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Tidak ada servis yang sedang diproses.");
                }

                while (queueBantu != NULL)
                {
                    if (queueBantu->cust_data != NULL && queueBantu->cust_data->name == searchName)
                    {
                   
                        ImVec2 pJob = ImGui::GetCursorScreenPos();
                        ImGui::Dummy(ImVec2(900, 110));
                        ImVec2 nextCardPos = ImGui::GetCursorScreenPos();

                        
                        draw_list->AddRectFilled(pJob, ImVec2(pJob.x + 900, pJob.y + 110), IM_COL32(248, 250, 252, 255), 10.0f);
                        draw_list->AddRect(pJob, ImVec2(pJob.x + 900, pJob.y + 110), COLOR_BORDER, 10.0f);
                        draw_list->AddRectFilled(pJob, ImVec2(pJob.x + 8, pJob.y + 110), COLOR_BADGE_ORG, 10.0f, ImDrawFlags_RoundCornersLeft);

                       
                        ImGui::SetCursorScreenPos(ImVec2(pJob.x + 30, pJob.y + 15));
                        ImGui::PushFont(fontMedium);
                        ImGui::Text("%s %s", queueBantu->brand.c_str(), queueBantu->model.c_str());
                        ImGui::PopFont();

                        ImGui::SetCursorScreenPos(ImVec2(pJob.x + 30, pJob.y + 45));
                        ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Montir: %s | Tgl Masuk: %s", queueBantu->mech_data->name.c_str(), queueBantu->date_in.c_str());

                        ImGui::SetCursorScreenPos(ImVec2(pJob.x + 30, pJob.y + 70));
                        ImGui::Text("Keluhan: %s", queueBantu->issue.c_str());

                      
                        ImGui::SetCursorScreenPos(ImVec2(pJob.x + 780, pJob.y + 15));
                        DrawBadge(draw_list, ImGui::GetCursorScreenPos(), "DIPROSES", COLOR_BADGE_ORG, COLOR_TEXT_ORG);

                        ImGui::SetCursorScreenPos(nextCardPos);
                        ImGui::Dummy(ImVec2(0, 5));
                    }
                    queueBantu = queueBantu->next_q;
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

               
                // SERVIS YANG SUDAH SELESAI (Dari history customer)
                ImGui::PushFont(fontMedium);
                ImGui::Text("Servis yang sudah selesai:");
                ImGui::PopFont();
                ImGui::Spacing();

                while (s != NULL)
                {
                    
                    if (s->status == service::DONE)
                    {
                        ImVec2 pJob = ImGui::GetCursorScreenPos();
                        ImGui::Dummy(ImVec2(900, 110));
                        ImVec2 nextCardPos = ImGui::GetCursorScreenPos();

                        
                        draw_list->AddRectFilled(pJob, ImVec2(pJob.x + 900, pJob.y + 110), IM_COL32(248, 250, 252, 255), 10.0f);
                        draw_list->AddRect(pJob, ImVec2(pJob.x + 900, pJob.y + 110), COLOR_BORDER, 10.0f);
                        draw_list->AddRectFilled(pJob, ImVec2(pJob.x + 8, pJob.y + 110), COLOR_BADGE_GRN, 10.0f, ImDrawFlags_RoundCornersLeft);

                        ImGui::SetCursorScreenPos(ImVec2(pJob.x + 30, pJob.y + 15));
                        ImGui::PushFont(fontMedium);
                        ImGui::Text("%s %s", s->brand.c_str(), s->model.c_str());
                        ImGui::PopFont();

                        ImGui::SetCursorScreenPos(ImVec2(pJob.x + 30, pJob.y + 45));
                        ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Montir: %s | Tgl Masuk: %s", s->mech_data->name.c_str(), s->date_in.c_str());

                        ImGui::SetCursorScreenPos(ImVec2(pJob.x + 30, pJob.y + 70));
                        ImGui::Text("Keluhan: %s", s->issue.c_str());

                        // Badge Status Selesai
                        ImGui::SetCursorScreenPos(ImVec2(pJob.x + 780, pJob.y + 15));
                        DrawBadge(draw_list, ImGui::GetCursorScreenPos(), "SELESAI", COLOR_BADGE_GRN, COLOR_TEXT_GRN);

                        ImGui::SetCursorScreenPos(nextCardPos);
                        ImGui::Dummy(ImVec2(0, 5));
                    }
                    s = s->next_cust;
                }

                ImGui::EndChild();
            }
        }

        EndCard();
    }

    void bookingService()
    {
        BeginCard("FormBookingCard", ImVec2(900, 600), false, false);

        ImDrawList *child_draw_list = ImGui::GetWindowDrawList();

        ImGui::PushFont(fontTitle);
        ImGui::Text("Formulir Booking Servis Kendaraan");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::Spacing();

        customer::Customer *owner = customer::find(loggedInID);

        if (owner == NULL)
        {
            ImGui::TextColored(ImColor(255, 0, 0), "Error: Sesi login tidak valid atau data pelanggan tidak ditemukan!");
            EndCard();
            return;
        }

        ImGui::PushFont(fontMedium);
        ImGui::TextColored(ImColor(COLOR_PRIMARY), "Halo, %s!", owner->name.c_str());
        ImGui::PopFont();
        ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "ID: %s | Telp: %s", owner->id.c_str(), owner->phone.c_str());

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

       
        // FORM DATA KENDARAAN & KELUHAN
        static char in_merekMobil[128] = "";
        static char in_modelMobil[128] = "";
        static char in_kendala[512] = "";
        static int idx_montir = 0;

        ImGui::PushFont(fontMedium);
        ImGui::Text("Data Kendaraan & Keluhan");
        ImGui::PopFont();
        ImGui::Spacing();

       
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

        // DROPDOWN MONTIR
        mechanic::Mechanic *m_loop = mechanic::head;
        const char *list_montir_names[100];
        mechanic::Mechanic *arr_montir_ptrs[100];
        int count_montir = 0;

        while (m_loop != NULL && count_montir < 100)
        {
            list_montir_names[count_montir] = m_loop->name.c_str();
            arr_montir_ptrs[count_montir] = m_loop;
            m_loop = m_loop->next;
            count_montir++;
        }

        if (count_montir > 0)
        {
            ImGui::Combo("##montir", &idx_montir, list_montir_names, count_montir);
        }
        else
        {
            ImGui::TextColored(ImColor(255, 0, 0), "Tidak ada montir terdaftar!");
        }

        ImGui::Spacing();
        ImGui::Text("Deskripsi Keluhan / Kendala");
        ImGui::SetNextItemWidth(600);
        ImGui::InputTextMultiline("##kendala", in_kendala, IM_ARRAYSIZE(in_kendala), ImVec2(0, 80));

        ImGui::Spacing();
        ImGui::Spacing();


        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(COLOR_TEXT_GRN)));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.6f, 0.2f, 1.0f));

        if (ImGui::Button("Proses Booking Servis", ImVec2(250, 45)))
        {
            if (count_montir == 0)
            {
                ShowToast("Error: Tidak bisa menyimpan, montir kosong!");
            }
            else if (strlen(in_merekMobil) == 0 || strlen(in_modelMobil) == 0 || strlen(in_kendala) == 0)
            {
                ShowToast("Error: Harap isi semua form kendaraan dan keluhan!");
            }
            else
            {
                
                service::Service *s = new service::Service;

                s->prev = NULL;
                s->next = NULL;
                s->next_cust = NULL;
                s->next_q = NULL;
                s->next_hist = NULL;
                s->next_mech_q = NULL;
                s->next_stack = NULL;

                
                s->id_cust = owner->id;
                s->cust_data = owner;

               
                s->model = in_modelMobil;
                s->brand = in_merekMobil;
                s->issue = in_kendala;

                mechanic::Mechanic *montir_terpilih = arr_montir_ptrs[idx_montir];
                s->id_mech = montir_terpilih->id;
                s->mech_data = montir_terpilih;

                int totalServ = service::count();
                helper::generateID('S', totalServ, &s->id);

                s->date_in = helper::getCurrentDate();
                s->status = service::PROCESS;

                service::assign(owner, s, montir_terpilih);

                
                if (service::front_queue == NULL)
                {
                    service::front_queue = service::rear_queue = s;
                }
                else
                {
                    service::rear_queue->next_q = s;
                    service::rear_queue = s;
                }
                service::total_queue++;

                if (montir_terpilih->front_queue == NULL)
                {
                    montir_terpilih->front_queue = montir_terpilih->rear_queue = s;
                }
                else
                {
                    montir_terpilih->rear_queue->next_mech_q = s;
                    montir_terpilih->rear_queue = s;
                }

                
                service::save(s);

               
                std::string pesanToast = "Booking berhasil! Nomor antrian Anda adalah: " + std::to_string(service::total_queue);

                // RESET FORM
                in_merekMobil[0] = '\0';
                in_modelMobil[0] = '\0';
                in_kendala[0] = '\0';
                idx_montir = 0; 

                ShowToast(pesanToast.c_str());
            }
        }

        ImGui::PopStyleColor(2);
        EndCard();
    }

    void cancelService()
    {
        BeginCard("CardCancelService", ImVec2(900, 600), false, false);

        ImGui::PushFont(fontTitle);
        ImGui::Text("Manajemen Pembatalan Servis");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::Spacing();

        customer::Customer *owner = customer::find(loggedInID);

        if (owner == NULL)
        {
            ImGui::TextColored(ImColor(255, 0, 0), "Error: Data pelanggan tidak ditemukan atau sesi berakhir!");
            EndCard();
            return;
        }

        ImGui::PushFont(fontMedium);
        ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Kendaraan Anda yang sedang dalam antrean berjalan:");
        ImGui::PopFont();
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::BeginChild("ActiveServicesList", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImDrawList *draw_list = ImGui::GetWindowDrawList();

        
        service::Service *s = owner->head_service;
        int activeCount = 0;

        while (s != NULL)
        {
          
            if (s->status == service::PROCESS)
            {
                ImVec2 pCard = ImGui::GetCursorScreenPos();
                ImGui::Dummy(ImVec2(840, 100)); 
                ImVec2 nextPos = ImGui::GetCursorScreenPos();

                
                draw_list->AddRectFilled(pCard, ImVec2(pCard.x + 840, pCard.y + 100), IM_COL32(250, 250, 250, 255), 8.0f);
                draw_list->AddRect(pCard, ImVec2(pCard.x + 840, pCard.y + 100), COLOR_BORDER, 8.0f);

               
                draw_list->AddRectFilled(pCard, ImVec2(pCard.x + 8, pCard.y + 100), COLOR_BADGE_ORG, 8.0f, ImDrawFlags_RoundCornersLeft);

                
                ImGui::SetCursorScreenPos(ImVec2(pCard.x + 25, pCard.y + 15));
                ImGui::PushFont(fontMedium);
                ImGui::Text("%s %s", s->brand.c_str(), s->model.c_str());
                ImGui::PopFont();

                ImGui::SetCursorScreenPos(ImVec2(pCard.x + 25, pCard.y + 40));
                ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Keluhan: %s", s->issue.c_str());

                ImGui::SetCursorScreenPos(ImVec2(pCard.x + 25, pCard.y + 65));
                ImGui::TextColored(ImColor(COLOR_PRIMARY), "Montir: %s | ID Servis: %s", s->mech_data->name.c_str(), s->id.c_str());

            
                ImGui::SetCursorScreenPos(ImVec2(pCard.x + 680, pCard.y + 30));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(COLOR_TEXT_RED))); // Warna merah peringatan
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));

                std::string btnID = "Batalkan##" + s->id;
                if (ImGui::Button(btnID.c_str(), ImVec2(130, 40)))
                {

                    if (service::cancelService(s->id, loggedInID))
                    {
                        ShowToast("Booking berhasil dibatalkan dan dimasukkan ke dalam riwayat pembatalan!");
                    }
                    else
                    {
                        ShowToast("Error: Gagal membatalkan servis.");
                    }
                }
                ImGui::PopStyleColor(2);

                
                ImGui::SetCursorScreenPos(nextPos);
                ImGui::Dummy(ImVec2(0, 10));

                activeCount++;
            }
            s = s->next_cust; 
        }

        if (activeCount == 0)
        {
            ImGui::Dummy(ImVec2(0, 20));
            ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Anda tidak memiliki booking servis yang sedang diproses.");
        }

        ImGui::EndChild();
        EndCard();
    }

    
    void undoCancelService()
    {
        BeginCard("CardUndoCancel", ImVec2(900, 600), false, false);

        ImGui::PushFont(fontTitle);
        ImGui::Text("Riwayat Pembatalan (Undo Cancel)");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::PushFont(fontMedium);
        ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Daftar servis Anda yang telah dibatalkan (Terbaru di atas):");
        ImGui::PopFont();
        ImGui::Spacing();
        ImGui::Spacing();

        
        static std::string selected_cancel_id = "";
        static std::string selected_brand = "";
        static std::string selected_old_date = "";
        static char in_newDate[64] = "-";

        ImGui::BeginChild("CanceledServicesList", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImDrawList *draw_list = ImGui::GetWindowDrawList();

        service::Service *s = service::top_cancel_stack;
        int cancelCount = 0;

        while (s != NULL)
        {
            if (s->id_cust == loggedInID && s->status == service::CANCELED)
            {
                ImVec2 pCard = ImGui::GetCursorScreenPos();
                ImGui::Dummy(ImVec2(840, 100)); 
                ImVec2 nextPos = ImGui::GetCursorScreenPos();

                draw_list->AddRectFilled(pCard, ImVec2(pCard.x + 840, pCard.y + 100), IM_COL32(250, 250, 250, 255), 8.0f);
                draw_list->AddRect(pCard, ImVec2(pCard.x + 840, pCard.y + 100), COLOR_BORDER, 8.0f);

              
                draw_list->AddRectFilled(pCard, ImVec2(pCard.x + 8, pCard.y + 100), IM_COL32(150, 150, 150, 255), 8.0f, ImDrawFlags_RoundCornersLeft);

               
                ImGui::SetCursorScreenPos(ImVec2(pCard.x + 25, pCard.y + 15));
                ImGui::PushFont(fontMedium);
                ImGui::Text("%s %s", s->brand.c_str(), s->model.c_str());
                ImGui::PopFont();

                ImGui::SetCursorScreenPos(ImVec2(pCard.x + 25, pCard.y + 40));
                ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Dibatalkan pada antrean sebelumnya | ID: %s", s->id.c_str());

                ImGui::SetCursorScreenPos(ImVec2(pCard.x + 25, pCard.y + 65));
                ImGui::TextColored(ImColor(150, 150, 150, 255), "Tgl Booking Lama: %s", s->date_in.c_str());

                ImGui::SetCursorScreenPos(ImVec2(pCard.x + 680, pCard.y + 30));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(COLOR_PRIMARY)));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(COLOR_PRIMARY_DRK)));

                std::string btnID = "Undo Booking##" + s->id;
                if (ImGui::Button(btnID.c_str(), ImVec2(130, 40)))
                {
                    selected_cancel_id = s->id;
                    selected_brand = s->brand + " " + s->model;
                    selected_old_date = s->date_in;
                    strcpy(in_newDate, "-"); 

                    ImGui::OpenPopup("Konfirmasi Undo Cancel");
                }
                ImGui::PopStyleColor(2);

                ImGui::SetCursorScreenPos(nextPos);
                ImGui::Dummy(ImVec2(0, 10));

                cancelCount++;
            }
            s = s->next_stack; 
        }

        if (cancelCount == 0)
        {
            ImGui::Dummy(ImVec2(0, 20));
            ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Tidak ada riwayat pembatalan yang bisa dikembalikan.");
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Konfirmasi Undo Cancel", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::PushFont(fontMedium);
            ImGui::Text("Apakah anda ingin membooking kembali servis ini?");
            ImGui::PopFont();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Kendaraan  : %s", selected_brand.c_str());
            ImGui::Text("Tanggal Lama : %s", selected_old_date.c_str());
            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::Text("Apakah ingin di-reschedule? Input (-) jika tidak:");
            ImGui::SetNextItemWidth(250);
            ImGui::InputText("##newdate", in_newDate, IM_ARRAYSIZE(in_newDate));

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Tombol Undo
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(COLOR_TEXT_GRN)));
            if (ImGui::Button("YA (Booking Kembali)", ImVec2(180, 40)))
            {
                std::string newDateStr = in_newDate;


                if (service::undoCancelService(selected_cancel_id, loggedInID, newDateStr))
                {
                    std::string msg = "Servis " + selected_brand + " telah dibooking kembali!";
                    ShowToast(msg.c_str());
                }
                else
                {
                    ShowToast("Error: Gagal melakukan undo. Data tidak ditemukan di Stack.");
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleColor();

            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(COLOR_BORDER)));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(COLOR_TEXT_MAIN)));
            if (ImGui::Button("TIDAK (Batal)", ImVec2(100, 40)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleColor(2);

            ImGui::EndPopup();
        }

        ImGui::EndChild();
        EndCard();
    }

}