#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include "mechanic.h"
#include "customer.h"
#define DB_SERVICES "./src/database/services.csv"
#define DB_SERVICES_TEMP "./src/database/services_temp.csv"

namespace service
{
    enum Status
    {
        PROCESS,
        DONE,
        CANCELED
    };

    struct Service
    {
        std::string id;
        std::string id_cust;
        std::string id_mech;
        std::string model;
        std::string brand;
        std::string issue;

        std::string date_in;
        std::string date_out;
        int priority;

        Status status;

        mechanic::Mechanic *mech_data;
        customer::Customer *cust_data;

        Service *prev;
        Service *next;
        Service *next_cust;
        Service *next_q;
        Service *next_hist;
        Service *next_mech_q;

        Service *next_stack;

        std::string cust_review;
        int rating;
    };

    extern Service *head, *tail, *front_queue, *rear_queue;
    extern int total_queue;

    extern Service *top_cancel_stack;
    extern int total_cancel_stack;

    void add();
    Service *find(std::string id);
    bool update(Service *service);
    bool finish(std::string id);

    void assign(customer::Customer *c, Service *s, mechanic::Mechanic *m);

    void all();
    void queue();
    void history(std::string custName);
    bool save(Service *newService);

    int count();
    bool load();
    bool loadQueue();
    void enqueueService(Service *newService);

    void pushCancel(Service *service);
    Service *popCancel();

    bool cancelService(std::string id_servis, std::string id_customer);
    bool undoCancelService(std::string id_servis, std::string id_customer, std::string new_date);

    std::string addOneDay(std::string date_str);
    std::string findAvailableDateRecursive(std::string target_date, int attempt_count, std::string exclude_id = "");

    // helper
    long dateToInt(std::string d);
    bool compareServices(service::Service *a, service::Service *b, int mode);
    void insertionSort(service::Service *arr[], int n, int mode);
    void swapService(service::Service **a, service::Service **b);
    int partition(service::Service *arr[], int low, int high, int mode);
    void quickSort(service::Service *arr[], int low, int high, int mode);

}

#endif // !SERVICE_H