#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

const string DB_CUSTOMERS = "customers.csv";
const string DB_SERVICES = "services.csv";

struct Customer
{
    string id_customer, nama, nomor_telepon, alamat;
    int umur;
    char gender;
    Customer *prev = NULL, *next = NULL;
};

struct Service
{
    string id_service, id_customer, model_mobil, merek_mobil, deskripsi_kendala, nama_montir;
    Service *prev = NULL, *next = NULL;
};

Customer *headCust = NULL, *tailCust = NULL;
Service *headServ = NULL, *tailServ = NULL;

int totalCustomers = 0;
int totalServices = 0;

string generateId(string prefix, int count)
{
    return prefix + (count < 10 ? "0" : "") + to_string(count);
}

void insertCustomer(Customer c)
{
    Customer *baru = new Customer(c);
    if (!headCust)
        headCust = tailCust = baru;
    else
    {
        tailCust->next = baru;
        baru->prev = tailCust;
        tailCust = baru;
    }
    totalCustomers++;
}

void insertService(Service s)
{
    Service *baru = new Service(s);
    if (!headServ)
        headServ = tailServ = baru;
    else
    {
        tailServ->next = baru;
        baru->prev = tailServ;
        tailServ = baru;
    }
    totalServices++;
}

void loadAllCustomers()
{
    ifstream file(DB_CUSTOMERS);
    string line, token;
    while (getline(file, line))
    {
        if (line.empty())
            continue;
        stringstream ss(line);
        Customer c;
        getline(ss, c.id_customer, ',');
        getline(ss, c.nama, ',');
        getline(ss, token, ',');
        c.umur = token.empty() ? 0 : stoi(token);
        getline(ss, token, ',');
        c.gender = token.empty() ? '-' : token[0];
        getline(ss, c.nomor_telepon, ',');
        getline(ss, c.alamat, ',');
        insertCustomer(c);
    }
}

void loadAllServices()
{
    ifstream file(DB_SERVICES);
    string line;
    while (getline(file, line))
    {
        if (line.empty())
            continue;
        stringstream ss(line);
        Service s;
        getline(ss, s.id_service, ',');
        getline(ss, s.id_customer, ',');
        getline(ss, s.model_mobil, ',');
        getline(ss, s.merek_mobil, ',');
        getline(ss, s.deskripsi_kendala, ',');
        getline(ss, s.nama_montir, ',');
        insertService(s);
    }
}

void saveCustomerCSV(Customer c)
{
    ofstream file(DB_CUSTOMERS, ios::app);
    if (file.is_open())
        file << endl
             << c.id_customer << "," << c.nama << "," << c.umur << "," << c.gender << "," << c.nomor_telepon << "," << c.alamat;
}

void saveServiceCSV(Service s)
{
    ofstream file(DB_SERVICES, ios::app);
    if (file.is_open())
        file << endl
             << s.id_service << "," << s.id_customer << "," << s.model_mobil << "," << s.merek_mobil << "," << s.deskripsi_kendala << "," << s.nama_montir;
}

Customer *findCustomerById(string id)
{
    for (Customer *bantu = headCust; bantu != NULL; bantu = bantu->next)
        if (bantu->id_customer == id)
            return bantu;
    return NULL;
}

string findCustomerIdByName(string nama)
{
    for (Customer *bantu = headCust; bantu != NULL; bantu = bantu->next)
        if (bantu->nama == nama)
            return bantu->id_customer;
    return "";
}

void main_menu();
void menu_services();
void all_services();
void new_service();
void mechanic_job_history();
void all_customers();
void customer_data();

void main_menu()
{
    int choice;
    cout << "\nWelcome to Lognuts!\nPilih Menu:\n\n1. Servis\n2. Semua Data Pelanggan\n3. Data Pelanggan\n4. Keluar\n\nMasukkan Pilihan Anda: ";
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
    int choice;
    cout << "\n=== Servis ===\nPilih Menu?\n\n1. Semua Servis Singkat\n2. Servis Baru\n3. Riwayat Kerja Montir\n\nMasukkan Pilihan Anda: ";
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
    default:
        cout << "Pilihan tidak valid!\n";
        break;
    }
}

