#include "data.h"
#include "utils/helper.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <filesystem>
#define MAX_SERVICES_QUEUE 100

using namespace std;

namespace data
{
    Customer *headCustomer, *tailCustomer;

    Service *headService, *tailService;

    Mechanic *headMechanic, *tailMechanic;
    
    int totalServicesInQueue = 0;
    Service *frontServiceQueue, *rearServiceQueue;

    void init()
    {
        headCustomer = NULL;
        tailCustomer = NULL;

        headService = NULL;
        tailService = NULL;

        frontServiceQueue = NULL;
        rearServiceQueue = NULL;

        headMechanic = NULL;
        tailMechanic = NULL;
    }

    int queueServiceEmpty()
    {
        return frontServiceQueue == NULL;
    }
    int queueServiceFull()
    {
        return totalServicesInQueue >= MAX_SERVICES_QUEUE;
    }

    void enqueueService(Service *service)
    {
        if (!queueServiceFull())
        {
            service->next_in_queue = NULL;

            if (queueServiceEmpty())
            {
                frontServiceQueue = rearServiceQueue = service;
            }
            else
            {
                rearServiceQueue->next_in_queue = service;
                rearServiceQueue = service;
            }
            totalServicesInQueue++;
        }
    }

    void dequeueService()
    {
        if (!queueServiceEmpty())
        {
            frontServiceQueue = frontServiceQueue->next_in_queue;
            totalServicesInQueue--;

            if (queueServiceEmpty())
            {
                rearServiceQueue = NULL;
            }
        }
    }

    bool isCustomerEmpty()
    {
        return headCustomer == NULL;
    }

    void insertCustomerBelakang(Customer newCustomer)
    {
        Customer *baru = new Customer;
        *baru = newCustomer;
        baru->head_service = NULL;
        baru->next = NULL;
        baru->prev = NULL;

        if (isCustomerEmpty())
        {
            headCustomer = tailCustomer = baru;
        }
        else
        {
            tailCustomer->next = baru;
            baru->prev = tailCustomer;
            tailCustomer = baru;
        }
    }

    void insertMontirBelakang(Mechanic newMechanic)
    {
        Mechanic *baru = new Mechanic;
        *baru = newMechanic;

        baru->front_service_queue = NULL;
        baru->rear_service_queue = NULL;
        baru->head_service_history = NULL;

        baru->next = NULL;
        baru->prev = NULL;

        if (headMechanic == NULL)
        {
            headMechanic = tailMechanic = baru;
        }
        else
        {
            tailMechanic->next = baru;
            baru->prev = tailMechanic;
            tailMechanic = baru;
        }

        Service *s = headService;

        while (s != NULL)
        {
            if (s->id_montir == baru->id_mechanic)
            {
                s->data_montir = baru;

                if (s->status == Service::DIPROSES)
                {
                    if (baru->front_service_queue == NULL)
                    {
                        baru->rear_service_queue = baru->front_service_queue = s;
                    }
                    else
                    {

                        baru->rear_service_queue->next_in_mechanic_queue = s;
                        baru->rear_service_queue = s;
                    }

                    s->next_in_mechanic_queue = NULL;
                }

                if (baru->head_service_history == NULL)
                {
                    baru->head_service_history = s;
                    s->next_in_history = NULL;
                }
                else
                {
                    Service *historyBantu = baru->head_service_history;
                    while (historyBantu->next_in_history != NULL)
                    {
                        historyBantu = historyBantu->next_in_history;
                    }
                    historyBantu->next_in_history = s;
                    s->next_in_history = NULL;
                }
            }

            s = s->next;
        }
    }

    Customer *findCustomerById(string id_customer)
    {
        Customer *bantu = headCustomer;

        while (bantu != NULL)
        {
            if (bantu->id_customer == id_customer)
            {
                return bantu;
            }
            bantu = bantu->next;
        }

        return NULL;
    }

    Mechanic *findMechanicById(string id_mechanic)
    {
        Mechanic *bantu = headMechanic;

        while (bantu != NULL)
        {
            if (bantu->id_mechanic == id_mechanic)
            {
                return bantu;
            }
            bantu = bantu->next;
        }

        return NULL;
    }

