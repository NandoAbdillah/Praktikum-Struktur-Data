#include "helper.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;

namespace helper
{
    string getCSVColumn(string line, int colIndex)
    {
        string currentWord = "";
        int currentCol = 0;

        for (int i = 0; i < line.length(); i++)
        {
            if (line[i] == ',')
            {
                if (currentCol == colIndex)
                {
                    return currentWord;
                }
                currentCol++;
                currentWord = "";
            }
            else
            {
                currentWord += line[i];
            }
        }

        if (currentCol == colIndex)
        {
            return currentWord;
        }

        return "";
    }

    bool saveCSVData(string filename, string *dataLines, int totalLines)
    {
        ofstream file(filename, ios::trunc);

        if (file.is_open())
        {
            for (int i = 0; i < totalLines; i++)
            {
                file << dataLines[i] << endl;
            }
            file.close();
            return true;
        }
        else
        {
            return false;
        }
    }

    void generateID(char prefix, int totalData, string *outID)
    {
        int newNumber = totalData + 1;

        string numberPart;
        if (newNumber < 10)
        {
            numberPart = "0" + to_string(newNumber);
        }
        else
        {
            numberPart = to_string(newNumber);
        }

        *outID = "";
        *outID += prefix;
        *outID += numberPart;

        cout << "Generated ID: " << *outID << endl;
    }

    int filterHanyaAngka(ImGuiInputTextCallbackData *data)
    {
        // Hanya izinkan angka 0-9
        if (data->EventChar >= '0' && data->EventChar <= '9')
            return 0;

        return 1;
    }

    string getCurrentDate()
    {
        time_t now = time(0);
        tm *ltm = localtime(&now);

        int year = 1900 + ltm->tm_year;
        int month = 1 + ltm->tm_mon;
        int day = ltm->tm_mday;

        stringstream ss;
        ss << setw(4) << setfill('0') << year << "-"
           << setw(2) << setfill('0') << month << "-"
           << setw(2) << setfill('0') << day;

        return ss.str();
    }
}