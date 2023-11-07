

#include <iostream>
using namespace std;

int main(/*int argc, char** argv*/) {

  //string my_string = (argv[1]);
  string my_string= "home";
  //add code below this line
  char temp;
  for (int i=0;i<my_string.size();i+=2)
  {
    temp=my_string.at(i+1);
    my_string.at(i+1)=my_string.at(i);
    my_string.at(i)=temp;

  }
  cout << my_string;



  //add code above this line

  return 0;

}
