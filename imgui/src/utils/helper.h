#ifndef HELPER_H
#define HELPER_H
#include "imgui.h"

#include <string>

using namespace std;

namespace helper
{
    string getCSVColumn(string line, int colIndex);
    bool saveCSVData(string filename, string *dataLines, int totalLines);
    void generateID(char prefix, int totalData, string *outID);
    string getCurrentDate();
    int filterHanyaAngka(ImGuiInputTextCallbackData *data);
}

#endif