#include "common.h"
string file_name;
REAL scale_level;
bool is_first_time = true;
bool data_loaded = false;
int main(int argc, char** argv) {
	scale_level = 3.0;
	file_name = FILE_PATH + "/" + DEFAULT_FILE;
	run();
	return 0;
}