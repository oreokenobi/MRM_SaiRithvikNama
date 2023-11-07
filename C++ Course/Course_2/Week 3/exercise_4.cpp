
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
  
  string my_string = (argv[1]);
  
  //add code below this line
for (int i=0;i<=(my_string.size()/2 -1);i++)
{
  cout<<my_string.at(i);
}
cout <<endl;

for (int i=(my_string.size()/2);i<my_string.size();i++)
{
  cout<<my_string.at(i);
}
  //add code above this line
  
  return 0;
  
}