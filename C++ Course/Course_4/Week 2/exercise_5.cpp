#include <iostream>
using namespace std;

//DO NOT EDIT code below this line

class Person {
  public:
    Person(string n, string a) {
      name = n;
      address = a;
    }
  
    string GetName() {
      return name;
    }

    void SetName(string new_name) {
      name = new_name;
    }

    string GetAddress() {
      return address;
    }

    void SetAddress(string new_address) {
      address = new_address;
    }

    string Info() {
      return name + " lives at " + address + ".\n";
    }
  
  private:
    string name;
    string address;
};

//DO NOT EDIT code above this line

//add class definitions below this line

//DO NOT EDIT/////////////////////////////////////////////////
class CardHolder : public Person {                          //
  public:                                                   //
    CardHolder(string n, string a, int an) : Person(n, a) { //
      account_number = an;                                  //
      balance = 0;                                          //
      credit_limit = 5000;                                  //
    }                                                       //
//////////////////////////////////////////////////////////////
    int GetAccno(){
      return account_number;
    }
    float GetBalance(){
      return balance;
    }
    int GetCredlim(){
      return credit_limit;
    }
    void SetAccno(int a){
      account_number=a;
    }
    void SetBalance(float b){
      balance=b;
    }
    void SetCredlim(int c){
      credit_limit=c;
    }
    void Sale(double p){
      balance+=p;
    }
    void Payment(double p){
      balance-=p;
    }
  private:
    int account_number;
    float balance;
    int credit_limit;
};
  
  
//DO NOT EDIT/////////////////////////////////////////////////////////////
class PlatinumClient : public CardHolder {                              //
  public:                                                               //
    PlatinumClient(string n, string a, int an) : CardHolder(n, a, an) { //
      cash_back = 0.02;                                                 //
      rewards = 0;                                                      //
    }                                                                   //
//////////////////////////////////////////////////////////////////////////
    float GetCashback(){
      return cash_back;
    }
    float GetRewards(){
      return rewards;
    }
    void SetCashback(float ca){
      cash_back=ca;
    }
    void SetRewards(float r){
      rewards=r;
    }
    void Sale(double p){
      rewards+=(p*cash_back);
      SetBalance(p+GetBalance());
    }


  private:
    float cash_back;
    float rewards;

};
  
  
//add class definitions above this line

int main() {
  
  //DO NOT EDIT code below this line

  PlatinumClient pc("Sarah", "101 Main Street", 123364);
  pc.CardHolder::Sale(100);
  cout << pc.GetRewards() << endl;
  pc.Sale(100);
  cout << pc.GetRewards() << endl;
  cout << pc.GetBalance() << endl;
  pc.Payment(50);
  cout << pc.GetBalance() << endl;
  cout << pc.Info() << endl;

  //DO NOT EDIT code above this line
  
  return 0;
  
}