#include "data.h"
#include "utils/helper.h"
#include <iostream>
#include <fstream>

using namespace std;

namespace data
{
    Customer *headCustomer = NULL;
    Customer *tailCustomer = NULL;

    void init()
    {
        headCustomer = NULL;
        tailCustomer = NULL;
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

        if (isCustomerEmpty())
        {
            headCustomer = tailCustomer = baru;
        }
        else
        {
            tailCustomer->next = baru;
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
        baru->data_customer = customer;
        baru->next = NULL;

        if (customer->head_service == NULL)
        {
            customer->head_service = baru;
        }
        else
        {
            Service *bantu = customer->head_service;
            while (bantu->next != NULL)
            {
                bantu = bantu->next;
            }
            bantu->next = baru;
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
        Customer *c = headCustomer;
        while (c != NULL)
        {
            Service *s = c->head_service;
            while (s != NULL)
            {
                count++;
                s = s->next;
            }
            c = c->next;
        }
        return count;
    }

    bool loadAllCustomers()
    {
        ifstream file(DB_CUSTOMERS);
        if (!file.is_open())
        {
            return false;
        }

        string line;

        // skip header
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

        // skip header
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

        file << newCustomer.id_customer << ","
             << newCustomer.nama << ","
             << newCustomer.umur << ","
             << newCustomer.gender << ","
             << newCustomer.nomor_telepon << ","
             << newCustomer.alamat << endl;

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
             << newService.nama_montir << endl;

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
            cout << "ID          : " << c->id_customer << endl;
            cout << "Nama        : " << c->nama << endl;
            cout << "Umur        : " << c->umur << endl;
            cout << "Gender      : " << c->gender << endl;
            cout << "Telepon     : " << c->nomor_telepon << endl;
            cout << "Alamat      : " << c->alamat << endl;

            if (c->head_service != NULL)
            {
                Service *last = c->head_service;
                while (last->next != NULL)
                {
                    last = last->next;
                }

                cout << "Servis Terakhir: " << last->model_mobil
                     << " (" << last->merek_mobil << ")" << endl;
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
        Customer *c = headCustomer;
        bool ada = false;

        while (c != NULL)
        {
            Service *s = c->head_service;
            while (s != NULL)
            {
                ada = true;
                cout << "ID Service         : " << s->id_service << endl;
                cout << "ID Customer        : " << s->id_customer << endl;
                cout << "Model Mobil        : " << s->model_mobil << endl;
                cout << "Merek Mobil        : " << s->merek_mobil << endl;
                cout << "Deskripsi Kendala  : " << s->deskripsi_kendala << endl;
                cout << "Nama Montir        : " << s->nama_montir << endl;

                if (s->data_customer != NULL)
                {
                    cout << "Nama Customer      : " << s->data_customer->nama << endl;
                    cout << "No Telepon         : " << s->data_customer->nomor_telepon << endl;
                }

                cout << "-----------------------------" << endl;
                s = s->next;
            }
            c = c->next;
        }

        if (!ada)
        {
            cout << "Data service kosong.\n";
        }
    }
}