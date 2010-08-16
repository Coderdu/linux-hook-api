/*
 * elf_parser.cc
 *
 *  Created on: Aug 11, 2010
 *      Author: fify
 */

#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include <algorithm>

#include "elf_parser.h"

using std::string;
using std::fstream;
using std::cout;
using std::endl;

namespace ElfParser
{

int getAllSectionHeader(vector<MyElf64_Shdr> &vec, fstream &fs)
{
	int old_offset = fs.tellg();

	vec.clear();

	Elf64_Ehdr e_hdr;
	MyElf64_Shdr s_hdr;

	MyElf64_Sym sym;

	char *sec_name_str_table;

	fs.seekg(fs.beg);

	// ------------ Part I: ELF header ------------ //
	// Read program file header.
	fs.read((char *) &e_hdr, sizeof(Elf64_Ehdr));

	// printf("%lf\n", -1.0 * (e_hdr.e_phoff - e_hdr.e_shoff) / sizeof(Elf64_Phdr));

	// ------------ Part II: Program header ------------ //

	// ------------ Part III: Section header table ------------ //
	fs.seekg(e_hdr.e_shoff + e_hdr.e_shstrndx * sizeof(Elf64_Shdr),
			std::ios::beg);
	// Read information about "Section Name String Table"
	fs.read((char *) &s_hdr, e_hdr.e_shentsize);
	fs.seekg(s_hdr.sh_offset, std::ios::beg);

	// Read in "Section Name String Table".
	sec_name_str_table = new char[s_hdr.sh_size + 1];
	fs.read(sec_name_str_table, s_hdr.sh_size);

	fs.seekg(e_hdr.e_shoff, std::ios::beg);

	int count = 0;
	for (int i = 0; i < e_hdr.e_shnum; i++)
	{
		fs.read((char *) &s_hdr, e_hdr.e_shentsize);

		s_hdr.name = string(sec_name_str_table + s_hdr.sh_name);

		vec.push_back(s_hdr);
		count ++;
	}

	fs.seekg(old_offset);

	sort(vec.begin(), vec.end());

	return count;
}

int getAllSym(vector<MyElf64_Sym> &vec, fstream &fs)
{
	int old_offset = fs.tellg();

	vec.clear();

	Elf64_Ehdr e_hdr;
	Elf64_Shdr s_hdr;
	MyElf64_Sym sym;

	Elf64_Off SymTblFileOffset;
	int SymTblNum;

	Elf64_Off SymNamStrTblFileOffset;
	Elf64_Xword SymNamStrTblSize;

	char *SymNamStrTable;

	char *sec_name_str_table;

	fs.seekg(fs.beg);

	// ------------ Part I: ELF header ------------ //
	// Read program file header.
	fs.read((char *) &e_hdr, sizeof(Elf64_Ehdr));

	// printf("%lf\n", -1.0 * (e_hdr.e_phoff - e_hdr.e_shoff) / sizeof(Elf64_Phdr));

	// ------------ Part II: Program header ------------ //

	// ------------ Part III: Section header table ------------ //
	fs.seekg(e_hdr.e_shoff + e_hdr.e_shstrndx * sizeof(Elf64_Shdr),
			std::ios::beg);
	// Read information about "Section Name String Table"
	fs.read((char *) &s_hdr, e_hdr.e_shentsize);
	fs.seekg(s_hdr.sh_offset, std::ios::beg);

	// Read in "Section Name String Table".
	sec_name_str_table = new char[s_hdr.sh_size + 1];
	fs.read(sec_name_str_table, s_hdr.sh_size);

	fs.seekg(e_hdr.e_shoff, std::ios::beg);
	for (int i = 0; i < e_hdr.e_shnum; i++)
	{
		fs.read((char *) &s_hdr, e_hdr.e_shentsize);

		if (strcmp(sec_name_str_table + s_hdr.sh_name, ".dynsym") == 0)
		{
			SymTblFileOffset = s_hdr.sh_offset;
			SymTblNum = (s_hdr.sh_size) / (s_hdr.sh_entsize);
		}
		if (strcmp(sec_name_str_table + s_hdr.sh_name, ".dynstr") == 0)
		{
			SymNamStrTblFileOffset = s_hdr.sh_offset;
			SymNamStrTblSize = s_hdr.sh_size;
		}
	}

	fs.seekg(SymNamStrTblFileOffset, std::ios::beg);
	SymNamStrTable = new char[SymNamStrTblSize + 1];
	fs.read(SymNamStrTable, SymNamStrTblSize);

	fs.seekg(SymTblFileOffset, std::ios::beg);

	// If the given API was not found, then return -1 or else return 0.
	int ret = 0;
	for (int i = 0; i < SymTblNum; i++)
	{
		fs.read((char *) &sym, sizeof(Elf64_Sym));

		sym.name = string(SymNamStrTable + sym.st_name);

		vec.push_back(sym);

		ret ++;
	}

	delete sec_name_str_table;

	fs.seekg(old_offset, fs.cur);

	sort(vec.begin(), vec.end());

	return ret;
}

int getElfHeader(Elf64_Ehdr &hdr, fstream &fs)
{
	int old_addr = fs.tellg();
	fs.seekg(fs.beg);

	fs.read((char *) &hdr, sizeof(Elf64_Ehdr));

	fs.seekg(old_addr, fs.cur);

	if(hdr.e_ident[1] == 'E' && hdr.e_ident[2] == 'L' && hdr.e_ident[3] == 'F')
	{
		return 0;
	}
	else
	{
		cout << "Illegal ELF file!" << endl;
		return -1;
	}
}

/*
 * sym: output Elf64_Sym
 * name: API name
 * fs: Elf file
 */
int getSymByName(Elf64_Sym &sym, string name, fstream &fs)
{
	int old_offset = fs.tellg();

	Elf64_Ehdr e_hdr;
	Elf64_Shdr s_hdr;

	Elf64_Off SymTblFileOffset;
	int SymTblNum;

	Elf64_Off SymNamStrTblFileOffset;
	Elf64_Xword SymNamStrTblSize;

	char *SymNamStrTable;

	char *sec_name_str_table;

	fs.seekg(fs.beg);

	// ------------ Part I: ELF header ------------ //
	// Read program file header.
	fs.read((char *) &e_hdr, sizeof(Elf64_Ehdr));

	// printf("%lf\n", -1.0 * (e_hdr.e_phoff - e_hdr.e_shoff) / sizeof(Elf64_Phdr));

	// ------------ Part II: Program header ------------ //

	// ------------ Part III: Section header table ------------ //
	fs.seekg(e_hdr.e_shoff + e_hdr.e_shstrndx * sizeof(Elf64_Shdr),
			std::ios::beg);
	// Read information about "Section Name String Table"
	fs.read((char *) &s_hdr, e_hdr.e_shentsize);
	fs.seekg(s_hdr.sh_offset, std::ios::beg);

	// Read in "Section Name String Table".
	sec_name_str_table = new char[s_hdr.sh_size + 1];
	fs.read(sec_name_str_table, s_hdr.sh_size);

	fs.seekg(e_hdr.e_shoff, std::ios::beg);
	for (int i = 0; i < e_hdr.e_shnum; i++)
	{
		fs.read((char *) &s_hdr, e_hdr.e_shentsize);

		if (strcmp(sec_name_str_table + s_hdr.sh_name, ".dynsym") == 0)
		{
			SymTblFileOffset = s_hdr.sh_offset;
			SymTblNum = (s_hdr.sh_size) / (s_hdr.sh_entsize);
		}
		if (strcmp(sec_name_str_table + s_hdr.sh_name, ".dynstr") == 0)
		{
			SymNamStrTblFileOffset = s_hdr.sh_offset;
			SymNamStrTblSize = s_hdr.sh_size;
		}
	}

	fs.seekg(SymNamStrTblFileOffset, std::ios::beg);
	SymNamStrTable = new char[SymNamStrTblSize + 1];
	fs.read(SymNamStrTable, SymNamStrTblSize);

	fs.seekg(SymTblFileOffset, std::ios::beg);

	// If the given API was not found, then return -1 or else return 0.
	int found = -1;
	for (int i = 0; i < SymTblNum; i++)
	{
		fs.read((char *) &sym, sizeof(Elf64_Sym));

		if(strcmp(SymNamStrTable + sym.st_name, name.c_str()) == 0)
		{
			found = 0;
			break;
		}
	}

	delete sec_name_str_table;

	fs.seekg(old_offset, fs.cur);

	return found;
}

} // End of namespace ElfParser
