#include <iostream>
#include "diskspace.h"

int main(int, char **)
{
	DiskSpace d(boost::filesystem::current_path().string().c_str());

	std::cout << "free disk space of " << d.path() << " is " << d.freeMB() << " MBytes" << std::endl;
	std::cout << "available disk space of " << d.path() << " is " << d.availableMB() << " MBytes" << std::endl;
	std::cout << "available disk space of " << d.path() << " is " << d.availableGB() << " GBytes" << std::endl;

	d.setPath("/boot");
	std::cout << "free disk space of " << d.path() << " is " << d.freeMB() << " MBytes" << std::endl;
	std::cout << "available disk space of " << d.path() << " is " << d.availableMB() << " MBytes" << std::endl;
	std::cout << "available disk space of " << d.path() << " is " << d.availableGB() << " GBytes" << std::endl;

	d.setPath("/");
	std::cout << "free disk space of " << d.path() << " is " << d.freeMB() << " MBytes" << std::endl;
	std::cout << "available disk space of " << d.path() << " is " << d.availableMB() << " MBytes" << std::endl;
	std::cout << "available disk space of " << d.path() << " is " << d.availableGB() << " GBytes" << std::endl;

	return 0;
}
