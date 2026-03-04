#include <iostream>
#include "data.h"
#include "utils/helper.h"

using namespace std;

int choice = 0;

void main_menu();
void menu_services();
void all_services();
void new_service();
void mechanic_job_history();
void all_customers();
void customer_data();

void main_menu()
{
    cout << endl
         << "Welcome To Lognuts" << endl
         << "1. Servis" << endl
         << "2. Semua Data Pelanggan" << endl
         << "3. Data Pelanggan" << endl
         << "4. Keluar" << endl
         << "Masukkan Pilihan : ";
    cin >> choice;

    switch (choice)
    {
    case 1:
        menu_services();
        break;
    case 2:
        all_customers();
        break;
    case 3:
        customer_data();
        break;
    case 4:
        exit(0);
    default:
        cout << "Pilihan tidak valid!\n";
        break;
    }
}

void menu_services()
{
    cout << endl
         << "Services" << endl
         << "1. Semua Servis Singkat" << endl
         << "2. Servis Baru" << endl
         << "3. Riwayat Kerja Montir" << endl
         << "Masukkan Pilihan : ";
    cin >> choice;

    switch (choice)
    {
    case 1:
        all_services();
        break;
    case 2:
        new_service();
        break;
    case 3:
        mechanic_job_history();
        break;
    default:
        cout << "Pilihan tidak valid!" << endl;
        break;
    }
}

void all_services()
{
    cout << "\nAll Services\n\n";
    data::showAllServices();
}

void all_customers()
{
    cout << "\nAll Customers\n\n";
    data::showAllCustomers();
}

void customer_data()
{
    data::Customer *c = data::headCustomer;

    if (c == NULL)
    {
        cout << "Customer tidak ditemukan.\n";
        return;
    }

    while (c != NULL)
    {
        cout << "\n\n\n ===== Data Pelanggan ======\n\n";
        cout << "Nama       : " << c->nama << endl
             << "Nomor Telepon : " << c->nomor_telepon << endl
             << "Umur       : " << c->umur << endl
             << "Gender    : " << c->gender << endl
             << "Alamat    : " << c->alamat << endl;

        cout << "\n --- 3 Servis Terakhir --- \n";
        cout << "-----------------------------\n";

        data::Service *s = c->head_service;

        int serviceCount = 0;
        while (s != NULL && serviceCount < 3)
        {

            cout << "Mobil              : " << s->model_mobil << endl;
            cout << "Deskripsi Kendala  : " << s->deskripsi_kendala << endl;
            cout << "Nama Montir        : " << s->nama_montir << endl;
            cout << "-----------------------------\n";

            s = s->next_in_customer;
            serviceCount++;
        }

        char navOption;
        cout << "[N]ext , [P]revious , [E]xit : ";
        cin >> navOption;
        if (toupper(navOption) == 'N')
        {
            if (c->next != NULL)
            {
                c = c->next;
            }
            else
            {
                cout << "Tidak ada data pelanggan berikutnya.\n";
            }
        }
        else if (toupper(navOption) == 'P')
        {
            if (c->prev != NULL)
            {
                c = c->prev;
            }
            else
            {
                cout << "Tidak ada data pelanggan sebelumnya.\n";
            }
        }
        else if (toupper(navOption) == 'E')
        {
            break;
        }

        else
        {
            cout << "Pilihan tidak valid!\n";
        }
    }
}

void new_service()
{
    data::Service s;
    data::Customer *owner = NULL;

    cout << "\nInput Service Baru\n";

    cin.ignore();
    cout << "Model Mobil       : ";
    getline(cin, s.model_mobil);
    cout << "Merek Mobil       : ";
    getline(cin, s.merek_mobil);
    cout << "Deskripsi Kendala : ";
    getline(cin, s.deskripsi_kendala);
    cout << "Nama Montir       : ";
    string namaMontir;
    getline(cin, s.nama_montir);

    int totalServices = data::countServices();
    helper::generateID('S', totalServices, &s.id_service);

    cout << "Nama Customer       : ";
    string namaCustomer;
    getline(cin, namaCustomer);

    if (data::findCustomerIdByName(namaCustomer) == "")
    {
        cout << "Pelanggan tidak ditemukan. Daftar Pelanggan Baru.\n";

        owner = new data::Customer;

        cout << "Nama Pelanggan       : ";
        getline(cin, owner->nama);
        cout << "Nomor Telepon       : ";
        getline(cin, owner->nomor_telepon);
        cout << "Alamat              : ";
        getline(cin, owner->alamat);
        cout << "Umur                : ";
        cin >> owner->umur;
        cout << "Gender (L/P)       : ";
        cin >> owner->gender;

        int totalCustomers = data::countCustomers();
        helper::generateID('C', totalCustomers, &owner->id_customer);
        data::insertCustomerBelakang(*owner);
        data::saveCustomer(*owner);

        s.id_customer = owner->id_customer;
    }
    else
    {

        s.id_customer = data::findCustomerIdByName(namaCustomer);

        owner = data::findCustomerById(s.id_customer);

        if (owner == NULL)
        {
            cout << "Customer tidak ditemukan. Service gagal ditambahkan.\n";
            return;
        }
    }

    s.data_customer = owner;
    s.next = NULL;

    data::insertServiceBelakangToCustomer(owner, s);
    data::saveService(s);

    cout << "Service berhasil ditambahkan.\n";
}

void mechanic_job_history()
{
    int pilMontir;
    cout << "\nPilih Montir: \n\n";
    cout << "1. Suby \n";
    cout << "2. Farhan \n";
    cout << "3. Dimas \n";
    cout << "4. Fajar \n";

    cout << "Masukkan Pilihan: ";
    cin >> pilMontir;

    string namaMontir = (pilMontir == 1) ? "Suby" : (pilMontir == 2) ? "Farhan"
                                                : (pilMontir == 3)   ? "Dimas"
                                                : (pilMontir == 4)   ? "Fajar"
                                                                     : "";
    data::Customer *c = data::headCustomer;
    bool found = false;

    cout << "\nRiwayat Kerja Montir : " << namaMontir << "\n\n";
    while (c != NULL)
    {
        data::Service *s = c->head_service;
        while (s != NULL)
        {
            if (s->nama_montir == namaMontir)
            {
                found = true;

                cout << "--------------------\n"
                     << "Model Mobil: " << s->model_mobil << "\n"
                     << "Merek Mobil: " << s->merek_mobil << "\n"
                     << "Deskripsi Kendala: " << s->deskripsi_kendala << "\n"
                     << "Nama Pelanggan: " << (c ? c->nama : "Unknown") << "\n"
                     << "Nomor Telepon Pelanggan: " << (c ? c->nomor_telepon : "-") << "\n"
                     << "--------------------\n";
            }
            s = s->next_in_customer;
        }
        c = c->next;
    }

    if (!found)
    {
        cout << "Tidak ada riwayat untuk montir tersebut.\n";
    }
}

int main()
{
    data::init();

    if (!data::loadAllCustomers())
    {
        cout << "Gagal membuka customers.csv\n";
    }

    if (!data::loadAllServices())
    {
        cout << "Gagal membuka services.csv\n";
    }

    while (true)
    {
        main_menu();
    }

    return 0;
}