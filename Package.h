#include <list>
#include <string>
#include <unordered_map>

#ifndef PACKAGE_H
#define PACKAGE_H

using namespace std;

extern string PORTSDIR;

class Package
{
		string Origin;
		string Flavor;
		list<Package *> Dependencies;
		bool Automatic = true;
		bool Outdated = true;
	public:
		Package(string PackageOrigin, unordered_map<string,Package> &Scope);
		void SimulateUpdate(unsigned int RecursionDegree = 0);
		void Update();
};

#endif
