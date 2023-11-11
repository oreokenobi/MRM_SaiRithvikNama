#include <iostream>
#include <vector>
using namespace std;

//add function definitions below this line

int ListSum(vector<int>& list) {
  if (list.size() <= 1) {
    return list.at(0);
  }
  else {
    vector<int> list2(list.begin() + 1, list.begin() + list.size());
    return list.at(0) + ListSum(list2);
  }
}
//add function definitions above this line

int main(int argc, char** argv) {
  vector<int> nums;
  for (int i = 1; i < argc; i++) {
    nums.push_back(stoi(argv[i]));
  }
  cout << ListSum(nums) << endl;
  return 0;
}