void all_services()
{
    cout << "\n=== Semua Servis ===\n\n";
    if (!headServ)
        cout << "Data servis kosong.\n";

    for (Service *s = headServ; s != NULL; s = s->next)
    {
        Customer *c = findCustomerById(s->id_customer);
        cout << "--------------------\n"
             << "Model Mobil: " << s->model_mobil << "\n"
             << "Merek Mobil: " << s->merek_mobil << "\n"
             << "Deskripsi Kendala: " << s->deskripsi_kendala << "\n"
             << "Nama Montir: " << s->nama_montir << "\n"
             << "Nama Customer: " << (c ? c->nama : "Unknown") << "\n"
             << "Nomor Telepon Customer: " << (c ? c->nomor_telepon : "-") << "\n"
             << "--------------------\n";
    }
}

void new_service()
{
    cout << "\n=== Servis Baru ===\n";
    Service s;
    string namaCust;

    cin.ignore();
    cout << "Masukkan Model Mobil: ";
    getline(cin, s.model_mobil);
    cout << "Masukkan Merek Mobil: ";
    getline(cin, s.merek_mobil);
    cout << "Masukkan Deskripsi Kendala: ";
    getline(cin, s.deskripsi_kendala);

    while (true)
    {
        cout << "Masukkan Nama Montir (Abdul/Adit/Agus): ";
        getline(cin, s.nama_montir);

        if (s.nama_montir == "Abdul" || s.nama_montir == "Adit" || s.nama_montir == "Agus")
        {
            break;
        }
        else
        {
            cout << "[!] Montir tidak tersedia. Harap masukkan nama sesuai pilihan.\n\n";
        }
    }

    cout << "Masukkan Nama Customer: ";
    getline(cin, namaCust);

    s.id_customer = findCustomerIdByName(namaCust);

    if (s.id_customer == "")
    {
        Customer newCust;
        string umurStr;
        newCust.nama = namaCust;
        cout << "Masukkan Nomor Telepon Customer: ";
        getline(cin, newCust.nomor_telepon);
        cout << "Masukkan Alamat Customer: ";
        getline(cin, newCust.alamat);
        cout << "Masukkan Umur Customer: ";
        getline(cin, umurStr);
        newCust.umur = umurStr.empty() ? 0 : stoi(umurStr);

        while (true)
        {
            cout << "Masukkan Gender Customer (L/P): ";
            cin >> newCust.gender;

            if (newCust.gender == 'L' || newCust.gender == 'l' || newCust.gender == 'P' || newCust.gender == 'p')
            {
                newCust.gender = toupper(newCust.gender);
                break;
            }
            else
            {
                cout << "[!] Gender tidak valid. Harap masukkan L atau P.\n\n";
            }
        }

        newCust.id_customer = generateId("C", totalCustomers + 1);
        s.id_customer = newCust.id_customer;

        insertCustomer(newCust);
        saveCustomerCSV(newCust);
        cout << "\n*Pelanggan baru telah terdaftar, data servis berhasil dicatat*\n";
    }
    else
    {
        cout << "\n*Pelanggan sudah terdaftar dan servis berhasil dicatat*\n";
    }

    s.id_service = generateId("S", totalServices + 1);
    insertService(s);
    saveServiceCSV(s);
}

void mechanic_job_history()
{
    int pilMontir;
    cout << "\n=== Riwayat Kerja Montir ===\nPilih Montir:\n1. Abdul\n2. Adit\n3. Agus\nMasukkan Pilihan Anda: ";
    cin >> pilMontir;

    string namaMontir = (pilMontir == 1) ? "Abdul" : (pilMontir == 2) ? "Adit"
                                                 : (pilMontir == 3)   ? "Agus"
                                                                      : "";
    if (namaMontir == "")
    {
        cout << "Pilihan tidak valid.\n";
        return;
    }

    cout << "\n=== Riwayat Montir " << namaMontir << " ===\n\n";
    bool found = false;

    for (Service *s = headServ; s != NULL; s = s->next)
    {
        if (s->nama_montir == namaMontir)
        {
            Customer *c = findCustomerById(s->id_customer);
            cout << "--------------------\n"
                 << "Model Mobil: " << s->model_mobil << "\n"
                 << "Merek Mobil: " << s->merek_mobil << "\n"
                 << "Deskripsi Kendala: " << s->deskripsi_kendala << "\n"
                 << "Nama Montir: " << s->nama_montir << "\n"
                 << "Nama Customer: " << (c ? c->nama : "Unknown") << "\n"
                 << "Nomor Telepon Customer: " << (c ? c->nomor_telepon : "-") << "\n"
                 << "--------------------\n";
            found = true;
        }
    }
    if (!found)
        cout << "Tidak ada riwayat kerja untuk montir " << namaMontir << ".\n";
}

