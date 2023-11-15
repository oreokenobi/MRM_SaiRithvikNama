#include <iostream>
using namespace std;

//add class definitions below this line
class Fruit{
  private:
  string name;
  string color;
  public:
  Fruit(string name, string color)
  {
    this->name=name;
    this->color=color;
  }
  string GetColor(){
    return color;}
  string GetName(){
    return name;}
  void SetColor(string color){
    this->color=color;}
  void SetName(string name){
    this->name=name;}
};


//add class definitions above this line


int main() {
  
  //DO NOT EDIT CODE BELOW THIS LINE

  Fruit fruit("apple", "red");
  cout << fruit.GetName() << endl;
  cout << fruit.GetColor() << endl;
  fruit.SetName("orange");
  fruit.SetColor("orange");
  cout << fruit.GetName() << endl;
  cout << fruit.GetColor() << endl;

  //DO NOT EDIT CODE ABOVE THIS LINE
  
  return 0;
  
}