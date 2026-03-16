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

namespace AdminView
{

    static char in_namaCustomer[128] = "";
    static bool customerFound = false;
    static bool hasCheckedCustomer = false;
    static customer::Customer *foundCustomerPtr = nullptr;

    static char in_telpCustomer[14] = "";
    static char in_alamatCustomer[256] = "";
    static int in_umurCustomer = 20;
    static int in_genderCustomer = 0; // 0 = L, 1 = P

    static char in_modelMobil[128] = "";
    static char in_merekMobil[128] = "";
    static char in_kendala[512] = "";
    static int idx_montir = 0;

    // Filter History Montir
    static int filter_montir_idx = 0;

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

        // Welcome Banner
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
    }

    void formNewService()
    {
        BeginCard("FormServisCard", ImVec2(900, 600), false, false);

        ImDrawList *child_draw_list = ImGui::GetWindowDrawList();

        ImGui::PushFont(fontRegular);

        ImGui::PushFont(fontMedium);
        ImGui::Text("Data Kendaraan & Kendala");
        ImGui::PopFont();
        ImGui::Separator();
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

    //   MONTIR
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


        ImGui::PushFont(fontMedium);
        ImGui::Text("Data Pelanggan");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::Spacing();

        // Input Nama Customer
        ImGui::Text("Nama Pelanggan (Pemilik)");
        ImGui::SetNextItemWidth(400);
        ImGui::InputText("##namacust", in_namaCustomer, IM_ARRAYSIZE(in_namaCustomer));
        ImGui::SameLine();

        if (ImGui::Button("Cek Pelanggan", ImVec2(150, 42)))
        {
            hasCheckedCustomer = true;
            std::string searchName = in_namaCustomer;

            customer::Customer *c_found = customer::findByName(searchName);
            if (c_found == NULL)
            {
                customerFound = false;
                foundCustomerPtr = nullptr;
            }
            else
            {
                customerFound = true;
                foundCustomerPtr = c_found;
            }
        }

        ImGui::Spacing();
        ImGui::Spacing();


        if (hasCheckedCustomer)
        {
            if (customerFound && foundCustomerPtr)
            {
                ImVec2 pBadge = ImGui::GetCursorScreenPos();
                DrawBadge(child_draw_list, pBadge, "Pelanggan Terdaftar Ditemukan!", COLOR_BADGE_GRN, COLOR_TEXT_GRN);
                ImGui::Dummy(ImVec2(0, 30));

                ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Data ID: %s | Telp: %s", foundCustomerPtr->id.c_str(), foundCustomerPtr->phone.c_str());
                ImGui::Spacing();
                ImGui::Spacing();
            }
            else
            {
                ImVec2 pBadge = ImGui::GetCursorScreenPos();
                DrawBadge(child_draw_list, pBadge, "Pelanggan Baru - Harap Lengkapi Data", COLOR_BADGE_ORG, COLOR_TEXT_ORG);
                ImGui::Dummy(ImVec2(0, 30));

                ImGui::BeginGroup(); 
                ImGui::Text("Nomor Telepon");
                ImGui::SetNextItemWidth(250);
                ImGui::InputText("##telp", in_telpCustomer, IM_ARRAYSIZE(in_telpCustomer));

                ImGui::Spacing();
                ImGui::Text("Umur");
                ImGui::SetNextItemWidth(150);
                ImGui::InputInt("##umur", &in_umurCustomer);
                ImGui::EndGroup();

                ImGui::SameLine(350);

                ImGui::BeginGroup();
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

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(COLOR_TEXT_GRN)));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.6f, 0.2f, 1.0f));

            if (ImGui::Button("Proses Data Servis", ImVec2(250, 45)))
            {
                if (count_montir == 0)
                {
                    ShowToast("Error: Tidak bisa menyimpan, montir kosong!");
                }
                else
                {
                    std::string finalCustName = in_namaCustomer;
                    customer::Customer *owner = customer::findByName(finalCustName);

                    if (owner == NULL)
                    {
                        customer::Customer newCust;
                        newCust.name = finalCustName;
                        newCust.phone = in_telpCustomer;
                        newCust.address = in_alamatCustomer;
                        newCust.age = in_umurCustomer;
                        newCust.gender = (in_genderCustomer == 0) ? 'L' : 'P';


                        newCust.prev = NULL;
                        newCust.next = NULL;
                        newCust.head_service = NULL;

                        int totalCust = customer::count();
                        helper::generateID('C', totalCust, &newCust.id);

                        customer::insert(newCust);
                        customer::save(newCust);

                        
                        owner = customer::findByName(finalCustName);
                    }

                    
                    service::Service *s = new service::Service;

                    s->prev = NULL;
                    s->next = NULL;
                    s->next_cust = NULL;
                    s->next_q = NULL;
                    s->next_hist = NULL;
                    s->next_mech_q = NULL;

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
                    service::enqueueService(s);
                    service::save(s);

                    cout << "Service Queue" << owner->name << ": " << service::total_queue << endl;
                    string pesanToast = "Servis sudah tercatat, nomor antrian " + owner->name + " adalah: " + to_string(service::total_queue);

                    // RESET FORM
                    in_namaCustomer[0] = '\0';
                    in_telpCustomer[0] = '\0';
                    in_alamatCustomer[0] = '\0';
                    in_umurCustomer = 20;
                    in_genderCustomer = 0;
                    in_merekMobil[0] = '\0';
                    in_modelMobil[0] = '\0';
                    in_kendala[0] = '\0';
                    hasCheckedCustomer = false;

                    ShowToast(pesanToast.c_str());
                }
            }

            ImGui::PopStyleColor(2);
        } // End of hasCheckedCustomer

        ImGui::PopFont(); // Menutup PushFont(fontRegular) global di awal
        EndCard();
    }

    void showAllServices()
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

        int totalServices = 0;
        service::Service *count_s = service::head;
        while (count_s != NULL)
        {
            totalServices++;
            count_s = count_s->next; 
        }

        int totalPages = (totalServices + itemsPerPage - 1) / itemsPerPage;
        if (totalPages == 0)
            totalPages = 1; 

        if (currentPage >= totalPages)
            currentPage = totalPages - 1;
        if (currentPage < 0)
            currentPage = 0;

        ImGuiTableFlags table_flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY;

        
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));

        if (ImGui::BeginTable("TableServices", 6, table_flags, ImVec2(0, 420)))
        {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableSetupColumn("Pelanggan & Telp", ImGuiTableColumnFlags_WidthStretch); // Gabungan sesuai cout
            ImGui::TableSetupColumn("Kendaraan", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Keluhan", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Montir", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 90.0f);

            ImGui::PushFont(fontMedium);
            ImGui::TableHeadersRow();
            ImGui::PopFont();

           
            ImGui::PushFont(fontRegular);

           
            service::Service *s = service::head;
            int globalIndex = 0;
            int displayedCount = 0;

            int startIndex = currentPage * itemsPerPage;
            int endIndex = startIndex + itemsPerPage;

            while (s != NULL)
            {
                if (globalIndex >= startIndex && globalIndex < endIndex)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", s->id.c_str());

                    ImGui::TableSetColumnIndex(1);
                    const char *custName = s->cust_data ? s->cust_data->name.c_str() : "Unknown";
                    const char *custPhone = s->cust_data ? s->cust_data->phone.c_str() : "-";
                    ImGui::Text("%s\n(%s)", custName, custPhone);

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s %s", s->brand.c_str(), s->model.c_str());

                    ImGui::TableSetColumnIndex(3);
                    ImGui::TextWrapped("%s", s->issue.c_str());

                    ImGui::TableSetColumnIndex(4);
                    ImGui::TextColored(ImColor(COLOR_PRIMARY), "%s", s->mech_data ? s->mech_data->name.c_str() : "Unknown");

                    ImGui::TableSetColumnIndex(5);
                    ImVec2 badgePos = ImGui::GetCursorScreenPos();

                    
                    if (s->status == service::PROCESS)
                    {
                        DrawBadge(ImGui::GetWindowDrawList(), badgePos, "DIPROSES", COLOR_BADGE_ORG, COLOR_TEXT_ORG);
                    }
                    else
                    {
                        DrawBadge(ImGui::GetWindowDrawList(), badgePos, "SELESAI", COLOR_BADGE_GRN, COLOR_TEXT_GRN);
                    }

                    displayedCount++;
                }

                globalIndex++;
                s = s->next; 
            }

            if (totalServices == 0)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(1);
                ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Data service kosong.");
            }

            ImGui::PopFont(); 
            ImGui::EndTable();
        }
        ImGui::PopStyleVar(); 

       
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImVec2 buttonSize(80, 0);
        float availableWidth = ImGui::GetContentRegionAvail().x;

        ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Menampilkan %d-%d dari %d servis",
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

    void showServiceHistory()
    {

        BeginCard("CardMontirHistory", ImVec2(1000, 600));

        ImGui::PushFont(fontMedium);
        ImGui::Text("Filter Riwayat Pekerjaan Berdasarkan Montir");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Pilih Nama Montir:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);

        const char *list_montir_names[100];
        int mechanic_count = 0;

        for (int i = 0; i < 100 && mechanic::list_mechanics[i] != nullptr; i++)
        {
            list_montir_names[i] = mechanic::list_mechanics[i]->name.c_str();
            mechanic_count++;
        }
        if (mechanic_count > 0)
        {
            ImGui::Combo("##filterMontir", &filter_montir_idx, list_montir_names, mechanic_count);
        }
        else
        {
            ImGui::TextColored(ImColor(255, 0, 0), "Data montir kosong!");
        }

        ImGui::Spacing();
        ImGui::Spacing();

        mechanic::Mechanic *targetMontir = nullptr;
        if (filter_montir_idx >= 0 && filter_montir_idx < mechanic_count)
        {
            targetMontir = mechanic::list_mechanics[filter_montir_idx];
        }

   
        customer::Customer *c = customer::head;
        bool found = false;

        ImGui::BeginChild("ScrollArea", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        //
        ImDrawList *draw_list = ImGui::GetWindowDrawList();

        while (c != NULL)
        {
            service::Service *s = c->head_service;
            while (s != NULL)
            {
                if (s->mech_data->id == targetMontir->id)
                {
                    // cout << "DEBUG: Menampilkan service ID " << s->id << " untuk montir " << targetMontir->name << endl;
                    found = true;

                   
                    ImVec2 pJob = ImGui::GetCursorScreenPos();

                 
                    ImGui::Dummy(ImVec2(900, 110));

                   
                    ImVec2 nextCardPos = ImGui::GetCursorScreenPos();

                  
                    draw_list->AddRectFilled(pJob, ImVec2(pJob.x + 900, pJob.y + 110), IM_COL32(248, 250, 252, 255), 10.0f);
                    draw_list->AddRect(pJob, ImVec2(pJob.x + 900, pJob.y + 110), COLOR_BORDER, 10.0f);
                  
                    draw_list->AddRectFilled(pJob, ImVec2(pJob.x + 8, pJob.y + 110), COLOR_PRIMARY, 10.0f, ImDrawFlags_RoundCornersLeft);

                   
                    ImGui::SetCursorScreenPos(ImVec2(pJob.x + 30, pJob.y + 15));
                    ImGui::PushFont(fontMedium);
                    ImGui::Text("%s %s", s->brand.c_str(), s->model.c_str());
                    ImGui::PopFont();

                    ImGui::SetCursorScreenPos(ImVec2(pJob.x + 30, pJob.y + 45));
                    ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Pemilik: %s | Telp: %s", c->name.c_str(), c->phone.c_str());

                    ImGui::SetCursorScreenPos(ImVec2(pJob.x + 30, pJob.y + 70));
                    ImGui::Text("Keluhan: %s", s->issue.c_str());

                    ImGui::SetCursorScreenPos(ImVec2(pJob.x + 780, pJob.y + 15));
                    ImGui::TextColored(ImColor(COLOR_PRIMARY), "%s", s->id.c_str());

                  
                    ImGui::SetCursorScreenPos(nextCardPos);
                    ImGui::Dummy(ImVec2(0, 5)); 
                }
                s = s->next_cust;
            }
            c = c->next;
        }

        if (!found)
        {
            ImGui::Dummy(ImVec2(0, 20)); 
            ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Montir '%s' belum memiliki riwayat pekerjaan.", targetMontir->name.c_str());
        }

        ImGui::EndChild();
        EndCard();
    }

    void showAllCustomers()
    {
        ImDrawList *bg_draw_list = ImGui::GetWindowDrawList();

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

            customer::Customer *c = customer::head;
            int rowCount = 0;

            while (c != NULL)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", c->id.c_str());
                ImGui::TableSetColumnIndex(1);

                // Gambar icon avatar mini
                ImVec2 pAv = ImGui::GetCursorScreenPos();
                DrawAvatar(bg_draw_list, ImVec2(pAv.x + 15, pAv.y + 12), 12.0f, c->name.c_str(), COLOR_PRIMARY);
                ImGui::SetCursorScreenPos(ImVec2(pAv.x + 35, pAv.y + 4));
                ImGui::Text("%s", c->name.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", c->phone.c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%s", (c->gender == 'L' || c->gender == 'l') ? "Pria" : "Wanita");
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%d thn", c->age);

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
    }

    void showCustomerData()
    {
        static customer::Customer *current_detail_cust = customer::head;

     
        if (current_detail_cust == nullptr)
            current_detail_cust = customer::head;

        if (current_detail_cust == nullptr)
        {
            BeginCard("ErrCard", ImVec2(600, 200));
            ImGui::TextColored(ImColor(COLOR_TEXT_RED), "Database Pelanggan Kosong.");
            EndCard();
            return;
        }

        BeginCard("CardCustomerProfile", ImVec2(900, 600));

      
        ImDrawList *parent_draw_list = ImGui::GetWindowDrawList();

      
        ImVec2 pHeader = ImGui::GetCursorScreenPos();
        parent_draw_list->AddRectFilled(pHeader, ImVec2(pHeader.x + 840, pHeader.y + 120), COLOR_PRIMARY, 16.0f);

        DrawAvatar(parent_draw_list, ImVec2(pHeader.x + 70, pHeader.y + 60), 40.0f, current_detail_cust->name.c_str(), COLOR_PRIMARY_DRK);

        ImGui::SetCursorScreenPos(ImVec2(pHeader.x + 130, pHeader.y + 30));
        ImGui::PushFont(fontTitle);
        ImGui::TextColored(ImColor(255, 255, 255, 255), "%s", current_detail_cust->name.c_str());
        ImGui::PopFont();

        ImGui::SetCursorScreenPos(ImVec2(pHeader.x + 130, pHeader.y + 65));
        ImGui::PushFont(fontRegular);
        ImGui::TextColored(ImColor(255, 255, 255, 220), "ID: %s   |   No. Telp: %s   |   %d Thn (%c)",
                           current_detail_cust->id.c_str(), current_detail_cust->phone.c_str(), current_detail_cust->age, current_detail_cust->gender);
        ImGui::PopFont();

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

        ImGui::PushFont(fontMedium);
        ImGui::TextColored(ImColor(COLOR_TEXT_MAIN), "Alamat Lengkap");
        ImGui::PopFont();
        ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "%s", current_detail_cust->address.c_str());

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::PushFont(fontMedium);
        ImGui::TextColored(ImColor(COLOR_TEXT_MAIN), "3 Riwayat Servis Terakhir");
        ImGui::PopFont();
        ImGui::Spacing();

        service::Service *s = current_detail_cust->head_service;
        int serviceCount = 0;

       
        ImGui::BeginChild("HistoryArea", ImVec2(0, 0), false);

        ImDrawList *child_draw_list = ImGui::GetWindowDrawList();

        while (s != NULL && serviceCount < 3)
        {
            ImVec2 pHist = ImGui::GetCursorScreenPos();

            ImGui::Dummy(ImVec2(800, 80));
            ImVec2 nextCardPos = ImGui::GetCursorScreenPos(); 

            child_draw_list->AddRectFilled(pHist, ImVec2(pHist.x + 800, pHist.y + 80), IM_COL32(245, 248, 252, 255), 8.0f);

            child_draw_list->AddRectFilled(pHist, ImVec2(pHist.x + 6, pHist.y + 80), COLOR_PRIMARY, 8.0f, ImDrawFlags_RoundCornersLeft);

            ImGui::SetCursorScreenPos(ImVec2(pHist.x + 25, pHist.y + 15));
            ImGui::PushFont(fontMedium);
            ImGui::Text("%s %s", s->brand.c_str(), s->model.c_str());
            ImGui::PopFont();

            ImGui::SetCursorScreenPos(ImVec2(pHist.x + 25, pHist.y + 40));
            ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Montir: %s  |  Keluhan: %s", s->mech_data->name.c_str(), s->issue.c_str());

            
            ImGui::SetCursorScreenPos(nextCardPos);
            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            s = s->next_cust;
            serviceCount++;
        }

        if (serviceCount == 0)
        {
            ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Pelanggan ini belum memiliki riwayat servis.");
        }
        ImGui::EndChild();

        EndCard();
    }

    void finishService()
    {
        BeginCard("FinishServiceCard", ImVec2(900, 600));

        ImGui::PushFont(fontTitle);
        ImGui::Text("Selesaikan Servis Kendaraan");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::Spacing();

        // LIST MONTIR YANG AKTIF
        mechanic::Mechanic *active_mechanics[100];
        int active_count = 0;

        mechanic::Mechanic *m_loop = mechanic::head;
        while (m_loop != NULL)
        {
            // hanya masuk dropdown jika punya antrian servis yang diproses
            if (m_loop->front_queue != NULL)
            {
                active_mechanics[active_count] = m_loop;
                active_count++;
            }
            m_loop = m_loop->next;
        }

        // state untuk dropdwon
        static int selected_idx = 0;

        if (selected_idx >= active_count)
            selected_idx = 0;

        //  DROPDOWN
        ImGui::PushFont(fontMedium);
        ImGui::Text("Pilih Montir yang akan menyelesaikan service:");
        ImGui::PopFont();

        ImGui::SetNextItemWidth(300);
        if (active_count == 0)
        {
            ImGui::BeginDisabled();
            if (ImGui::BeginCombo("##montirSelect", "Tidak ada servis diproses..."))
            {
                ImGui::EndCombo();
            }
            ImGui::EndDisabled();

            ImGui::Spacing();
            ImGui::TextColored(ImColor(COLOR_TEXT_MUTED), "Semua montir sedang nganggur. Tidak ada antrian yang berjalan.");
        }
        else
        {
            // Jika ada montir yang sedang bekerja
            const char *preview_value = active_mechanics[selected_idx]->name.c_str();
            if (ImGui::BeginCombo("##montirSelect", preview_value))
            {
                for (int i = 0; i < active_count; i++)
                {
                    const bool is_selected = (selected_idx == i);
                    if (ImGui::Selectable(active_mechanics[i]->name.c_str(), is_selected))
                    {
                        selected_idx = i;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

      
            mechanic::Mechanic *m = active_mechanics[selected_idx];
            service::Service *s = m->front_queue; // servis terdepan milik montir 

            if (s != NULL)
            {
                ImVec2 pCard = ImGui::GetCursorScreenPos();
                ImDrawList *draw_list = ImGui::GetWindowDrawList();

                draw_list->AddRectFilled(pCard, ImVec2(pCard.x + 840, pCard.y + 220), IM_COL32(245, 248, 252, 255), 12.0f);
                draw_list->AddRect(pCard, ImVec2(pCard.x + 840, pCard.y + 220), COLOR_BORDER, 12.0f);
                draw_list->AddRectFilled(pCard, ImVec2(pCard.x + 8, pCard.y + 220), COLOR_BADGE_BLU, 12.0f, ImDrawFlags_RoundCornersLeft);

                ImGui::SetCursorScreenPos(ImVec2(pCard.x + 30, pCard.y + 20));
                ImGui::PushFont(fontMedium);
                ImGui::Text("Detail Servis Berjalan");
                ImGui::PopFont();

                ImGui::SetCursorScreenPos(ImVec2(pCard.x + 30, pCard.y + 55));
                ImGui::Text("Kendaraan      : %s %s", s->brand.c_str(), s->model.c_str());

                ImGui::SetCursorScreenPos(ImVec2(pCard.x + 30, pCard.y + 80));
                ImGui::Text("Keluhan        : %s", s->issue.c_str());

                ImGui::SetCursorScreenPos(ImVec2(pCard.x + 30, pCard.y + 105));
                ImGui::Text("Nama Montir    : %s", s->mech_data->name.c_str());

                ImGui::SetCursorScreenPos(ImVec2(pCard.x + 30, pCard.y + 130));
                ImGui::Text("Tgl Masuk      : %s", s->date_in.c_str());

                const char *custName = s->cust_data ? s->cust_data->name.c_str() : "Unknown";
                const char *custPhone = s->cust_data ? s->cust_data->phone.c_str() : "-";

                ImGui::SetCursorScreenPos(ImVec2(pCard.x + 30, pCard.y + 155));
                ImGui::Text("Pelanggan      : %s", custName);

                ImGui::SetCursorScreenPos(ImVec2(pCard.x + 30, pCard.y + 180));
                ImGui::Text("No Telepon     : %s", custPhone);

                
                ImGui::SetCursorScreenPos(ImVec2(pCard.x, pCard.y + 240));

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(COLOR_TEXT_GRN)));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.7f, 0.2f, 1.0f));
                if (ImGui::Button("Selesaikan Servis Ini", ImVec2(250, 45)))
                {
                    ImGui::OpenPopup("Konfirmasi Selesai");
                }
                ImGui::PopStyleColor(2);


                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

                if (ImGui::BeginPopupModal("Konfirmasi Selesai", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Apakah Anda yakin ingin menyelesaikan service ini?\nData akan ditandai selesai dan dicabut dari antrian.");
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(COLOR_PRIMARY)));
                    if (ImGui::Button("Ya, Selesaikan", ImVec2(150, 40)))
                    {
                     
                        s->status = service::DONE;
                        s->date_out = helper::getCurrentDate();

                        if (m->front_queue->next_mech_q != NULL)
                        {
                            m->front_queue = m->front_queue->next_mech_q;
                        }
                        else
                        {
                            m->front_queue = NULL;
                            m->rear_queue = NULL;
                        }
                        s->next_mech_q = NULL;

                        if (service::front_queue == s)
                        {
                            service::front_queue = s->next_q;
                            if (service::front_queue == NULL)
                            {
                                service::rear_queue = NULL;
                            }
                        }
                        else
                        {
                            service::Service *bantuGlobal = service::front_queue;
                            service::Service *prevGlobal = NULL;

                            while (bantuGlobal != NULL && bantuGlobal != s)
                            {
                                prevGlobal = bantuGlobal;
                                bantuGlobal = bantuGlobal->next_q;
                            }

                            if (bantuGlobal == s)
                            {
                                prevGlobal->next_q = bantuGlobal->next_q;

                                if (bantuGlobal == service::rear_queue)
                                {
                                    service::rear_queue = prevGlobal;
                                }
                            }
                        }

                        s->next_q = NULL;
                        service::total_queue--;

                        service::update(s);


                        ShowToast("Sukses: Service berhasil diselesaikan!");
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::PopStyleColor();

                    ImGui::SameLine();

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(COLOR_BORDER)));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(COLOR_TEXT_MAIN)));
                    if (ImGui::Button("Batal (N)", ImVec2(120, 40)))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::PopStyleColor(2);

                    ImGui::EndPopup();
                }
            }
        }

        EndCard();
    }

    void formNewCustomer()
    {
        BeginCard("FormNewCustomerCard", ImVec2(900, 600), false, false);

        ImGui::PushFont(fontTitle);
        ImGui::Text("Registrasi Pelanggan Baru");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::Spacing();

        static char new_namaCustomer[128] = "";
        static char new_telpCustomer[64] = "";
        static char new_alamatCustomer[256] = "";
        static int new_umurCustomer = 20;
        static int new_genderCustomer = 0; // 0 = L, 1 = P

        ImGui::PushFont(fontMedium);
        ImGui::Text("Informasi Data Diri Pelanggan");
        ImGui::PopFont();
        ImGui::Spacing();

      
        ImGui::BeginGroup(); // Kolom Kiri
        ImGui::PushFont(fontRegular);
        ImGui::Text("Nama Lengkap");
        ImGui::SetNextItemWidth(350);
        ImGui::InputText("##namacust_new", new_namaCustomer, IM_ARRAYSIZE(new_namaCustomer));

        ImGui::Spacing();
        ImGui::Text("Nomor Telepon");
        ImGui::SetNextItemWidth(250);
        ImGui::InputText("##telp_new", new_telpCustomer, IM_ARRAYSIZE(new_telpCustomer));

        ImGui::Spacing();
        ImGui::Text("Umur");
        ImGui::SetNextItemWidth(150);
        ImGui::InputInt("##umur_new", &new_umurCustomer);
        ImGui::EndGroup();

        ImGui::SameLine(450); // Jarak antar kolom

        ImGui::BeginGroup(); 
        ImGui::Text("Jenis Kelamin");
        ImGui::RadioButton("Laki-laki (L)", &new_genderCustomer, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Perempuan (P)", &new_genderCustomer, 1);

        ImGui::Spacing();
        ImGui::Text("Alamat Lengkap");
        ImGui::SetNextItemWidth(350);
        ImGui::InputTextMultiline("##alamat_new", new_alamatCustomer, IM_ARRAYSIZE(new_alamatCustomer), ImVec2(0, 80));
        ImGui::PopFont();
        ImGui::EndGroup();

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // TOMBOL SUBMIT & VALIDASI
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(COLOR_TEXT_GRN)));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.6f, 0.2f, 1.0f));

        if (ImGui::Button("Simpan Data Pelanggan", ImVec2(250, 45)))
        {
            std::string finalCustName = new_namaCustomer;

            if (finalCustName.empty())
            {
                ShowToast("Error: Nama pelanggan tidak boleh kosong!");
            }
           

            else if (customer::findByName(finalCustName) != NULL)
            {
                ShowToast("Error: Pelanggan dengan nama tersebut sudah terdaftar!");
            }
            else
            {
                customer::Customer newCust;
                newCust.name = finalCustName;
                newCust.phone = new_telpCustomer;
                newCust.address = new_alamatCustomer;
                newCust.age = new_umurCustomer;
                newCust.gender = (new_genderCustomer == 0) ? 'L' : 'P';

                newCust.prev = NULL;
                newCust.next = NULL;
                newCust.head_service = NULL;

                int totalCust = customer::count();
                helper::generateID('C', totalCust, &newCust.id);

                
                customer::insert(newCust);
                customer::save(newCust);

                
                new_namaCustomer[0] = '\0';
                new_telpCustomer[0] = '\0';
                new_alamatCustomer[0] = '\0';
                new_umurCustomer = 20;
                new_genderCustomer = 0;

                ShowToast("Sukses: Data Pelanggan Berhasil Didaftarkan!");
            }
        }
        ImGui::PopStyleColor(2);

        EndCard();
    }

    void formNewMechanic()
    {
        BeginCard("FormNewMechanicCard", ImVec2(900, 600), false, false);

        ImGui::PushFont(fontTitle);
        ImGui::Text("Registrasi Montir Baru");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::Spacing();

        static char new_namaMontir[128] = "";

        ImGui::PushFont(fontMedium);
        ImGui::Text("Informasi Data Montir");
        ImGui::PopFont();
        ImGui::Spacing();

        ImGui::PushFont(fontRegular);
        ImGui::Text("Nama Lengkap Montir");
        ImGui::SetNextItemWidth(400);
        ImGui::InputText("##namamontir_new", new_namaMontir, IM_ARRAYSIZE(new_namaMontir));
        ImGui::PopFont();

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(COLOR_TEXT_GRN)));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.6f, 0.2f, 1.0f));

        if (ImGui::Button("Simpan Data Montir", ImVec2(250, 45)))
        {
            std::string finalMechName = new_namaMontir;

            if (finalMechName.empty())
            {
                ShowToast("Error: Nama montir tidak boleh kosong!");
            }

            else if (mechanic::findByName(finalMechName) != NULL)
            {
                ShowToast("Error: Montir dengan nama tersebut sudah terdaftar!");
            }
            else
            {
                mechanic::Mechanic *newMech = new mechanic::Mechanic;
                newMech->name = finalMechName;

               
                newMech->front_queue = NULL;
                newMech->rear_queue = NULL;
                newMech->head_history = NULL;
                newMech->prev = NULL;
                newMech->next = NULL;

                
                int totalMech = mechanic::count();
                helper::generateID('M', totalMech, &newMech->id);

                mechanic::insert(newMech);

                for (int i = 0; i < 100; i++)
                {
                    if (mechanic::list_mechanics[i] == nullptr)
                    {
                        mechanic::list_mechanics[i] = newMech;
                        break;
                    }
                }

                mechanic::save(newMech);

                // Reset Form
                new_namaMontir[0] = '\0';
                ShowToast("Sukses: Data Montir Berhasil Ditambahkan!");
            }
        }
        ImGui::PopStyleColor(2);

        EndCard();
    }
}