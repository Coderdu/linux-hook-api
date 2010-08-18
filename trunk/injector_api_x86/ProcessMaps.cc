/*
 * ProcessMaps.cc
 *
 *  Created on: Aug 18, 2010
 *      Author: fify
 */

#include "ProcessMaps.h"

#include <sstream>
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include <algorithm>

using std::ostringstream;
using std::ifstream;
using std::sort;

namespace Injector
{

ProcessMaps::ProcessMaps()
{
	// TODO Auto-generated constructor stub
	p_id = 0;
	entries.clear();
}

ProcessMaps::~ProcessMaps()
{
	// TODO Auto-generated destructor stub
	entries.clear();
}

string ProcessMaps::get_libfile_by_addr(Elf32_Addr addr, pid_t pid)
{
	if(p_id != pid)
	{
		get_maps_entries(pid);
	}

	vector<MapsEntry>::iterator it;

	for(it = entries.begin(); it != entries.end(); ++it)
	{
		if(addr >= it->start && addr < it->end)
		{
			return it->pathname;
		}
	}

	return string("");
}

Elf32_Addr ProcessMaps::get_vaddr_by_off(Elf32_Addr start, Elf32_Off off, pid_t pid)
{
	if(p_id != pid)
	{
		get_maps_entries(pid);
	}

	vector<MapsEntry>::iterator it;
	unsigned iNode = 0;

	for(it = entries.begin(); it!= entries.end(); ++ it)
	{
		if(start >= it->start && start < it->end)
		{
			iNode = it->inode;
			break;
		}
	}

	for(;it != entries.end(); ++it)
	{
		if(it->inode != iNode)
		{
			break;
		}

		if(off >= it->offset && off < it->offset + it->end - it->start)
		{
			return it->start + off - it->offset;
		}
	}

	return 0;
}

int ProcessMaps::get_maps_entries(pid_t pid)
{
	char buf[256];
	char perms[256];
	char dev[256];
	char pathname[256];

	MapsEntry entry;

	int count = 0;

	// If the Maps entries are already in the memory..
	if(p_id == pid)
	{
		return entries.size();
	}
	else
	{
		p_id = pid;
	}

	entries.clear();

	get_maps_path(pid);

	ifstream ifs;
	ifs.open(path.c_str());

	while(ifs.getline(buf, sizeof(buf)))
	{
		sscanf(buf, "%x-%x %s %x %s %d %s", &entry.start,
				&entry.end, perms, &entry.offset, dev, &entry.inode,
				pathname);
		entry.perms = string(perms);
		entry.dev = string(dev);
		entry.pathname = string(pathname);

		if(entry.inode != 0)
		{
			entries.push_back(entry);
			count ++;
		}
	}

	sort(entries.begin(), entries.end());

	return count;
}

void ProcessMaps::get_maps_path(pid_t pid)
{
	ostringstream oss;

	oss.clear();

	oss << "/proc/" << pid << "/maps";

	path = oss.str();
}

}
