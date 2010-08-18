/*
 * ProcessMaps.h
 *
 *  Created on: Aug 18, 2010
 *      Author: fify
 */

#ifndef PROCESSMAPS_H_
#define PROCESSMAPS_H_

#include <string>
#include <unistd.h>
#include <vector>
#include <elf.h>

using std::string;
using std::vector;

namespace Injector
{

class ProcessMaps;
// Entries in /proc/<pid>/maps.
class MapsEntry
{
	friend class ProcessMaps;
private:
	Elf32_Addr start, end;	// Start and end address of the entry.
	string perms;			// Permissions.
	Elf32_Off offset;		// Offset in the file.
	string dev;				// Device number. <Master>:<Slave>.
	unsigned int inode;		// iNode number of the file.
	string pathname;		// Full path name of the loaded file.

public:
	bool operator <(const MapsEntry entry) const
	{
		return start < entry.start;
	}
};

class ProcessMaps
{
public:
	ProcessMaps();
	string get_libfile_by_addr(Elf32_Addr addr, pid_t pid);
	Elf32_Addr get_vaddr_by_off(Elf32_Addr start, Elf32_Off off, pid_t pid);
	virtual ~ProcessMaps();

private:
	int get_maps_entries(pid_t pid);
	void get_maps_path(pid_t pid);

private:
	vector<MapsEntry> entries;
	string path;

	pid_t p_id;
};

}

#endif /* PROCESSMAPS_H_ */
