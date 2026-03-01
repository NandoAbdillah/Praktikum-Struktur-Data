#include "helper.h"
#include <iostream>
#include <string>
#include <fstream>

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
    }
}