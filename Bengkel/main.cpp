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
    cout << "\nWelcome To Lognuts\n";
    cout << "1. Servis\n";
    cout << "2. Semua Data Pelanggan\n";
    cout << "3. Data Pelanggan\n";
    cout << "4. Keluar\n";
    cout << "Masukkan Pilihan : ";
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
    cout << "\nServices\n";
    cout << "1. Semua Servis Singkat\n";
    cout << "2. Servis Baru\n";
    cout << "3. Riwayat Kerja Montir\n";
    cout << "Masukkan Pilihan : ";
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
        cout << "Pilihan tidak valid!\n";
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
    string id;
    cout << "\nMasukkan ID Customer: ";
    cin >> id;

    data::Customer *c = data::findCustomerById(id);
    if (c == NULL)
    {
        cout << "Customer tidak ditemukan.\n";
        return;
    }

    cout << "Nama          : " << c->nama << endl;
    cout << "Nomor Telepon : " << c->nomor_telepon << endl;
    cout << "Umur          : " << c->umur << endl;
    cout << "Gender        : " << c->gender << endl;
    cout << "Alamat        : " << c->alamat << endl;

    cout << "\n___ Riwayat Service ___\n";
    data::Service *s = c->head_service;
    if (s == NULL)
    {
        cout << "Belum ada service.\n";
    }
    else
    {
        while (s != NULL)
        {
            cout << "- " << s->id_service << " | "
                 << s->model_mobil << " | "
                 << s->merek_mobil << " | "
                 << s->deskripsi_kendala << " | "
                 << s->nama_montir << endl;
            s = s->next;
        }
    }
}

void new_service()
{
    data::Service s;
    data::Customer *owner = NULL;

    cout << "\nInput Service Baru\n";

    helper::generateID('S', data::countServices(), &s.id_service);

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

    cout << "Nama Customer       : ";
    string namaCustomer;
    getline(cin, namaCustomer);
    if (data::findCustomerIdByName(namaCustomer) == "")
    {
        cout << "Customer tidak ditemukan. Service gagal ditambahkan.\n";
        return;
    }

    s.id_customer = data::findCustomerIdByName(namaCustomer);

    owner = data::findCustomerById(s.id_customer);

    if (owner == NULL)
    {
        cout << "Customer tidak ditemukan. Service gagal ditambahkan.\n";
        return;
    }

    s.data_customer = owner;
    s.next = NULL;

    data::insertServiceBelakangToCustomer(owner, s);
    data::saveService(s);

    cout << "Service berhasil ditambahkan.\n";
}

void mechanic_job_history()
{
    string namaMontir;
    cout << "\nMasukkan nama montir: ";
    cin.ignore();
    getline(cin, namaMontir);

    data::Customer *c = data::headCustomer;
    bool found = false;

    while (c != NULL)
    {
        data::Service *s = c->head_service;
        while (s != NULL)
        {
            if (s->nama_montir == namaMontir)
            {
                found = true;
                cout << s->id_service << " | "
                     << s->model_mobil << " | "
                     << s->merek_mobil << " | "
                     << s->deskripsi_kendala << " | Customer: "
                     << c->nama << endl;
            }
            s = s->next;
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