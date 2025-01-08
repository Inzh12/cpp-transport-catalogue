#include "input_reader.h"
#include "stat_reader.h"

#include <iostream>
#include <string>

using namespace std;
using namespace transport_catalogue;

int main() {
    TransportCatalogue catalogue;

    int base_request_count;
    cin >> base_request_count >> ws;

    {
        io::InputReader reader;
        for (int i = 0; i < base_request_count; ++i) {
            string line;
            getline(cin, line);
            reader.ParseLine(line);
        }
        reader.ApplyCommands(catalogue);
    }

    {
        io::StatReader reader {catalogue, cout};
        int stat_request_count;

        cin >> stat_request_count >> ws;

        for (int i = 0; i < stat_request_count; ++i) {
            string line;
            getline(cin, line);
            reader.ParseAndPrintStat(line);
        }
    }
}
