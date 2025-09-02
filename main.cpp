#ifndef STD 
using namespace std;
#include <iostream>
#define STD
#endif
#ifndef STRINGS
#include <string>
#define STRINGS
#endif
#ifndef FILEHANDLE
#include <fstream>
#define FILEHANDLE
#endif 
#ifndef NOTCURSES
#include "notcurses.h"
#define NOTCURSES
#endif


string eddi_open = "eddi_open";

int main(int argc, char* argv[]){
    ofstream thisFile_(argc > 1 ? argv[1]: eddi_open); 
    if (argc > 1 ){

        //search to see if this filename is valid and free filepath.
        //if yes, "touch" this filename
        
        cout<<argv[1]<< " was the filename supplied by the command\n";
        thisFile_ <<argv[1]<< " was the filename supplied by the command\n";

    }

    else {

        cout<<eddi_open<< " is the temporary filename chosen by eddi\n";

        thisFile_ <<eddi_open<< " is the temporary filename chosen by eddi\n";


    }
    thisFile_.close();




    return 0;

}


// When this program is executed ./eddi  it can take in a series of arugements in this order  ./eddi [filename, lines_flag, language_flag]
