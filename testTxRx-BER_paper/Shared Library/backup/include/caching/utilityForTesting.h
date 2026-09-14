#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>

namespace caching{

using namespace std;

vector<string> getDirectoryFiles(string &directory);

double getFileSize(string filepath);

}//end namespace caching