#include <iostream>
#include <fstream>
#include <cstdio>
#include <filesystem>
#include "controller/customer.h"
#include "controller/service.h" 
#include "controller/mechanic.h"
#include "utils/helper.h"

using namespace std;

namespace customer
{
    // Inisialisasi variabel global (dari extern)
    Customer *head = NULL;
    Customer *tail = NULL;

    // Tambahan: Implementasi fungsi pencarian
    Customer *find(std::string id)
    {
        Customer *temp = head;
        while (temp != NULL)
        {
            if (temp->id == id)
                return temp;
            temp = temp->next;
        }
        return NULL;
    }

    // Tambahan: Implementasi fungsi pencarian berdasarkan nama
    Customer *findByName(std::string name)
    {
        Customer *temp = head;
        while (temp != NULL)
        {
            if (temp->name == name)
                return temp;
            temp = temp->next;
        }
        return NULL;
    }

    bool load()
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
            c.id = helper::getCSVColumn(line, 0);
            c.name = helper::getCSVColumn(line, 1);

            string umurStr = helper::getCSVColumn(line, 2);
            c.age = umurStr.empty() ? 0 : stoi(umurStr);

            string genderStr = helper::getCSVColumn(line, 3);
            c.gender = genderStr.empty() ? '-' : genderStr[0];

            c.phone = helper::getCSVColumn(line, 4);
            c.address = helper::getCSVColumn(line, 5);

            c.head_service = NULL;
            c.next = NULL;

            insert(c);
        }

        file.close();
        return true;
    }

    bool save(Customer newCustomer)
    {
        ofstream file(DB_CUSTOMERS, ios::app);
        if (!file.is_open())
        {
            return false;
        }

        file << endl
             << newCustomer.id << ","
             << newCustomer.name << ","
             << newCustomer.age << ","
             << newCustomer.gender << ","
             << newCustomer.phone << ","
             << newCustomer.address;

        file.close();
        return true;
    }

    int count()
    {
        int total = 0;
        Customer *bantu = head;
        while (bantu != NULL)
        {
            total++;
            bantu = bantu->next;
        }
        return total;
    }

    bool isEmpty()
    {
        return head == NULL;
    }

    void insert(Customer newCustomer)
    {
        Customer *baru = new Customer;
        *baru = newCustomer;
        baru->head_service = NULL;
        baru->next = NULL;
        baru->prev = NULL;

        if (isEmpty())
        {
            head = tail = baru;
        }
        else
        {
            tail->next = baru;
            baru->prev = tail;
            tail = baru;
        }
    }

    void all()
    {
        Customer *c = head;

        if (c == NULL)
        {
            cout << "Data customer kosong.\n";
            return;
        }

        while (c != NULL)
        {
            cout << "Nama              : " << c->name << endl;
            cout << "Nomor Telepon     : " << c->phone << endl;
            cout << "Alamat            : " << c->address << endl;

            cout << "--- Servis Terakhir ---" << endl;

            if (c->head_service != NULL)
            {
                service::Service *last = c->head_service;
                // Menggunakan variabel yang sudah disingkat dari service.h
                while (last->next_cust != NULL)
                {
                    last = last->next_cust;
                }

                cout << "Mobil              : " << last->model << endl;
                cout << "Deskripsi Kendala  : " << last->issue << endl;
                cout << "Nama Montir        : " << (last->mech_data ? last->mech_data->name : "-") << endl;
            }
            else
            {
                cout << "Servis Terakhir: Belum ada" << endl;
            }

            cout << "-----------------------------" << endl;
            c = c->next;
        }
    }

    Customer *add()
    {
        Customer *c = new Customer;

        cout << "Nama Pelanggan       : ";
        getline(cin, c->name);

        Customer *existingCustomer = customer::findByName(c->name);
        if (existingCustomer != NULL)
        {
            cout << "Pelanggan dengan nama ini sudah ada. Silahkan menggunakan nama lain.\n";
            delete c;
            return existingCustomer;
        }

        cout << "Nomor Telepon        : ";
        getline(cin, c->phone);
        cout << "Alamat               : ";
        getline(cin, c->address);
        cout << "Umur                 : ";
        cin >> c->age;
        cout << "Gender (L/P)         : ";
        cin >> c->gender;

        int totalCustomers = customer::count();
        helper::generateID('C', totalCustomers, &c->id);

        c->head_service = NULL;
        c->next = NULL;
        c->prev = NULL;

        customer::insert(*c);
        customer::save(*c);

        string idBaru = c->id;

        delete c;

        cout << "Pelanggan berhasil ditambahkan dengan" << endl;

        return customer::find(idBaru);
    }


}