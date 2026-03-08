#include "data.h"
#include "utils/helper.h"
#include <iostream>
#include <fstream>

using namespace std;

namespace data
{
    Customer *headCustomer, *tailCustomer;

    Service *headService, *tailService;

    void init()
    {
        headCustomer = NULL;
        tailCustomer = NULL;

        headService = NULL;
        tailService = NULL;
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

    void insertServiceBelakangToCustomer(Customer *customer, Service newService)
    {
        if (customer == NULL)
            return;

        Service *baru = new Service;
        *baru = newService;

        if (headService == NULL)
        {
            headService = tailService = baru;
        }
        else
        {
            tailService->next = baru;
            baru->prev = tailService;
            tailService = baru;
        }

        baru->data_customer = customer;

        baru->next = NULL;
        baru->prev = NULL;
        baru->next_in_customer = NULL;

        if (customer->head_service == NULL)
        {
            customer->head_service = baru;
        }
        else
        {
            Service *bantu = customer->head_service;
            while (bantu->next_in_customer != NULL)
            {
                bantu = bantu->next_in_customer;
            }
            bantu->next_in_customer = baru;
        }
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

            Service s;
            s.id_service = helper::getCSVColumn(line, 0);
            s.id_customer = helper::getCSVColumn(line, 1);
            s.model_mobil = helper::getCSVColumn(line, 2);
            s.merek_mobil = helper::getCSVColumn(line, 3);
            s.deskripsi_kendala = helper::getCSVColumn(line, 4);
            s.nama_montir = helper::getCSVColumn(line, 5);
            s.data_customer = NULL;
            s.next = NULL;

            Customer *owner = findCustomerById(s.id_customer);
            if (owner != NULL)
            {
                insertServiceBelakangToCustomer(owner, s);
            }
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

    bool saveService(Service newService)
    {
        ofstream file(DB_SERVICES, ios::app);
        if (!file.is_open())
        {
            return false;
        }

        file << endl
             << newService.id_service << ","
             << newService.id_customer << ","
             << newService.model_mobil << ","
             << newService.merek_mobil << ","
             << newService.deskripsi_kendala << ","
             << newService.nama_montir;

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
                cout << "Nama Montir      : " << last->nama_montir << endl;
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
            cout << "Nama Montir        : " << s->nama_montir << endl;
            cout << "Nama Pelanggan     : " << (s->data_customer ? s->data_customer->nama : "Unknown") << endl;
            cout << "No Telepon         : " << (s->data_customer ? s->data_customer->nomor_telepon : "-") << endl;
            cout << "-----------------------------" << endl;

            serviceCount++;
            s = s->next;
        }
    }
}