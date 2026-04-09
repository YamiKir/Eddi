-----------------------------------------------------------

   September 2025 -> onwards
   Eddi, the terminal embedded text edditor
   Revision: 0.0.0a - the bare text "Eddi"tor 

-----------------------------------------------------------

   Written by Koby A. Ramsey in C++

============================================================
   
   Questions:
   What is Eddi's purpose? 
    Eddi is a tui-based editior in the nature of Vi/Vim, Emacs, Nano. Eddi should be intutitive for the average terminal user to navigate and to be eventually feature rich. 
    Eddi is NOT Ed but it can run some familiar commands. 

============================================================

How to compile: 

The current command I use to compile this program is 
g++ eddi_main.cpp -o eddi -lnotcurses -lnotcurses-core
Currently has been tested to work with the included pytxis terminal on Fedora 42+, issues with Visual Studio Code terminal emulator
