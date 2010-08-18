/*
 * utils.h
 *
 *  Created on: Aug 17, 2010
 *      Author: fify
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <elf.h>

namespace Injector
{

void print_elf_header(Elf32_Ehdr *e_hdr_ptr, FILE *file = NULL);
void print_program_header(Elf32_Phdr *p_hdr_ptr, FILE *file = NULL);
void print_section_header(Elf32_Shdr *s_hdr_ptr, char * sec_name_str_table, Elf32_Xword size, FILE *file = NULL);

void print_sym(Elf32_Sym *sym_ptr, char * SymNamStrTable = NULL, FILE *file = NULL);

void print_hex(char *data, long long len);


}// End of namespace.

#endif /* UTILS_H_ */
