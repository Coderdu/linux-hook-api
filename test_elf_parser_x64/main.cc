/*
 * main.cc
 *
 *  Created on: Aug 12, 2010
 *      Author: fify
 */

#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "elf_parser_api_x64/utils.h"
#include "elf_parser_api_x64/headers.h"
#include "elf_parser_api_x64/utils.h"
#include "elf_parser_api_x64/elf_parser.h"

using std::cout;
using std::endl;
using std::fstream;
using std::vector;

using namespace ElfParser;

int main()
{
	Elf64_Sym sym;
	Elf64_Ehdr hdr;

	fstream fs;
	string name = string("gettimeofday");

	fs.open("libc-2.5.so", std::ios::in | std::ios::binary);

	if(getSymByName(sym, name, fs) < 0 || getElfHeader(hdr, fs) < 0)
	{
		cout << "Error!" << endl;
		return -1;
	}

	print_sym(&sym);

	char *buf;
	int offset = 0;		// offset in the file.
	int pro_count;	// Program header count.
	vector<MyElf64_Shdr> vec;

	pro_count = ElfParser::getAllSectionHeader(vec, fs);

	for(int i = 0;i<pro_count;i++)
	{
		if(sym.st_value >= vec[i].sh_addr && sym.st_value < vec[i].sh_addr + vec[i].sh_size)
		{
			offset = sym.st_value - vec[i].sh_addr + vec[i].sh_offset;
			break;
		}
	}

	//long long offset = sym.st_value;
	long long len = sym.st_size;

	buf = new char[len + 1];

	fs.seekg(offset);
	fs.read(buf, len);

	print_hex(buf, len);

	//print_sec_headers(fs);
	// print_sym_table(fs);

	fs.close();
	return 0;
}

