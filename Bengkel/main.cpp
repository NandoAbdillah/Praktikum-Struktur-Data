#include <iostream>
#include "data.h"
#include "utils/helper.h"

using namespace std;

int choice = 0;
bool isAdmin = false;

void main_menu();
void menu_services();
void all_services();
void new_service();
void mechanic_job_history();
void all_customers();
void customer_data();
void finish_service();
void main_menu_admin();

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
         << "4. Selesaikan Pekerjaan" << endl
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
    case 4:
        finish_service();
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
            cout << "Nama Montir        : " << s->data_montir->nama << endl;
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
    cout << "\nInput Service Baru\n";
    data::tambahServiceBaru();
}

void finish_service()
{
    cout << "\nSelesaikan Pekerjaan\n";
    data::selesaikanService();
}

void new_customer()
{
    cin.ignore();
    cout << "\nInput Customer Baru\n";
    data::tambahCustomerBaru();
}
void mechanic_job_history()
{
    cout << "\nRiwayat Kerja Montir\n";
    data::riwayatKerjaMontir();
}

void antrian_servis()
{
    cout << "\nAntrian Servis\n\n";
    data::showServiceQueue();
}

void riwayat_service()
{
    cout << "\nRiwayat Servis\n\n";
    cin.ignore();
    cout << "Masukkan Nama Pelanggan    : ";
    string namaPelanggan;
    getline(cin, namaPelanggan);
    data::showCustomerServiceHistory(namaPelanggan);
}

void main_menu_customer()
{
    string customerChoice;
    cout << endl
         << "Welcome To Lognuts" << endl
         << "1. Antrian Servis" << endl
         << "2. Riwayat Servis Anda" << endl
         << "3. Keluar" << endl
         << "Masukkan Pilihan : ";

    cin >> customerChoice;

    if (customerChoice == "1")
        antrian_servis();

    else if (customerChoice == "2")
        riwayat_service();

    else if (customerChoice == "3")
        exit(0);

    else if (customerChoice == "adminacces8008")
        isAdmin = true;

    else
        cout << "Pilihan tidak valid!\n";
}

void main_menu_admin()
{
    cout << endl
         << "Welcome To Lognuts" << endl
         << "1. Servis" << endl
         << "2. Pelanggan Baru" << endl
         << "3. Keluar dari Admin" << endl
         << "4. Keluar Aplikasi" << endl
         << "Masukkan Pilihan : ";
    cin >> choice;

    switch (choice)
    {
    case 1:
        menu_services();
        break;
    case 2:
        new_customer();
        break;
    case 3:
        isAdmin = false;
        break;
    case 4:
        exit(0);
        break;
    default:
        cout << "Pilihan tidak valid!\n";
        break;
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

    if (!data::loadAllMechanics())
    {
        cout << "Gagal membuka mechanics.csv\n";
    }

    if (!data::loadServiceQueue())
    {
        cout << "Gagal memuat antrian servis.\n";
    }

    while (true)
    {
        if (!isAdmin)
            main_menu_customer();
        else
            main_menu_admin();

    }

    return 0;
}