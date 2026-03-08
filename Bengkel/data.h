#ifndef DATA_H
#define DATA_H

#include <string>
#define MAX_DATA_CACHE 100
#define DB_CUSTOMERS "./database/customers.csv"
#define DB_SERVICES "./database/services.csv"
#define DB_MECHANICS "./database/mechanics.csv"

using namespace std;

namespace data
{
    struct Customer;
    struct Mechanic;

    struct Service
    {
        string id_service;
        string id_customer;
        string id_montir;
        string model_mobil;
        string merek_mobil;
        string deskripsi_kendala;

        string tanggal_masuk_bengkel;
        string tanggal_selesai;

        enum status_service
        {
            DIPROSES,
            SELESAI
        } status;

        Mechanic *data_montir;

        Service *prev;
        Service *next;

        Service *next_in_customer;
        Service *next_in_queue;
        Service *next_in_history;
        Service *next_in_mechanic_queue;

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


    struct Mechanic
    {
        string id_mechanic;
        string nama;
        Service *front_service_queue;
        Service *rear_service_queue;

        Service *head_service_history;
        
        Mechanic *prev;
        Mechanic *next;
    };

    extern Customer *headCustomer;
    extern Customer *tailCustomer;

    void init();

    Customer *findCustomerById(string id_customer);

    bool isCustomerEmpty();
    void insertCustomerBelakang(Customer newCustomer);
    void insertServiceBelakangToCustomer(Customer *customer, Service *newService);
    void tambahServiceBaru();
    void selesaikanService();
    void riwayatKerjaMontir();

    Customer *tambahCustomerBaru();

    int countCustomers();
    int countServices();

    bool loadAllCustomers();
    bool loadAllMechanics();
    bool loadAllServices();
    bool loadServiceQueue();

    bool saveCustomer(Customer newCustomer);
    bool saveService(Service *newService);

    string findCustomerIdByName(string nama);

    void showAllCustomers();
    void showAllServices();
    void showServiceQueue();
    void showCustomerServiceHistory(string namaPelanggan);
}

#endif