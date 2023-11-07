#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;

int main(int argc, char** argv) {

////////// DO NOT EDIT! //////////
  string path = argv[1];        //
//////////////////////////////////  
  
  //add code below this line
 try {
  ifstream file;
  string read;
  int lsum=0, csum=0;
  file.open(path);
  if (!file) {
    throw runtime_error("File failed to open.");
  }
  while (getline(file, read)) {
    lsum+=1;
    csum += read.length();
  }
  file.close();
  cout << lsum << " line(s)" << endl;
  cout << csum << " character(s)"; 
}
  
catch (exception& e) {
  cerr << e.what() << endl;
}

  
  //add code above this line
  
  return 0;
  
}