    string findCustomerIdByName(string nama)
    {
        Customer *bantu = headCustomer;

        while (bantu != NULL)
        {
            if (bantu->nama == nama)
            {
                return bantu->id_customer;
            }
            bantu = bantu->next;
        }

        return "";
    }

    void insertServiceBelakangToCustomer(Customer *customer, Service *newService, Mechanic *montir)
    {
        if (customer == NULL || newService == NULL)
            return;

        if (headService == NULL)
        {
            headService = tailService = newService;
        }
        else
        {
            tailService->next = newService;
            newService->prev = tailService;
            tailService = newService;
        }

        newService->data_customer = customer;
        newService->data_montir = montir;

        newService->next = NULL;
        newService->next_in_customer = NULL;

        if (customer->head_service == NULL)
        {
            customer->head_service = newService;
        }
        else
        {
            Service *bantu = customer->head_service;
            while (bantu->next_in_customer != NULL)
            {
                bantu = bantu->next_in_customer;
            }
            bantu->next_in_customer = newService;
        }
    }

    Customer *tambahCustomerBaru()
    {
        Customer *c = new Customer;

        cout << "Nama Pelanggan       : ";
        getline(cin, c->nama);

        Customer *existingCustomer = findCustomerById(findCustomerIdByName(c->nama));
        if (existingCustomer != NULL)
        {
            cout << "Pelanggan dengan nama ini sudah ada. Silahkan menggunakan nama lain.\n";
            delete c;
            return existingCustomer;
        }

        cout << "Nomor Telepon        : ";
        getline(cin, c->nomor_telepon);
        cout << "Alamat               : ";
        getline(cin, c->alamat);
        cout << "Umur                 : ";
        cin >> c->umur;
        cout << "Gender (L/P)         : ";
        cin >> c->gender;

        int totalCustomers = data::countCustomers();
        helper::generateID('C', totalCustomers, &c->id_customer);

        c->head_service = NULL;
        c->next = NULL;
        c->prev = NULL;

        data::insertCustomerBelakang(*c);
        data::saveCustomer(*c);

        string idBaru = c->id_customer;

        delete c;

        cout << "Pelanggan berhasil ditambahkan dengan" << endl;

        return data::findCustomerById(idBaru);
    }

    void tambahServiceBaru()
    {
        data::Service *s = new data::Service;
        data::Customer *owner = NULL;
        data::Mechanic *montir = NULL;

        cin.ignore();
        cout << "Model Mobil       : ";
        getline(cin, s->model_mobil);
        cout << "Merek Mobil       : ";
        getline(cin, s->merek_mobil);
        cout << "Deskripsi Kendala : ";
        getline(cin, s->deskripsi_kendala);
        cout << "Nama Montir       : ";

        Mechanic *m = headMechanic;
        int index = 1;
        while (m != NULL)
        {
            cout << index << ". " << m->nama << endl;
            m = m->next;
            index++;
        }
        int pilMontir;
        cout << "Masukkan Pilihan: ";
        cin >> pilMontir;
        m = headMechanic;
        int currentIndex = 1;
        while (m != NULL)
        {
            if (currentIndex == pilMontir)
            {
                s->id_montir = m->id_mechanic;
                break;
            }
            currentIndex++;
            m = m->next;
        }

        s->id_montir = m->id_mechanic;

        int totalServices = data::countServices();
        helper::generateID('S', totalServices, &s->id_service);

        cin.ignore();
        cout << "Nama Customer       : ";
        string namaCustomer;
        getline(cin, namaCustomer);

        if (data::findCustomerIdByName(namaCustomer) == "")
        {
            cout << "Pelanggan tidak ditemukan. Daftar Pelanggan Baru.\n";

            owner = tambahCustomerBaru();
            s->id_customer = owner->id_customer;
        }
        else
        {
            s->id_customer = data::findCustomerIdByName(namaCustomer);

            owner = data::findCustomerById(s->id_customer);

            if (owner == NULL)
            {
                cout << "Customer tidak ditemukan. Service gagal ditambahkan.\n";
                return;
            }
        }

        s->tanggal_masuk_bengkel = helper::getCurrentDate();
        s->status = Service::DIPROSES;

        s->data_customer = owner;
        s->next = NULL;

        data::insertServiceBelakangToCustomer(owner, s, montir);
        data::saveService(s);

        cout << "Service berhasil ditambahkan.\n";
    }

