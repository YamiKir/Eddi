
#include <iostream>
#include <notcurses/notcurses.h>
#include <iomanip>
#include <fstream>
#include <notcurses/notcurses.h>

int MAX_HEIGHT = 10;
int MAX_WIDTH = 30;

std::size_t file_size(std::fstream& fi) { //returns the number of bytes this file is 
	
	if(!fi.is_open()){
		return 0;}	

	std::streampos prev_pos, end;
	prev_pos = fi.tellg();
	
	fi.seekg(0, std::ios::end);
	
	end = fi.tellg();
	
	std::size_t size = static_cast<std::size_t> (end);
	fi.seekg(prev_pos);

	return size;




}


std::size_t openFile(const char* path) {
    std::fstream newFile(path,
        std::ios::in | std::ios::out | std::ios::binary);

    if (!newFile.is_open()) {
        // Try creating it
        newFile.open(path,
            std::ios::out | std::ios::binary);
        newFile.close();

        // Reopen for reading
        newFile.open(path,
            std::ios::in | std::ios::binary);
    }

    return file_size(newFile);
}



 char* validFile( char* path){

	





	return path;

}

bool initializeNotCurses(const char* path,int height, int width){
	notcurses_options opts{};
	struct notcurses *nc = notcurses_init(&opts, nullptr);
	if(!nc) return false;
	
	ncplane_options popts{};
	popts.x = 0; 
	popts.y = 0; 
	popts.rows = height;
	popts.cols = width;
	popts.name = path;
	
	ncplane* plane = ncplane_create(notcurses_stdplane(nc), &popts);
	if(!plane){
		notcurses_stop(nc);
		return false;
		}

	ncplane_putstr_yx(plane,0,0, "Hello,NotCurses!");
	
	notcurses_render(nc);
	
	ncinput ni;
	char32_t ch = notcurses_get(nc,nullptr, &ni);
	
	 int x =0;
	while(x<1000000) {x+=2; x--; x=x?x:x;}
	
	//ncplane_destroy(plane);
	//notcurses_stop(nc);		


	return true; //to-do: this needs to initalize notcurses, config some settings, and make the planes.
}

void handleControls(){ //reads the keyboard for things like mode swapping, exiting, key inputs, etc.



}


//loadfile -> file ->  notcurses 
//savefile -> notcureses/buffer -> file 


int main(int argc, char* argv[]){
	
	char str_arr[] = "unnamed_eddi.txt";
	char* filename=  str_arr; // = "unnamed_eddi.txt";
	if(argc > 1) {
		filename = validFile(argv[1]);
	}
	

	std::cout<<filename<<"\n";
	
	//if(file_isempty(filename)){
		std::cout<<openFile(filename)<<"\n";
		
	//	}
	initializeNotCurses(filename, MAX_HEIGHT, MAX_WIDTH);
	



















	return 0;
}
