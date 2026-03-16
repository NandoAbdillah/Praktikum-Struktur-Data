#include <iostream>
#include <fstream>
#include "customer.h"
#include "controller/service.h"
#include "controller/mechanic.h"
#include "utils/helper.h"
#define MAX_SERVICES_QUEUE 100

using namespace std;

namespace service
{
    Service *top_cancel_stack = NULL;
    int total_cancel_stack = 0;

    int total_queue = 0;
    Service *head = NULL, *tail = NULL, *front_queue = NULL, *rear_queue = NULL;

    void init()
    {
        head = NULL;
        tail = NULL;

        front_queue = NULL;
        rear_queue = NULL;
    }

    Service *find(std::string id)
    {
        Service *temp = head;
        while (temp != NULL)
        {
            if (temp->id == id)
            {
                return temp;
            }
            temp = temp->next;
        }
        return NULL;
    }

    // Fungsi Queue

    int queueServiceEmpty()
    {
        return front_queue == NULL;
    }
    int queueServiceFull()
    {
        return total_queue >= MAX_SERVICES_QUEUE;
    }

   
    void enqueueService(Service *newService)
    {
        if (newService == NULL)
            return;

        if (front_queue == NULL)
        {
            front_queue = rear_queue = newService;
        }
        else
        {
            rear_queue->next_q = newService;
            rear_queue = newService;
        }
        total_queue++; // Penghitung antrean jalan di sini!

        // 2. Masukkan ke Antrean Montir
        if (newService->mech_data != NULL)
        {
            mechanic::Mechanic *m = newService->mech_data;
            if (m->front_queue == NULL)
            {
                m->front_queue = m->rear_queue = newService;
            }
            else
            {
                m->rear_queue->next_mech_q = newService;
                m->rear_queue = newService;
            }
        }
    }

    void dequeueService()
    {
        if (!queueServiceEmpty())
        {
            front_queue = front_queue->next_q;
            total_queue--;

            if (queueServiceEmpty())
            {
                rear_queue = NULL;
            }
        }
    }

    // stack
    void pushCancel(Service *s)
    {
        if (s == NULL)
            return;

        s->status = CANCELED;

        s->next_stack = top_cancel_stack;
        top_cancel_stack = s;

        total_cancel_stack++;
    }

    // pop 
    Service *popCancel()
    {
        if (top_cancel_stack == NULL)
            return NULL; 

        Service *s = top_cancel_stack;                   
        top_cancel_stack = top_cancel_stack->next_stack; 

        s->next_stack = NULL; 
        total_cancel_stack--;

        return s;
    }

    bool cancelService(std::string id_servis, std::string id_customer)
    {
        Service *s = find(id_servis);
        if (s == NULL || s->id_cust != id_customer)
            return false; 
        if (s->status != PROCESS)
            return false; 

 
        // CABUT DARI QUEUE GLOBAL
        if (front_queue == s)
        {
            front_queue = s->next_q;
            if (front_queue == NULL)
                rear_queue = NULL;
        }
        else
        {
            Service *bantu = front_queue;
            Service *prev = NULL;
            while (bantu != NULL && bantu != s)
            {
                prev = bantu;
                bantu = bantu->next_q;
            }
            if (bantu == s)
            {
                prev->next_q = s->next_q;
                if (s == rear_queue)
                    rear_queue = prev;
            }
        }
        s->next_q = NULL; 
        total_queue--;

        
        // CABUT DARI QUEUE MONTIR
        if (s->mech_data != NULL)
        {
            mechanic::Mechanic *m = s->mech_data;
            if (m->front_queue == s)
            {
                m->front_queue = s->next_mech_q;
                if (m->front_queue == NULL)
                    m->rear_queue = NULL;
            }
            else
            {
                Service *bantu_m = m->front_queue;
                Service *prev_m = NULL;
                while (bantu_m != NULL && bantu_m != s)
                {
                    prev_m = bantu_m;
                    bantu_m = bantu_m->next_mech_q;
                }
                if (bantu_m == s)
                {
                    prev_m->next_mech_q = s->next_mech_q;
                    if (s == m->rear_queue)
                        m->rear_queue = prev_m;
                }
            }
            s->next_mech_q = NULL; 
        }

       
        pushCancel(s);

        // Fungis belum diterapkan off
        // service::update(s);

        return true;
    }

