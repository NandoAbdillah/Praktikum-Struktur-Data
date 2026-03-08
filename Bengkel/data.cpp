#include "data.h"
#include "utils/helper.h"
#include <iostream>
#include <fstream>
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
                        s->next_in_queue = NULL;
                    }
                    else
                    {
                        baru->rear_service_queue->next_in_queue = s;
                        baru->rear_service_queue = s;
                        s->next_in_queue = NULL;
                    }
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

    void insertServiceBelakangToCustomer(Customer *customer, Service *newService)
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

        // PENTING: Kosongkan pointernya agar bersih!
        c->head_service = NULL;
        c->next = NULL;
        c->prev = NULL;

        data::insertCustomerBelakang(*c);
        data::saveCustomer(*c);

        string idBaru = c->id_customer;

        delete c;

        return data::findCustomerById(idBaru);
    }
    void tambahServiceBaru()
    {
        data::Service *s = new data::Service;
        data::Customer *owner = NULL;

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

        s->data_customer = owner;
        s->next = NULL;

        data::insertServiceBelakangToCustomer(owner, s);
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
        cout << "Tanggal Masuk       : " << s->tanggal_masuk_bengkel << endl;
        cout << "Nama Pelanggan     : " << (s->data_customer ? s->data_customer->nama : "Unknown") << endl;
        cout << "No Telepon         : " << (s->data_customer ? s->data_customer->nomor_telepon : "-") << endl;

        cout << "Apakah Anda yakin ingin menyelesaikan service ini? (Y/N): ";
        char confirm;
        cin >> confirm;
        if (toupper(confirm) == 'Y')
        {
            s->status = Service::SELESAI;
            s->tanggal_selesai = "2024-06-30"; 
            data::dequeueService();
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

        // 1. Tampilkan HANYA yang punya riwayat
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

        // Cegah program lanjut jika memang tidak ada data sama sekali
        if (!adaYangPunyaRiwayat)
        {
            cout << "Belum ada montir yang memiliki riwayat pekerjaan.\n";
            return;
        }

        int pilMontir;
        cout << "\nMasukkan Pilihan: ";
        cin >> pilMontir;

        // 2. Logika Pencarian yang BENAR
        m = headMechanic;
        int currentIndex = 1;

        while (m != NULL)
        {
            if (m->head_service_history != NULL)
            {
                if (currentIndex == pilMontir)
                {
                    break; // KUNCI UTAMA: Hentikan pencarian jika montir yang tepat sudah ketemu!
                }
                currentIndex++;
            }
            m = m->next;
        }

        // Jika user memasukkan angka di luar pilihan (misal: 99)
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
            cout << "Model Mobil: " << s->model_mobil << endl;
            cout << "Merek Mobil: " << s->merek_mobil << endl;
            cout << "Deskripsi Kendala: " << s->deskripsi_kendala << endl;
            cout << "Nama Montir: " << s->data_montir->nama << endl;
            cout << "Nama Pelanggan: " << (s->data_customer ? s->data_customer->nama : "Unknown") << endl;
            cout << "Nomor Telepon: " << (s->data_customer ? s->data_customer->nomor_telepon : "-") << endl;
            cout << "Status: " << (s->status == Service::DIPROSES ? "DIPROSES" : "SELESAI") << endl;
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
            if (owner != NULL)
            {
                insertServiceBelakangToCustomer(owner, s);
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
             << newService->tanggal_selesai << ","
             << (newService->status == Service::DIPROSES ? "0" : "1");

        file.close();
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

                cout << "Mobil            : " << last->model_mobil << endl;
                cout << "Deskripsi Kendala : " << last->deskripsi_kendala << endl;
                cout << "Nama Montir      : " << last->data_montir->nama << endl;
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

        int serviceCount = 0;
        while (s != NULL && serviceCount < 3)
        {
            cout << "-----------------------------" << endl;
            cout << "Model Mobil        : " << s->model_mobil << endl;
            cout << "Merek Mobil        : " << s->merek_mobil << endl;
            cout << "Deskripsi Kendala  : " << s->deskripsi_kendala << endl;
            cout << "Nama Montir        : " << s->data_montir->nama << endl;
            cout << "Nama Pelanggan     : " << (s->data_customer ? s->data_customer->nama : "Unknown") << endl;
            cout << "No Telepon         : " << (s->data_customer ? s->data_customer->nomor_telepon : "-") << endl;
            cout << "-----------------------------" << endl;

            serviceCount++;
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
            cout << "No Telepon         : " << (s->data_customer ? s->data_customer->nomor_telepon : "-") << endl;
            cout << "Status             : " << s->DIPROSES << endl;
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
                Service *s = c->head_service;
                while (s != NULL)
                {
                    cout << "--------------------" << endl;
                    cout << "Model Mobil: " << s->model_mobil << endl;
                    cout << "Merek Mobil: " << s->merek_mobil << endl;
                    cout << "Deskripsi Kendala: " << s->deskripsi_kendala << endl;
                    cout << "Nama Montir: " << s->data_montir->nama << endl;
                    cout << "Tanggal Masuk: " << s->tanggal_masuk_bengkel << endl;
                    cout << "Nama Pelanggan: " << (c ? c->nama : "Unknown") << endl;
                    cout << "Nomor Telepon Pelanggan: " << (c ? c->nomor_telepon : "-") << endl;
                    cout << "--------------------" << endl;

                    s = s->next_in_customer;
                }
                break;
            }
            c = c->next;
        }

        if (!found)
        {
            cout << "Pelanggan tidak ditemukan.\n";
        }
    }
}