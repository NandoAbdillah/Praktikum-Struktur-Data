#ifndef MECHANIC_H
#define MECHANIC_H
#define DB_MECHANICS "./src/database/mechanics.csv"

#include <string>

namespace service {
    struct Service;
}

namespace mechanic
{
   
    struct Mechanic
    {
        std::string id;
        std::string name;

       
        service::Service *front_queue;
        service::Service *rear_queue;

        service::Service *head_history;

        Mechanic *prev;
        Mechanic *next;
    };

    extern Mechanic *head;
    extern Mechanic *tail;

    extern Mechanic *list_mechanics[100];



    void init();
    

    Mechanic* find(std::string id);          
    void insert(Mechanic *newMechanic);        
    Mechanic* findByName(std::string name); 
    
    bool load();                             
    int count();                           
    bool save(Mechanic *newMechanic);       

    // void history();

}

#endif // !MECHANIC_H