    bool undoCancelService(std::string id_servis, std::string id_customer, std::string new_date)
    {
        Service *tempStack = NULL; 
        Service *target = NULL;    

        while (top_cancel_stack != NULL)
        {
            Service *current = popCancel(); 
            
            if (current->id == id_servis && current->id_cust == id_customer)
            {
                target = current; 
                break;
            }
            else
            {
                current->next_stack = tempStack;
                tempStack = current;
            }
        }

        while (tempStack != NULL)
        {
            Service *current = tempStack;
            tempStack = tempStack->next_stack;
            
            pushCancel(current); 
        }

        // Kalau ketemu 
        if (target != NULL)
        {
            target->status = PROCESS; 
            
            if (new_date != "-" && new_date != "") 
            {
                target->date_in = new_date;
            }


            target->next_stack = NULL; 

            enqueueService(target);
            
            save(target); 
            
            return true;
        }

        return false; 
    }

    //  Tambah Service
    void add()
    {
        Service *s = new Service;
        customer::Customer *owner = NULL;
        mechanic::Mechanic *montir = NULL;

        cin.ignore();
        cout << "Model Mobil       : ";
        getline(cin, s->model);
        cout << "Merek Mobil       : ";
        getline(cin, s->brand);
        cout << "Deskripsi Kendala : ";
        getline(cin, s->issue);
        cout << "Nama Montir       : ";

        mechanic::Mechanic *m = mechanic::head;
        int index = 1;
        while (m != NULL)
        {
            cout << index << ". " << m->name << endl;
            m = m->next;
            index++;
        }

        int pilMontir;
        cout << "Masukkan Pilihan: ";
        cin >> pilMontir;

        m = mechanic::head;
        int currentIndex = 1;
        while (m != NULL)
        {
            if (currentIndex == pilMontir)
            {
                s->id_mech = m->id;
                montir = m; // Update pointer montir
                break;
            }
            currentIndex++;
            m = m->next;
        }

        s->id_mech = m->id;

        int totalServices = count();
        helper::generateID('S', totalServices, &s->id);

        cin.ignore();
        cout << "Nama Customer       : ";
        string namaCustomer;
        getline(cin, namaCustomer);

        if (customer::findByName(namaCustomer)->name == "")
        {
            cout << "Pelanggan tidak ditemukan. Daftar Pelanggan Baru.\n";
            owner = customer::add();
            s->id_cust = owner->id;
        }
        else
        {
            s->id_cust = customer::findByName(namaCustomer)->id;
            owner = customer::find(s->id_cust);

            if (owner == NULL)
            {
                cout << "Customer tidak ditemukan. Service gagal ditambahkan.\n";
                return;
            }
        }

        s->date_in = helper::getCurrentDate();
        s->status = PROCESS;

        s->cust_data = owner;
        s->next = NULL;

        assign(owner, s, montir);
        save(s);

        cout << "Service berhasil ditambahkan.\n";
    }

    //  Selesaikan Service
    bool finish(std::string id)
    {
        Service *s = find(id);
        if (s == NULL)
        {
            cout << "Service dengan ID " << id << " tidak ditemukan.\n";
            return false;
        }

        if (s->status == DONE)
        {
            cout << "Service ini sudah diselesaikan sebelumnya.\n";
            return false;
        }

        cout << "Servis" << endl;
        cout << "Model Mobil        : " << s->model << endl;
        cout << "Merek Mobil        : " << s->brand << endl;
        cout << "Deskripsi Kendala  : " << s->issue << endl;
        cout << "Nama Montir        : " << (s->mech_data ? s->mech_data->name : s->id_mech) << endl;
        cout << "Tanggal Masuk      : " << s->date_in << endl;
        cout << "Nama Pelanggan     : " << (s->cust_data ? s->cust_data->name : "Unknown") << endl;
        cout << "No Telepon         : " << (s->cust_data ? s->cust_data->phone : "-") << endl;

        cout << "Apakah Anda yakin ingin menyelesaikan service ini? (Y/N): ";
        char confirm;
        cin >> confirm;

        if (toupper(confirm) == 'Y')
        {
            s->status = DONE;
            s->date_out = helper::getCurrentDate();

            mechanic::Mechanic *m = s->mech_data;
            if (m != NULL && m->front_queue != NULL)
            {
                if (m->front_queue == s)
                {
                    m->front_queue = s->next_mech_q;
                    if (m->front_queue == NULL)
                    {
                        m->rear_queue = NULL;
                    }
                }
                else
                {
                    Service *bantuM = m->front_queue;
                    while (bantuM->next_mech_q != NULL && bantuM->next_mech_q != s)
                    {
                        bantuM = bantuM->next_mech_q;
                    }
                    if (bantuM->next_mech_q == s)
                    {
                        bantuM->next_mech_q = s->next_mech_q;
                        if (m->rear_queue == s)
                            m->rear_queue = bantuM;
                    }
                }
            }
            s->next_mech_q = NULL;

            if (front_queue == s)
            {
                front_queue = s->next_q;
                if (front_queue == NULL)
                {
                    rear_queue = NULL;
                }
            }
            else
            {
                Service *bantuGlobal = front_queue;
                Service *prevGlobal = NULL;

                while (bantuGlobal != NULL && bantuGlobal != s)
                {
                    prevGlobal = bantuGlobal;
                    bantuGlobal = bantuGlobal->next_q;
                }

                if (bantuGlobal == s)
                {
                    prevGlobal->next_q = bantuGlobal->next_q;
                    if (bantuGlobal == rear_queue)
                    {
                        rear_queue = prevGlobal;
                    }
                }
            }

            s->next_q = NULL;
            total_queue--;

            update(s);

            cout << "Service berhasil diselesaikan.\n";
            return true;
        }
        else
        {
            cout << "Operasi dibatalkan.\n";
            return false;
        }
    }

