#include <string>
#include <iostream>
#include <array>
using namespace std;
void parse(string command, array<string,3> x){
  const int count = 3;
  int i,j;
  for (i=0; i < count; i++)
    x[i].clear();
  for(j = 0,i = 2; j < count - 1; j++)
    for(;command[i] != ' ';i++)x[j] += command[i];
  for(;i < command.length();i++)x[count - 1] += command[i];
}

int main(int argc, char const *argv[])
{
	array<string,3> out;
	char inp_char[10];
	cin.getline(inp_char,sizeof(inp_char));
	string inp(inp_char);
	parse(inp, out);
	cout << inp << endl;
	for(auto x :out)
		cout << "elem "<< x << endl << endl;
	return 0;
}