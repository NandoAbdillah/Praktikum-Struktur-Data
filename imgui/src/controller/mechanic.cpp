#include <iostream>
#include <fstream>
#include <cstdio>
#include <filesystem>
#include "controller/mechanic.h"
#include "controller/customer.h"
#include "controller/service.h" // Wajib di-include karena ada forward declaration di mechanic.h
#include "utils/helper.h"

using namespace std;

namespace mechanic
{
    // Inisialisasi pointer global (dari extern di header)
    Mechanic *head = NULL;
    Mechanic *tail = NULL;

    Mechanic *list_mechanics[100];

    static int mechanic_count = 0;

    void init()
    {
        head = NULL;
        tail = NULL;
    }

    Mechanic *find(std::string id)
    {
        Mechanic *temp = head;
        while (temp != NULL)
        {
            if (temp->id == id)
            {
                return temp;
            }
            temp = temp->next;
        }
        return NULL;
    }

    // Mencari montir berdasarkan nama
    Mechanic *findByName(std::string name)
    {
        Mechanic *temp = head;
        while (temp != NULL)
        {
            if (temp->name == name)
            {
                return temp;
            }
            temp = temp->next;
        }
        return NULL;
    }

    // Menghitung total montir terdaftar
    int count()
    {
        int total = 0;
        Mechanic *temp = head;
        while (temp != NULL)
        {
            total++;
            temp = temp->next;
        }
        return total;
    }

    // Menyimpan 1 data montir baru ke CSV
    bool save(Mechanic *newMechanic)
    {
        if (newMechanic == NULL)
            return false;

        std::ofstream file(DB_MECHANICS, std::ios::app);
        if (!file.is_open())
        {
            return false;
        }

        file << endl
             << newMechanic->id << "," << newMechanic->name;

        file.close();
        return true;
    }

    bool load()
    {
        ifstream file(DB_MECHANICS);

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

            Mechanic *m = new Mechanic;
            m->id = helper::getCSVColumn(line, 0);
            m->name = helper::getCSVColumn(line, 1);

            list_mechanics[mechanic_count++] = m;

            insert(m);
        }

        file.close();
        return true;
    }

    void insert(Mechanic *newMechanic)
    {
        Mechanic *baru = new Mechanic;
        *baru = *newMechanic;

        baru->front_queue = NULL;
        baru->rear_queue = NULL;
        baru->head_history = NULL;

        baru->next = NULL;
        baru->prev = NULL;

        if (head == NULL)
        {
            head = tail = baru;
        }
        else
        {
            tail->next = baru;
            baru->prev = tail;
            tail = baru;
        }

        service::Service *s = service::head;

        while (s != NULL)
        {

            if (s->id_mech == baru->id)
            {
                s->mech_data = baru;

                if (s->status == service::PROCESS)
                {
                    if (baru->front_queue == NULL)
                    {
                        baru->rear_queue = baru->front_queue = s;
                    }
                    else
                    {
                        baru->rear_queue->next_mech_q = s;
                        baru->rear_queue = s;
                    }

                    s->next_mech_q = NULL;
                }

                if (baru->head_history == NULL)
                {
                    baru->head_history = s;
                    s->next_hist = NULL;
                }
                else
                {
                    service::Service *historyBantu = baru->head_history;
                    while (historyBantu->next_hist != NULL)
                    {
                        historyBantu = historyBantu->next_hist;
                    }
                    historyBantu->next_hist = s;
                    s->next_hist = NULL;
                }
            }

            s = s->next;
        }
    }

    // void history()
    // {
    // }

}