    void assign(customer::Customer *customer, Service *newService, mechanic::Mechanic *montir)
    {
        if (customer == NULL || newService == NULL)
            return;

        if (head == NULL)
        {
            head = tail = newService;
        }
        else
        {
            tail->next = newService;
            newService->prev = tail;
            tail = newService;
        }

        newService->cust_data = customer;
        newService->mech_data = montir;

        newService->next = NULL;
        newService->next_cust = NULL;
        newService->next_q = NULL;
        newService->next_mech_q = NULL;

        if (customer->head_service == NULL)
        {
            customer->head_service = newService;
        }
        else
        {
            Service *bantu = customer->head_service;
            while (bantu->next_cust != NULL)
            {
                bantu = bantu->next_cust;
            }
            bantu->next_cust = newService;
        }
    }

    //  Menghitung jumlah
    int count()
    {
        int total = 0;
        Service *bantu = head;

        while (bantu != NULL)
        {
            total++;
            bantu = bantu->next;
        }
        return total;
    }

    // 7. Load dari file CSV
    bool load()
    {
        ifstream file(DB_SERVICES);
        if (!file.is_open())
        {
            return false;
        }

        string line;
        getline(file, line);
        while (getline(file, line))
        {
            if (line.empty())
                continue;

            Service *s = new Service;
            s->id = helper::getCSVColumn(line, 0);
            s->id_mech = helper::getCSVColumn(line, 1);
            s->id_cust = helper::getCSVColumn(line, 2);
            s->model = helper::getCSVColumn(line, 3);
            s->brand = helper::getCSVColumn(line, 4);
            s->issue = helper::getCSVColumn(line, 5);

            s->date_in = helper::getCSVColumn(line, 6);
            s->date_out = helper::getCSVColumn(line, 7);
            string statusStr = helper::getCSVColumn(line, 8);
            s->status = statusStr == "0" ? PROCESS : DONE;

            s->cust_data = NULL;
            s->next = NULL;

            customer::Customer *owner = customer::find(s->id_cust);
            mechanic::Mechanic *montir = mechanic::find(s->id_mech);

            if (owner != NULL)
            {
                assign(owner, s, montir);
            }
        }

        file.close();
        return true;
    }

   

    bool loadQueue()
    {
        Service *bantu = head; 
        while (bantu != NULL)
        {
           
            if (bantu->status == PROCESS)
            {
                enqueueService(bantu);
            }
            bantu = bantu->next;
        }
        return true;
    }

    // 9. Update ke file CSV
    bool update(Service *service)
    {

        ifstream fileIn(DB_SERVICES);
        if (!fileIn.is_open())
        {
            return false;
        }

        const char *tempFileName = DB_SERVICES_TEMP;
        ofstream fileOut(tempFileName);
        if (!fileOut.is_open())
        {
            fileIn.close();
            return false;
        }

        string line;
        if (getline(fileIn, line))
        {
            fileOut << line << "\n";
        }

        while (getline(fileIn, line))
        {
            if (line.empty())
                continue;

            string id_service = helper::getCSVColumn(line, 0);

            if (id_service == service->id)
            {
                fileOut << service->id << ","
                        << service->id_mech << ","
                        << service->id_cust << ","
                        << service->model << ","
                        << service->brand << ","
                        << service->issue << ","
                        << service->date_in << ","
                        << (service->date_out.empty() ? "-" : service->date_out) << ","
                        << (service->status == PROCESS ? "0" : "1") << "\n";
            }
            else
            {
                fileOut << line << endl;
            }
        }

        fileIn.close();
        fileOut.close();

        remove(DB_SERVICES);
        rename(tempFileName, DB_SERVICES);

        return true;
    }

