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
  vector<string> data;

try {
  ifstream file;
  string read;
  file.open(path);
  if (!file) {
    throw runtime_error("File failed to open.");
  }
  while (getline(file, read)) {
    stringstream ss(read);
    while (getline(ss, read, ',')) {
      data.push_back(read);
    }
  }
  file.close();
}
  
catch (exception& e) {
  cerr << e.what() << endl;
}
  
int c1=0,c2=0,c3=0,c4=0;
for (int i = 0; i < data.size(); i++) {
  if (i == 0 || i == 4 | i == 8) {
    c1 += stoi(data.at(i));
  }
  else if (i == 1 || i == 5 | i == 9) {
    c2 += stoi(data.at(i));
  }
  else if (i == 2 || i == 6 | i == 10) {
    c3 += stoi(data.at(i));
  }
  else if (i == 3 || i == 7 | i == 11) {
    c4 += stoi(data.at(i));
  }
}
  
cout <<c1/3<<" "<<c2/3<<" "<<c3/3<<" "<<c4/3;



  //add code above this line
  
  return 0;
  
}