    void selesaikanService()
    {
        cout << "Pilih Montir yang akan menyelesaikan service:\n";
        Mechanic *m = headMechanic;
        int index = 1;
        while (m != NULL)
        {
            cout << index << ". " << m->nama << endl;
            m = m->next;
            index++;
        }

        cout << "Masukkan Pilihan: ";

        int pilMontir;
        cin >> pilMontir;

        m = headMechanic;
        int currentIndex = 1;
        while (m != NULL)
        {
            if (currentIndex == pilMontir)
            {
                break;
            }
            currentIndex++;
            m = m->next;
        }

        if (m == NULL)
        {
            cout << "Pilihan tidak valid!\n";
            return;
        }
        Service *s = m->front_service_queue;
        if (s == NULL)
        {
            cout << "Montir ini tidak memiliki service yang sedang diproses.\n";
            return;
        }
        cout << "Servis" << endl;
        cout << "Model Mobil        : " << s->model_mobil << endl;
        cout << "Merek Mobil        : " << s->merek_mobil << endl;
        cout << "Deskripsi Kendala  : " << s->deskripsi_kendala << endl;
        cout << "Nama Montir        : " << s->data_montir->nama << endl;
        cout << "Tanggal Masuk      : " << s->tanggal_masuk_bengkel << endl;
        cout << "Nama Pelanggan     : " << (s->data_customer ? s->data_customer->nama : "Unknown") << endl;
        cout << "No Telepon         : " << (s->data_customer ? s->data_customer->nomor_telepon : "-") << endl;

        cout << "Apakah Anda yakin ingin menyelesaikan service ini? (Y/N): ";
        char confirm;
        cin >> confirm;
        if (toupper(confirm) == 'Y')
        {
            s->status = Service::SELESAI;
            s->tanggal_selesai = helper::getCurrentDate();

            if (m->front_service_queue->next_in_mechanic_queue != NULL)
            {
                m->front_service_queue = m->front_service_queue->next_in_mechanic_queue;
            }
            else
            {
                m->front_service_queue = NULL;
                m->rear_service_queue = NULL;
            }
            s->next_in_mechanic_queue = NULL;

            if (frontServiceQueue == s)
            {

                frontServiceQueue = s->next_in_queue;
                if (frontServiceQueue == NULL)
                {
                    rearServiceQueue = NULL;
                }
            }
            else
            {

                Service *bantuGlobal = frontServiceQueue;
                Service *prevGlobal = NULL;

                while (bantuGlobal != NULL && bantuGlobal != s)
                {
                    prevGlobal = bantuGlobal;
                    bantuGlobal = bantuGlobal->next_in_queue;
                }

                if (bantuGlobal == s)
                {

                    prevGlobal->next_in_queue = bantuGlobal->next_in_queue;

                    if (bantuGlobal == rearServiceQueue)
                    {
                        rearServiceQueue = prevGlobal;
                    }
                }
            }

            s->next_in_queue = NULL;
            totalServicesInQueue--;

            updateService(s);

            cout << "Service berhasil diselesaikan.\n";
        }
        else
        {
            cout << "Operasi dibatalkan.\n";
        }
    }

