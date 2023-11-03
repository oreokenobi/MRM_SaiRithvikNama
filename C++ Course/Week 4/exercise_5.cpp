#include <iostream>
using namespace std;

int main() {
  
  //add code below this line


for (int i = 1; i <= 5; i++) {
    for (int j = 1; j <= 5 - i; j++) {
      cout << ".";
    }
    cout << i << endl;
  }

  //add code above this line
  
  return 0;
  
}