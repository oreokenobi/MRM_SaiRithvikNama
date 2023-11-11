#include <iostream>
#include <vector>
using namespace std;

string Reverse(string x) {
  string y;
  for (int i = x.length() - 1; i >= 0; i--) {
    y += x.at(i);
  }
  return y;
}
bool IsPalindrome(string x)
{
  return(Reverse(x)==x);
}

//add code below this line



//add code above this line

int main(int argc, char** argv) {
  string x = argv[1];
  cout << boolalpha << IsPalindrome(x) << endl;
}