    void riwayatKerjaMontir()
    {
        cout << "\nPilih Montir: \n\n";

        Mechanic *m = headMechanic;
        int index = 1;
        bool adaYangPunyaRiwayat = false;

        while (m != NULL)
        {
            if (m->head_service_history != NULL)
            {
                cout << index << ". " << m->nama << endl;
                index++;
                adaYangPunyaRiwayat = true;
            }
            m = m->next;
        }

        if (!adaYangPunyaRiwayat)
        {
            cout << "Belum ada montir yang memiliki riwayat pekerjaan.\n";
            return;
        }

        int pilMontir;
        cout << "\nMasukkan Pilihan: ";
        cin >> pilMontir;

        m = headMechanic;
        int currentIndex = 1;

        while (m != NULL)
        {
            if (m->head_service_history != NULL)
            {
                if (currentIndex == pilMontir)
                {
                    break;
                }
                currentIndex++;
            }
            m = m->next;
        }

        if (m == NULL)
        {
            cout << "Pilihan tidak valid!\n";
            return;
        }

        // 3. Cetak Detail
        cout << "\nRiwayat Kerja Montir : " << m->nama << "\n\n";
        Service *s = m->head_service_history;
        while (s != NULL)
        {
            cout << "Model Mobil        : " << s->model_mobil << endl;
            cout << "Merek Mobil        : " << s->merek_mobil << endl;
            cout << "Deskripsi Kendala  : " << s->deskripsi_kendala << endl;
            cout << "Nama Montir        : " << s->data_montir->nama << endl;
            cout << "Nama Pelanggan     : " << (s->data_customer ? s->data_customer->nama : "Unknown") << endl;
            cout << "Nomor Telepon      : " << (s->data_customer ? s->data_customer->nomor_telepon : "-") << endl;
            cout << "Tanggal Masuk      : " << s->tanggal_masuk_bengkel << endl;
            cout << "Status             : " << (s->status == Service::DIPROSES ? "DIPROSES" : "SELESAI") << endl;
            cout << "-----------------------------" << endl;

            s = s->next_in_history;
        }
    }
    
    bool loadServiceQueue()
    {
        Service *bantu = headService;

        while (bantu != NULL)
        {
            if (bantu->status == Service::DIPROSES)
            {
                enqueueService(bantu);
            }
            bantu = bantu->next;
        }

        return true;
    }

    int countCustomers()
    {
        int count = 0;
        Customer *bantu = headCustomer;
        while (bantu != NULL)
        {
            count++;
            bantu = bantu->next;
        }
        return count;
    }

    int countServices()
    {
        int count = 0;
        Service *bantu = headService;

        while (bantu != NULL)
        {
            count++;

            bantu = bantu->next;
        }

        return count;
    }

    bool loadAllCustomers()
    {
        ifstream file(DB_CUSTOMERS);

        if (!file.is_open())
            return false;

        string line;

        getline(file, line);
        while (getline(file, line))
        {
            if (line.empty())
                continue;

            Customer c;
            c.id_customer = helper::getCSVColumn(line, 0);
            c.nama = helper::getCSVColumn(line, 1);

            string umurStr = helper::getCSVColumn(line, 2);
            c.umur = umurStr.empty() ? 0 : stoi(umurStr);

            string genderStr = helper::getCSVColumn(line, 3);
            c.gender = genderStr.empty() ? '-' : genderStr[0];

            c.nomor_telepon = helper::getCSVColumn(line, 4);
            c.alamat = helper::getCSVColumn(line, 5);

            c.head_service = NULL;
            c.next = NULL;

            insertCustomerBelakang(c);
        }

        file.close();
        return true;
    }

    bool loadAllServices()
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
            s->id_service = helper::getCSVColumn(line, 0);
            s->id_montir = helper::getCSVColumn(line, 1);
            s->id_customer = helper::getCSVColumn(line, 2);
            s->model_mobil = helper::getCSVColumn(line, 3);
            s->merek_mobil = helper::getCSVColumn(line, 4);
            s->deskripsi_kendala = helper::getCSVColumn(line, 5);

            s->tanggal_masuk_bengkel = helper::getCSVColumn(line, 6);
            s->tanggal_selesai = helper::getCSVColumn(line, 7);
            string statusStr = helper::getCSVColumn(line, 8);
            s->status = statusStr == "0" ? Service::DIPROSES : Service::SELESAI;

            s->data_customer = NULL;
            s->next = NULL;

            Customer *owner = findCustomerById(s->id_customer);
            Mechanic *montir = findMechanicById(s->id_montir);

