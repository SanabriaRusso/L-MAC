// constructing bitsets
#include <iostream>       // std::cout
#include <string>         // std::string
#include <bitset>         // std::bitset

using namespace std;
int main ()
{
  std::bitset<16> foo;
  std::bitset<4> bar (15);
  std::bitset<16> baz (std::string("0101111001"));

  std::cout << "foo: " << foo << '\n';
  std::cout << "bar: " << bar << '\n';
  std::cout << "baz: " << baz << '\n';

  for(int i = 0; i < bar.size(); i++)
  {
  	cout << bar[i] << endl;
  }

  cout << "tal " << bar[3] << endl;


  return 0;
}