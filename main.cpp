#include <iostream>
#include <unordered_map>

#include "Package.h"

using namespace std;

string PORTSDIR;

int main()
{
  char Buffer[128];
  FILE *Pipe;
  unordered_map<string,Package> Outdated;

  cout  << "=== Initializing variables..."
        << endl
        << "  PORTSDIR...";
  Pipe = popen("make -V PORTSDIR","r");
  if(!Pipe)
  {
    cerr << "Error while retrieving PORTSDIR.";
    exit(1);
  }
  fgets(Buffer,128,Pipe);
  Buffer[strlen(Buffer) - 1] = '/'; // replace \n with /
                                    // having / twice is fine
  pclose(Pipe);
  PORTSDIR = Buffer;
  cout << PORTSDIR << "." << endl;

  cout << "=== Retrieving list of outdated ports..." << endl;
  Pipe = popen("pkg version -oL '=' | cut -f 1 -d ' '","r");
  if(!Pipe)
  {
    cerr << "Error while retrieving the list of outdated port.";
    exit(1);
  }

  {
    while(fgets(Buffer,128,Pipe))
    {
      Buffer[strlen(Buffer) - 1] = 0; // remove \n
      string PackageOrigin = Buffer;
      cout << "=== Retrieving data for outdated port " + PackageOrigin + "..." << endl;
      Outdated.emplace(PackageOrigin,Package(PackageOrigin,Outdated));
    }
  }

  pclose(Pipe);

  for(auto Iterator: Outdated) (Iterator.second).Update();
  return 0;
}
