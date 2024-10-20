#include <iostream>
#include <unistd.h>
#include <unordered_map>

#include "Package.h"

using namespace std;

string PORTSDIR;

void Usage()
{
    cout  << "Usage: poesis [-h] [-n]" << endl
          << "-h      Print this help message" << endl
          << "-n      Dry-run: only print what needs to be updated" << endl;
}

int main(int argc, char *argv[])
{
  bool DryRun = false;
  char Buffer[128];
  FILE *Pipe;
  unordered_map<string,Package> Outdated;

  {
    int Option;
    while((Option = getopt(argc, argv, "hn")) != -1)
    {
      switch(Option)
      {
        case 'h':
          Usage();
          return 0;
        case 'n':
          DryRun = true;
          break;
      }
    }
  }

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
      if(DryRun) cout << "=== Outdated ports:" << endl;
      while(fgets(Buffer,128,Pipe))
      {
        Buffer[strlen(Buffer) - 1] = 0; // remove \n
        string PackageOrigin = Buffer;
        cout << "=== Retrieving data for outdated port " + PackageOrigin + "..." << endl;
        Outdated.emplace(PackageOrigin,Package(PackageOrigin,Outdated));
      }
    }
    pclose(Pipe);
    for(auto Iterator: Outdated)
    {
      if(DryRun) (Iterator.second).SimulateUpdate();
      else (Iterator.second).Update();
    }

  return 0;
}
