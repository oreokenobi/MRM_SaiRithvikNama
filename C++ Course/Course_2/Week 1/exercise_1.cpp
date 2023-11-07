#include <iostream>
using namespace std;

int main() {
  
  string oceans[] = {"Pacific", "Atlantic", "Indian", "Arctic", "Southern"};
  
  //add code below this line

  for(int i=0;i<sizeof(oceans)/sizeof(string);i++)
    cout << oceans[i]<<endl;

  //add code above this line
  
  return 0;
  
}