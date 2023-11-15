#include <iostream>
using namespace std;

//add class definitions below this line
class Watch{
private:
string manufacturer, model,material;
int diameter,waterresistance;
public:
Watch(string manufacturer, string model,int diameter,int waterresistance, string material)
{
  this->manufacturer=manufacturer;
  this->model=model;
  this->diameter=diameter;
  this->waterresistance=waterresistance;
  this->material=material;
}
string GetManufacturer(){
      return manufacturer;
    }
    string GetModel(){
      return model;
    }
    int GetDiameter(){
      return diameter;
    }
    int GetWaterResistance(){
      return waterresistance;
    }
    string GetMaterial(){
      return material;
    }

    void SetManufacturer(string manufacturer){
      this->manufacturer=manufacturer;
    }
    void SetModel(string model){
      this->model=model;
    }
    void SetDiameter(int diameter){
      this->diameter=diameter;
    }
    void SetWaterResistance(int waterresistance){
     this->waterresistance=waterresistance;
    }
    void SetMaterial(string material){
      this->material=material;
    }
    void Summary(){
      cout <<"Manufacturer: "<<manufacturer<<endl;
      cout << "Model: "<<model<<endl;
      cout <<"Diameter: "<< diameter<<" mm"<<endl;
      cout << "Water Resistance: "<<waterresistance<<" m"<<endl;
      cout << "Material: "<<material<<endl;

    }

};


//add class definitions above this line


int main() {
  
  //DO NOT EDIT CODE BELOW THIS LINE

  Watch my_watch("Omega", "Speedmaster", 42, 50, "steel");
  cout << my_watch.GetManufacturer() << endl;
  cout << my_watch.GetModel() << endl;
  cout << my_watch.GetDiameter() << endl;
  cout << my_watch.GetWaterResistance() << endl;
  cout << my_watch.GetMaterial() << endl;
  my_watch.SetManufacturer("Rolex");
  my_watch.SetModel("Explorer");
  my_watch.SetDiameter(36);
  my_watch.SetWaterResistance(60);
  my_watch.SetMaterial("gold");
  my_watch.Summary();

  //DO NOT EDIT CODE ABOVE THIS LINE
  
  return 0;
  
}