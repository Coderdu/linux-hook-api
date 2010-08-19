/*
 * elf_parser.h
 *
 *  Created on: Aug 11, 2010
 *      Author: fify
 */

#ifndef ELF_PARSER_H_
#define ELF_PARSER_H_

#include "headers.h"

int getSymByName(Elf64_Sym &sym, string name);

#endif /* ELF_PARSER_H_ */
