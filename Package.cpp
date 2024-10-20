#include <iostream>

#include "Package.h"

using namespace std;

Package::Package(string PackageOrigin, unordered_map<string,Package> &Scope)
{
  FILE *Pipe;
  char Buffer[128];

  string GetFlavorCmd = (string) "pkg info " +
                        (string) PackageOrigin +
                        (string) "| grep 'flavor         :' | cut -f 4 -w";

  string GetAutomaticStateCmd = (string) "pkg query -e '%o=" +
                        (string) PackageOrigin +
                        (string) "' %a";

  string GetDependencies;

  Origin = PackageOrigin;

  cout << "    Computing flavor of package...";
  Pipe = popen(GetFlavorCmd.c_str(),"r");
  if(!Pipe)
  {
    cerr << "Error while retrieving flavor of package " + Origin + "." << endl;
    exit(1);
  }
  Buffer[1] = 0;
  fgets(Buffer,128,Pipe);
  pclose(Pipe);
  Buffer[strlen(Buffer) - 1] = 0;
  Flavor = string(Buffer);
  cout << Flavor + "." << endl;

  cout << "    Computing automatic state of package...";
  Pipe = popen(GetAutomaticStateCmd.c_str(),"r");
  if(!Pipe)
  {
    cerr << "Error while retrieving automatic state of package " + Origin + "." << endl;
    exit(1);
  }
  fgets(Buffer,2,Pipe);
  pclose(Pipe);
  Automatic = (bool) (Buffer[0] - '0');
  if(Automatic) cout << "true." << endl;
  else cout << "false." << endl;

  cout << "    Computing dependencies..." << endl;
  GetDependencies =  (string) "pkg query -e \"\%o=" +
                     (string) Origin +
                     (string) "\" \%do";
  Pipe = popen(GetDependencies.c_str(),"r");
  if(!Pipe)
  {
    cerr << "Error while retrieving dependencies." << endl;
    exit(1);
  }
  while(fgets(Buffer,128,Pipe))
  {
    Buffer[strlen(Buffer) - 1] = 0;
    try
    {
      Dependencies.push_back(&(Scope.at(Buffer)));
      cout << "      " << Buffer << " is outdated." << endl;
    } catch(...)
    {
    };
  }
  pclose(Pipe);
}

void Package::SimulateUpdate(unsigned int RecursionDegree)
{
  for(auto PkgPtr: Dependencies) PkgPtr->SimulateUpdate(RecursionDegree + 1);
  if(Outdated)
  {
    string ToBeUpdated   =  string(RecursionDegree, ' ') +
                            (string) Origin +
                            "\n";
    cout << ToBeUpdated;
    Outdated = false;
  }
}

void Package::Update()
{
  for(auto PkgPtr: Dependencies) PkgPtr->Update();
  if(Outdated)
  {
    string UpdateCommand =  (string) "cd " +
                            (string) PORTSDIR +
                            (string) "/" +
                            (string) Origin +
                            (string) " && make build deinstall install clean";
    if(!Flavor.empty())
    {
      UpdateCommand +=  " FLAVOR=" +
                        (string) Flavor;
    }
    if(Automatic)
    {
      UpdateCommand += " INSTALLS_DEPENDS=-A";
    }
    system(UpdateCommand.c_str());
    Outdated = false;
  }
}
