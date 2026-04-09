#include <notcurses/notcurses.h>
#include <iostream>
#include <string> 
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <locale>

#ifndef NC_RGB
#define NC_RGB(r,g,b) (((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff))
#endif

template <typename T>
using vector = std::vector<T>;
using string = std::string;
using std::cout;
using std::cerr;


string default_filename = "eddi_unnamed"; //default filename for eddi-term


bool file_has_data(const string& filename){ //checks if a file is empty or not
    struct stat st;
    if(stat(filename.c_str(), &st) != 0) return false;
    return st.st_size > 0;
}


vector<string> read_file(const string& filename){ //turns the file into a series of vectors/lines
    std::ifstream in(filename);
    vector<string> lines;
    string line;
    while(getline(in,line)) lines.push_back(line);
    if(lines.empty()) lines.push_back("");
    return lines;
}


void save_file(const string& filename, const vector<string>& lines){ //saves 
    std::ofstream out(filename, std::ios::trunc);
    for(size_t i=0;i<lines.size();++i){
        out << lines[i];
        if(i != lines.size()-1) out << "\n";
    }
}

void draw_buffer(ncplane* ns, const vector<string>& lines,
                 unsigned cursor_y, unsigned cursor_x,
                 unsigned scroll_y, unsigned scroll_x,
                 unsigned rows, unsigned cols,
                 const string& filename, struct notcurses* nc){ //draw buffer and moves the hardware cursor

    ncplane_erase(ns);
    unsigned ed_height = rows - 1; // last line reserved for status
    uint32_t old_bg = NC_RGB(30,30,30), old_fg = NC_RGB(200,200,200);

    ncplane_set_fg_rgb(ns, old_fg);
    ncplane_set_bg_rgb(ns, old_bg);

    for(unsigned i=0;i<ed_height;i++){ //controls what is visibly displayed
        unsigned line_idx = scroll_y + i;
        if(line_idx >= lines.size()) break;
        string line = lines[line_idx];
        if(line.size() > scroll_x){
            string visible = line.substr(scroll_x, cols);
            ncplane_printf_yx(ns, i, 0, "%s", visible.c_str());
        } else {
            ncplane_printf_yx(ns, i, 0, "");
        }
    }

    //status line at the bottom of application
    ncplane_set_bg_rgb(ns, NC_RGB(50,50,150));
    ncplane_set_fg_rgb(ns, NC_RGB(200,200,200));
    ncplane_printf_yx(ns, rows-1, 0,
        "[%s] Line %u/%zu Col %u/%u (ESC=quit, Ctrl+S=save)",
        filename.c_str(),
        cursor_y+1, lines.size(),
        cursor_x+1, scroll_x+1);

    ncplane_set_bg_rgb(ns, old_bg);
    ncplane_set_fg_rgb(ns, old_fg);

    //cursor logic
    int screen_y = cursor_y - scroll_y;
    int screen_x = cursor_x - scroll_x;
    if(screen_y >= 0 && screen_y < (int)ed_height && screen_x >= 0 && screen_x < (int)cols){
        notcurses_cursor_enable(nc, screen_y, screen_x);
    }

    notcurses_render(nc);
}

int main(int argc, char* argv[]){
    string filename = argc > 1 ? argv[1] : default_filename;
    vector<string> lines;

    if(file_has_data(filename)){
        cout << filename << " already exists, loading...\n";
        lines = read_file(filename);
    } else {
        cout << filename << " is new or empty, starting fresh...\n";
        std::ofstream initfile(filename);
        initfile.close();
        lines.push_back("");
    }

    setlocale(LC_ALL,"");
    notcurses_options opts{};
    struct notcurses* nc = notcurses_init(&opts, nullptr);
    if(!nc){ cerr << "Failed to initialize Notcurses\n"; return 1; }

    unsigned rows, cols;
    ncplane* stdplane = notcurses_stddim_yx(nc, &rows, &cols);

    notcurses_cursor_enable(nc,0,0);
    unsigned cursor_y=0, cursor_x=0;
    unsigned scroll_y=0, scroll_x=0;

    draw_buffer(stdplane, lines, cursor_y, cursor_x, scroll_y, scroll_x, rows, cols, filename, nc);

    ncinput ni;
    while(notcurses_get_blocking(nc,&ni) != NCKEY_ESC){

		//control handling
        if(ni.id == NCKEY_BACKSPACE || ni.id == 127){
            if(cursor_x>0){ lines[cursor_y].erase(cursor_x-1,1); cursor_x--; }
            else if(cursor_y>0){
                cursor_x = lines[cursor_y-1].size();
                lines[cursor_y-1] += lines[cursor_y];
                lines.erase(lines.begin()+cursor_y);
                cursor_y--;
            }
        } else if(ni.id == NCKEY_ENTER || ni.id=='\n'){
            string rest = lines[cursor_y].substr(cursor_x);
            lines[cursor_y] = lines[cursor_y].substr(0,cursor_x);
            lines.insert(lines.begin()+cursor_y+1,rest);
            cursor_y++;
            cursor_x=0;
        } else if(ni.id == NCKEY_LEFT){
            if(cursor_x>0) cursor_x--;
            else if(cursor_y>0){ cursor_y--; cursor_x = lines[cursor_y].size(); }
        } else if(ni.id == NCKEY_RIGHT){
            if(cursor_x<lines[cursor_y].size()) cursor_x++;
            else if(cursor_y<lines.size()-1){ cursor_y++; cursor_x=0; }
        } else if(ni.id == NCKEY_UP){
            if(cursor_y>0) cursor_y--;
            cursor_x = std::min(cursor_x,(unsigned)lines[cursor_y].size());
        } else if(ni.id == NCKEY_DOWN){
            if(cursor_y<lines.size()-1) cursor_y++;
            cursor_x = std::min(cursor_x,(unsigned)lines[cursor_y].size());
        } else if(ni.id >= 32 && ni.id < 127){
            lines[cursor_y].insert(cursor_x,1,(char)ni.id);
            cursor_x++;
        } else if(ni.id == 's'-'a'+1){ // Ctrl+S
            save_file(filename, lines);
        }

        //vertical scrolling
        unsigned ed_height = rows-1;
        if(cursor_y<scroll_y) scroll_y = cursor_y;
        else if(cursor_y >= scroll_y + ed_height) scroll_y = cursor_y - ed_height +1;

        //horiz scrolling
        if(cursor_x < scroll_x) scroll_x = cursor_x;
        else if(cursor_x >= scroll_x + cols) scroll_x = cursor_x - cols +1;

        draw_buffer(stdplane, lines, cursor_y, cursor_x, scroll_y, scroll_x, rows, cols, filename, nc);
    }

    save_file(filename, lines);
    notcurses_stop(nc);
    return 0;
}
