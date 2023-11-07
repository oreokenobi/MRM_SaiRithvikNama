#include <iostream>
using namespace std;

int main(int argc, char** argv) {
  
  int a = atoi((argv[1]));
  int b = atoi((argv[2]));
  int *p1 = &a;
  int *p2 = &b;
  
  //add code below this line
 if(*p1 > *p2){
    cout << "The larger number is " << *p1;
}
 else if (*p2 >*p1){
    cout << "The larger number is " << *p2;
  }
 else{
    cout << "Neither number is larger";
  }
  
  //add code above this line
  
  return 0;
  
}