#ifndef DATA_H
#define DATA_H

#include <string>
#define MAX_DATA_CACHE 100
#define DB_CUSTOMERS "./src/database/customers.csv"
#define DB_SERVICES "./src/database/services.csv"

using namespace std;

namespace data
{
    struct Customer;

    struct Service
    {
        string id_service;
        string id_customer;
        string model_mobil;
        string merek_mobil;
        string deskripsi_kendala;
        string nama_montir;

        Service *prev;
        Service *next;
        Service *next_in_customer;

        Customer *data_customer;
    };

    struct Customer
    {
        string id_customer;
        string nama;
        int umur;
        char gender;
        string nomor_telepon;
        string alamat;

        Customer *prev;
        Customer *next;

        Service *head_service;
    };

    extern Customer *headCustomer;
    extern Customer *tailCustomer;

    void init();

    bool isCustomerEmpty();
    void insertCustomerBelakang(Customer newCustomer);
    Customer *findCustomerById(string id_customer);

    void insertServiceBelakangToCustomer(Customer *customer, Service newService);

    int countCustomers();
    int countServices();

    bool loadAllCustomers();
    bool loadAllServices();

    bool saveCustomer(Customer newCustomer);
    bool saveService(Service newService);

    string findCustomerIdByName(string nama);

    void showAllCustomers();
    void showAllServices();
}

#endif