            if (owner != NULL)
            {
                insertServiceBelakangToCustomer(owner, s, montir);
            }
        }

        file.close();
        return true;
    }

    bool loadAllMechanics()
    {
        ifstream file(DB_MECHANICS);

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

            Mechanic m;
            m.id_mechanic = helper::getCSVColumn(line, 0);
            m.nama = helper::getCSVColumn(line, 1);

            insertMontirBelakang(m);
        }

        file.close();
        return true;
    }

    bool saveCustomer(Customer newCustomer)
    {
        ofstream file(DB_CUSTOMERS, ios::app);
        if (!file.is_open())
        {
            return false;
        }

        file << endl
             << newCustomer.id_customer << ","
             << newCustomer.nama << ","
             << newCustomer.umur << ","
             << newCustomer.gender << ","
             << newCustomer.nomor_telepon << ","
             << newCustomer.alamat;

        file.close();
        return true;
    }

    bool saveService(Service *newService)
    {
        ofstream file(DB_SERVICES, ios::app);
        if (!file.is_open())
        {
            return false;
        }

        file << endl
             << newService->id_service << ","
             << newService->id_montir << ","
             << newService->id_customer << ","
             << newService->model_mobil << ","
             << newService->merek_mobil << ","
             << newService->deskripsi_kendala << ","
             << newService->tanggal_masuk_bengkel << ","
             << (newService->tanggal_selesai.empty() ? "-" : newService->tanggal_selesai) << ","
             << (newService->status == Service::DIPROSES ? "0" : "1");

        file.close();
        return true;
    }

    bool updateService(Service *service)
    {
        ifstream fileIn(DB_SERVICES);
        if (!fileIn.is_open())
        {
            return false;
        }

        const char *tempFileName = "./database/temp_services.csv";
        ofstream fileOut(tempFileName);
        if (!fileOut.is_open())
        {
            fileIn.close();
            return false;
        }

        string line;
        // Baca dan tulis header
        if (getline(fileIn, line))
        {
            fileOut << line << "\n";
        }

        while (getline(fileIn, line))
        {
            if (line.empty())
                continue;

            string id_service = helper::getCSVColumn(line, 0);

            if (id_service == service->id_service)
            {
                fileOut << service->id_service << ","
                        << service->id_montir << ","
                        << service->id_customer << ","
                        << service->model_mobil << ","
                        << service->merek_mobil << ","
                        << service->deskripsi_kendala << ","
                        << service->tanggal_masuk_bengkel << ","
                        << (service->tanggal_selesai.empty() ? "-" : service->tanggal_selesai) << ","
                        << (service->status == Service::DIPROSES ? "0" : "1") << "\n";
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

    void showAllCustomers()
    {
        Customer *c = headCustomer;

        if (c == NULL)
        {
            cout << "Data customer kosong.\n";
            return;
        }

        while (c != NULL)
        {
            cout << "Nama              : " << c->nama << endl;
            cout << "Nomor Telepon     : " << c->nomor_telepon << endl;
            cout << "Alamat            : " << c->alamat << endl;

            cout << "--- Servis Terakhir ---" << endl;

            if (c->head_service != NULL)
            {
                Service *last = c->head_service;
                while (last->next_in_customer != NULL)
                {
                    last = last->next_in_customer;
                }

                cout << "Mobil              : " << last->model_mobil << endl;
                cout << "Deskripsi Kendala  : " << last->deskripsi_kendala << endl;
                cout << "Nama Montir        : " << last->data_montir->nama << endl;
            }
            else
            {
                cout << "Servis Terakhir: Belum ada" << endl;
            }

            cout << "-----------------------------" << endl;
            c = c->next;
        }
    }

    void showAllServices()
    {
        Service *s = headService;
        if (s == NULL)
        {
            cout << "Data service kosong.\n";
            return;
        }

        while (s != NULL)
        {
            cout << "-----------------------------" << endl;
            cout << "Model Mobil        : " << s->model_mobil << endl;
            cout << "Merek Mobil        : " << s->merek_mobil << endl;
            cout << "Deskripsi Kendala  : " << s->deskripsi_kendala << endl;
            cout << "Nama Montir        : " << s->data_montir->nama << endl;
            cout << "Nama Pelanggan     : " << (s->data_customer ? s->data_customer->nama : "Unknown") << endl;
            cout << "No Telepon         : " << (s->data_customer ? s->data_customer->nomor_telepon : "-") << endl;
            cout << "Status             : " << (s->status == Service::DIPROSES ? "DIPROSES" : "SELESAI") << endl;
            cout << "-----------------------------" << endl;

            s = s->next;
        }
    }

    void showServiceQueue()
    {
        Service *s = frontServiceQueue;

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
            cout << "Model Mobil        : " << s->model_mobil << endl;
            cout << "Merek Mobil        : " << s->merek_mobil << endl;
            cout << "Deskripsi Kendala  : " << s->deskripsi_kendala << endl;
            cout << "Nama Montir        : " << s->data_montir->nama << endl;
            cout << "Nama Pelanggan     : " << (s->data_customer ? s->data_customer->nama : "Unknown") << endl;
            cout << "Tanggal Masuk      : " << s->tanggal_masuk_bengkel << endl;
            cout << "No Telepon         : " << (s->data_customer ? s->data_customer->nomor_telepon : "-") << endl;
            cout << "Status             : " << "DIPROSES" << endl;
            cout << "-----------------------------" << endl;

            s = s->next_in_queue;
        }
    }

    void showCustomerServiceHistory(string namaPelanggan)
    {
        Customer *c = headCustomer;
        bool found = false;

        while (c != NULL)
        {
            if (c->nama == namaPelanggan)
            {
                found = true;
                break;
            }
            c = c->next;
        }

        Service *s = c->head_service;
        if (s == NULL)
        {
            cout << "Pelanggan belum pernah melakukan servis.\n";
        }

        cout << "\n Servis yang sedang diproses:\n\n";
        Service *queueBantu = frontServiceQueue;

        if (queueBantu == NULL)
        {
            cout << "Tidak ada servis yang sedang diproses.\n";
        }

        while (queueBantu != NULL)
        {
            if (queueBantu->data_customer != NULL && queueBantu->data_customer->nama == namaPelanggan)
            {
                cout << "-----------------------------" << endl;
                cout << "Model Mobil        : " << queueBantu->model_mobil << endl;
                cout << "Merek Mobil        : " << queueBantu->merek_mobil << endl;
                cout << "Deskripsi Kendala  : " << queueBantu->deskripsi_kendala << endl;
                cout << "Nama Montir        : " << queueBantu->data_montir->nama << endl;
                cout << "Tanggal Masuk      : " << queueBantu->tanggal_masuk_bengkel << endl;
                cout << "No Telepon         : " << (queueBantu->data_customer ? queueBantu->data_customer->nomor_telepon : "-") << endl;
                cout << "Status             : " << "DIPROSES" << endl;
                cout << "-----------------------------" << endl;
            }
            queueBantu = queueBantu->next_in_queue;
        }

        cout << "\n Servis yang sudah selesai:\n\n";
        while (s != NULL)
        {
            if (s->status == Service::SELESAI)
            {
                cout << "-----------------------------" << endl;
                cout << "Model Mobil        : " << s->model_mobil << endl;
                cout << "Merek Mobil        : " << s->merek_mobil << endl;
                cout << "Deskripsi Kendala  : " << s->deskripsi_kendala << endl;
                cout << "Nama Montir        : " << s->data_montir->nama << endl;
                cout << "Tanggal Masuk      : " << s->tanggal_masuk_bengkel << endl;
                cout << "Nomor Telepon      : " << (s->data_customer ? s->data_customer->nomor_telepon : "-") << endl;
                cout << "Status             : " << (s->status == Service::DIPROSES ? "DIPROSES" : "SELESAI") << endl;
                cout << "-----------------------------" << endl;
            }

            s = s->next_in_customer;
        }

        if (!found)
        {
            cout << "Pelanggan tidak ditemukan.\n";
        }
    }
}