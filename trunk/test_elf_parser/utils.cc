/*
 * utils.cc
 *
 *  Created on: Aug 13, 2010
 *      Author: fify
 */

#include <iostream>
#include <fstream>
#include <vector>

#include "utils.h"

#include "elf_parser_api/headers.h"
#include "elf_parser_api/elf_parser.h"
#include "elf_parser_api/utils.h"

using std::fstream;
using std::cout;
using std::endl;
using std::vector;

using namespace ElfParser;

void print_sym_table(fstream &fs)
{
	vector<MyElf64_Sym> vec;

	int count = ElfParser::getAllSym(vec, fs);

	for(int i = 0;i<count;i++)
	{
		cout << vec[i].name << endl;
		//cout << vec[i].st_value << endl;
		printf("0x%llx\n", vec[i].st_value);
	}
}

void print_sec_headers(fstream &fs)
{
	vector<MyElf64_Shdr> vec;

	int count = ElfParser::getAllSectionHeader(vec, fs);

	for(int i = 0;i<count;++i)
	{
		cout << "Section name: " << vec[i].name << endl;

		printf("Section virtual address: 0x%llx\n", vec[i].sh_addr);
		printf("Section file offset: 0x%lld\n", vec[i].sh_offset);
	}
}