void all_customers()
{
    cout << "\n=== Semua Data Pelanggan ===\n\n";
    if (!headCust)
    {
        cout << "Data customer kosong.\n";
        return;
    }

    for (Customer *curr = headCust; curr != NULL; curr = curr->next)
    {
        cout << "--------------------\n"
             << "Nama Customer: " << curr->nama << "\n"
             << "Nomor Telepon Customer: " << curr->nomor_telepon << "\n"
             << "Alamat Customer: " << curr->alamat << "\n"
             << "--- Servis Terakhir ---\n";

        bool found = false;
        for (Service *s = tailServ; s != NULL; s = s->prev)
        {
            if (s->id_customer == curr->id_customer)
            {
                cout << "Model Mobil: " << s->model_mobil << "\n"
                     << "Deskripsi Kendala: " << s->deskripsi_kendala << "\n"
                     << "Nama Montir: " << s->nama_montir << "\n";
                found = true;
                break;
            }
        }
        if (!found)
            cout << "Belum ada riwayat servis.\n";
        cout << "--------------------\n";
    }
}

void customer_data()
{
    string namaCust;
    cout << "\nMasukkan Nama Customer: ";
    cin.ignore();
    getline(cin, namaCust);

    Customer *curr = findCustomerById(findCustomerIdByName(namaCust));
    if (!curr)
    {
        cout << "Customer tidak ditemukan.\n";
        return;
    }

    char navOption;
    while (true)
    {
        cout << "\n=== Data Pelanggan ===\n"
             << "Nama Customer: " << curr->nama << "\n"
             << "Nomor Telepon Customer: " << curr->nomor_telepon << "\n"
             << "Umur Customer: " << curr->umur << "\n"
             << "Gender Customer: " << curr->gender << "\n"
             << "Alamat Customer: " << curr->alamat << "\n\n"
             << "--- 3 Servis Terakhir ---\n";

        int count = 0;
        bool found = false;
        for (Service *s = tailServ; s != NULL && count < 3; s = s->prev)
        {
            if (s->id_customer == curr->id_customer)
            {
                cout << "--------------------\n"
                     << "Model Mobil: " << s->model_mobil << "\n"
                     << "Deskripsi Kendala: " << s->deskripsi_kendala << "\n"
                     << "Nama Montir: " << s->nama_montir << "\n"
                     << "--------------------\n";
                found = true;
                count++;
            }
        }
        if (!found)
            cout << "Belum ada data servis untuk pelanggan ini.\n";

        cout << "\n[N]ext, [P]revious, [E]xit\nPilihan: ";
        cin >> navOption;

        if (toupper(navOption) == 'N')
        {
            if (curr->next)
                curr = curr->next;
            else
                cout << "\n[!] Ini adalah data pelanggan terakhir.\n\n";
        }
        else if (toupper(navOption) == 'P')
        {
            if (curr->prev)
                curr = curr->prev;
            else
                cout << "\n[!] Ini adalah data pelanggan pertama.\n\n";
        }
        else if (toupper(navOption) == 'E')
        {
            cout << "\nKeluar dari menu pelanggan...\n";
            break;
        }
        else
        {
            cout << "\n[!] Pilihan tidak valid.\n\n";
        }
    }
}

int main()
{
    loadAllCustomers();
    loadAllServices();
    while (true)
        main_menu();
    return 0;
}