    //  Tampilkan semua data
    void all()
    {
        Service *s = head;
        if (s == NULL)
        {
            cout << "Data service kosong.\n";
            return;
        }

        while (s != NULL)
        {
            cout << "-----------------------------" << endl;
            cout << "Model Mobil        : " << s->model << endl;
            cout << "Merek Mobil        : " << s->brand << endl;
            cout << "Deskripsi Kendala  : " << s->issue << endl;
            cout << "Nama Montir        : " << (s->mech_data ? s->mech_data->name : s->id_mech) << endl;
            cout << "Nama Pelanggan     : " << (s->cust_data ? s->cust_data->name : "Unknown") << endl;
            cout << "No Telepon         : " << (s->cust_data ? s->cust_data->phone : "-") << endl;
            cout << "Status             : " << (s->status == PROCESS ? "DIPROSES" : "SELESAI") << endl;
            cout << "-----------------------------" << endl;

            s = s->next;
        }
    }

    // Tampilkan queue yang berjalan
    void queue()
    {
        Service *s = front_queue;

        if (s == NULL)
        {
            cout << "Antrian servis kosong.\n";
            return;
        }

        int count = 0;
        while (s != NULL)
        {
            cout << "-----------------------------" << endl;
            cout << "Nomor Antrian      : " << ++count << endl;
            cout << "Model Mobil        : " << s->model << endl;
            cout << "Merek Mobil        : " << s->brand << endl;
            cout << "Deskripsi Kendala  : " << s->issue << endl;
            cout << "Nama Montir        : " << (s->mech_data ? s->mech_data->name : s->id_mech) << endl;
            cout << "Nama Pelanggan     : " << (s->cust_data ? s->cust_data->name : "Unknown") << endl;
            cout << "Tanggal Masuk      : " << s->date_in << endl;
            cout << "No Telepon         : " << (s->cust_data ? s->cust_data->phone : "-") << endl;
            cout << "Status             : " << "DIPROSES" << endl;
            cout << "-----------------------------" << endl;

            s = s->next_q;
        }
    }

    // Tampilkan history berdasar customer
    void history(string custName)
    {
        customer::Customer *c = customer::head;
        bool found = false;

        while (c != NULL)
        {
            if (c->name == custName)
            {
                found = true;
                break;
            }
            c = c->next;
        }

        if (!found)
        {
            cout << "Pelanggan tidak ditemukan.\n";
            return;
        }

        Service *s = c->head_service;
        if (s == NULL)
        {
            cout << "Pelanggan belum pernah melakukan servis.\n";
        }

        cout << "\n Servis yang sedang diproses:\n\n";
        Service *queueBantu = front_queue;

        if (queueBantu == NULL)
        {
            cout << "Tidak ada servis yang sedang diproses.\n";
        }

        while (queueBantu != NULL)
        {
            if (queueBantu->cust_data != NULL && queueBantu->cust_data->name == custName)
            {
                cout << "-----------------------------" << endl;
                cout << "Model Mobil        : " << queueBantu->model << endl;
                cout << "Merek Mobil        : " << queueBantu->brand << endl;
                cout << "Deskripsi Kendala  : " << queueBantu->issue << endl;
                cout << "Nama Montir        : " << (queueBantu->mech_data ? queueBantu->mech_data->name : queueBantu->id_mech) << endl;
                cout << "Tanggal Masuk      : " << queueBantu->date_in << endl;
                cout << "No Telepon         : " << (queueBantu->cust_data ? queueBantu->cust_data->phone : "-") << endl;
                cout << "Status             : " << "DIPROSES" << endl;
                cout << "-----------------------------" << endl;
            }
            queueBantu = queueBantu->next_q;
        }

        cout << "\n Servis yang sudah selesai:\n\n";
        while (s != NULL)
        {
            if (s->status == DONE)
            {
                cout << "-----------------------------" << endl;
                cout << "Model Mobil        : " << s->model << endl;
                cout << "Merek Mobil        : " << s->brand << endl;
                cout << "Deskripsi Kendala  : " << s->issue << endl;
                cout << "Nama Montir        : " << (s->mech_data ? s->mech_data->name : s->id_mech) << endl;
                cout << "Tanggal Masuk      : " << s->date_in << endl;
                cout << "No Telepon         : " << (s->cust_data ? s->cust_data->phone : "-") << endl;
                cout << "Status             : " << "SELESAI" << endl;
                cout << "-----------------------------" << endl;
            }

            s = s->next_cust;
        }
    }

    bool save(Service *newService)
    {
        ofstream file(DB_SERVICES, ios::app);
        if (!file.is_open())
        {
            return false;
        }

        file << endl
             << newService->id << ","
             << newService->id_mech << ","
             << newService->id_cust << ","
             << newService->model << ","
             << newService->brand << ","
             << newService->issue << ","
             << newService->date_in << ","
             << (newService->date_out.empty() ? "-" : newService->date_out) << ","
             << (newService->status == PROCESS ? "0" : "1");

        file.close();
        return true;
    }
}