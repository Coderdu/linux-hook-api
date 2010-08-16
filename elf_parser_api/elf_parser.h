/*
 * elf_parser.h
 *
 *  Created on: Aug 11, 2010
 *      Author: fify
 */

#ifndef ELF_PARSER_H_
#define ELF_PARSER_H_

#include <vector>
#include "headers.h"

using std::vector;
using std::string;

namespace ElfParser
{

int getAllSectionHeader(vector<MyElf64_Shdr> &vec, fstream &fs);
int getAllSym(vector<MyElf64_Sym> &vec, fstream &fs);
int getElfHeader(Elf64_Ehdr &hdr, fstream &fs);
int getSymByName(Elf64_Sym &sym, string name, fstream &fs);

}

#endif /* ELF_PARSER_H_ */
