#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>

#define DB_CUSTOMERS "./src/database/customers.csv"


namespace service {
    struct Service;
}

namespace customer
{
    struct Customer
    {
        std::string id;
        std::string name;
        int age;
        char gender;
        std::string phone;
        std::string address;

      
        Customer *prev;
        Customer *next;

        service::Service *head_service;
    };

    extern Customer *head;
    extern Customer *tail;

 
    Customer* find(std::string id);         
    Customer* findByName(std::string name); 
    
    void insert(Customer newCustomer);      
    bool save(Customer newCustomer);      

    Customer* add();

  
    bool load();                          
    int count();                           
    bool isEmpty();                         

    // Menampilkan Data
    void all();                            

}

#endif // !CUSTOMER_H