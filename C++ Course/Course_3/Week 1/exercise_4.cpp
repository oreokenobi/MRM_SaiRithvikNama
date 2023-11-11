#include <iostream>
#include <vector>
using namespace std;

//add code below this line
bool IsPalindrome(string s)
{
  string rev="";
  for(int i=s.length()-1;i>=0;i--)
  {
    rev+=s.at(i);
  }
  return(rev==s);

}


//add code above this line

int main(int argc, char** argv) {
  string x = argv[1];
  cout << boolalpha << IsPalindrome(x) << endl;
}