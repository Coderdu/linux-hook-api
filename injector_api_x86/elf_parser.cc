/*
 * elf_parser.cc
 *
 *  Created on: Aug 16, 2010
 *      Author: fify
 */

#include "elf_parser.h"
#include "ptrace.h"
#include "ProcessMaps.h"

#include <link.h>
#include <elf.h>
#include <string.h>

#include <vector>
#include <fstream>

#include "utils.h"

using std::ifstream;

namespace Injector
{

ProcessMaps p_maps;

class SymInfo
{
public:
	Elf32_Addr symtab;
	Elf32_Addr strtab;
	Elf32_Addr jmprel;
	Elf32_Word totalrelsize;
	Elf32_Word relsize;
	Elf32_Word nrels;
	Elf32_Word nchains;
};

class ElfInfo
{
public:
	Elf32_Off dbgInfoFileOff;	// Debug information file offset
	Elf32_Word dgbInfoTblNum;	// Debug information table number

	Elf32_Off dbgInfoStrTbleFileOff;
	Elf32_Word dgbInfoStrTblSize;

	Elf32_Off symTblFileOff;
	Elf32_Word symTblNum;

	Elf32_Off symStrTblFileOff;
	Elf32_Word symNamStrTblSize;

	Elf32_Off hashOff;
	Elf32_Word hashTblNum;
};

class DynInfo
{
public:
	Elf32_Addr symtab;
	Elf32_Addr strtab;
	Elf32_Addr jmprel;
	Elf32_Word totalrelsize;
	Elf32_Word relsize;
	Elf32_Word nrels;
	Elf32_Word nchains;

	Elf32_Addr dyn_addr;
};

void get_dyn_info(int pid, struct link_map *lm, DynInfo &info);
void get_sym_info(int pid, struct link_map *lm, SymInfo &info);
Elf32_Addr find_symbol_in_linkmap(int pid, struct link_map *lm, char *sym_name);

#define IMAGE_ADDR 0x08048000
// TODO Get the process' image address by reading /proc/<pid>/maps.
Elf32_Addr get_image_addr(int pid)
{
	unsigned long res = 0;
	return res;
}

/*
 * Get the link_map of the given process(specified by pid).
 * Elf Header -> Program Header(PT_DYNAMIC) -> Dynamic Entry(DT_PLTGOT) ->
 *	GOT(base_addr + sizeof(Elf32_Addr))(Map Addr) -> Link Map.
 */
void get_linkmap(int pid, struct link_map &map)
{
	Elf32_Ehdr ehdr;
	Elf32_Phdr phdr;
	Elf32_Dyn dyn;
	Elf32_Addr got;	// Address of GOT.

	Elf32_Addr phdr_addr;
	Elf32_Addr dyn_addr;
	Elf32_Addr map_addr;

	int i = 0;

	// Read the Elf header of the given process, and store it into "ehdr".
	ptrace_read(pid, IMAGE_ADDR, &ehdr, sizeof(Elf32_Ehdr));

	phdr_addr = IMAGE_ADDR + ehdr.e_phoff;

	// Read the Program header, and store it into "phdr".
	ptrace_read(pid, phdr_addr, &phdr, sizeof(Elf32_Phdr));

	// Read and find the Program header with type "PT_DYNAMIC".
	while (phdr.p_type != PT_DYNAMIC)
	{
		ptrace_read(pid, phdr_addr += sizeof(Elf32_Phdr), &phdr,
			sizeof(Elf32_Phdr));
	}

	dyn_addr = phdr.p_vaddr;

	// Read a Dynamic Entry from "dyn_addr" and store it into "dyn".
	ptrace_read(pid, dyn_addr, &dyn, sizeof(Elf32_Dyn));

	// Read and find the Dynamic Entry with tag "DT_PLTGOT".
	while (dyn.d_tag != DT_PLTGOT)
	{
		ptrace_read(pid, dyn_addr + i * sizeof(Elf32_Dyn), &dyn,
				sizeof(Elf32_Dyn));
		i++;
	}

	got = dyn.d_un.d_ptr;

	// Read the link_map address from the GOT, and store it into "map_addr".
	ptrace_read(pid, got + sizeof(Elf32_Addr), &map_addr, sizeof(Elf32_Addr));

	// Read the link_map from the "map_addr" and store it into "map".
	ptrace_read(pid, map_addr, &map, sizeof(struct link_map));
}

/*
 * 取得给定link_map指向的SYMTAB、STRTAB、HASH、JMPREL、PLTRELSZ、RELAENT、RELENT信息
 * 这些地址信息将被保存到全局变量中，以方便使用
 */
void get_sym_info(int pid, struct link_map *lm, SymInfo &info)
{
	Elf32_Dyn *dyn = (Elf32_Dyn *) malloc(sizeof(Elf32_Dyn));
	unsigned long dyn_addr;

	dyn_addr = (unsigned long) lm->l_ld;

	ptrace_read(pid, dyn_addr, dyn, sizeof(Elf32_Dyn));

	while (dyn->d_tag != DT_NULL)
	{
		switch (dyn->d_tag)
		{
		case DT_SYMTAB:
			info.symtab = dyn->d_un.d_ptr;
			//puts("DT_SYMTAB");
			printf("SymInfo.symtab = 0x%08x\n", info.symtab);
			break;
		case DT_STRTAB:
			info.strtab = dyn->d_un.d_ptr;
			//puts("DT_STRTAB");
			printf("SymInfo.strtab = 0x%08x\n", info.strtab);
			break;
		case DT_HASH:
			ptrace_read(pid, dyn->d_un.d_ptr + lm->l_addr + sizeof(Elf32_Word), &info.nchains, sizeof(info.nchains));
			//puts("DT_HASH");
			break;
		case DT_JMPREL:
			info.jmprel = dyn->d_un.d_ptr;
			//puts("DT_JMPREL");
			printf("SymInfo.jmprel = 0x%08x\n", info.jmprel);
			break;
		case DT_PLTRELSZ:
			//puts("DT_PLTRELSZ");
			info.totalrelsize = dyn->d_un.d_val;
			printf("SymInfo.totalrelsize = %d\n", info.totalrelsize);
			break;
		case DT_RELAENT:
			info.relsize = dyn->d_un.d_val;
			printf("SymInfo.relsize = 0x%08x\n", info.relsize);
			//puts("DT_RELAENT");
			break;
		case DT_RELENT:
			info.relsize = dyn->d_un.d_val;
			printf("SymInfo.relsize = 0x%08x\n", info.relsize);
			//puts("DT_RELENT");
			break;
		}

		ptrace_read(pid, dyn_addr += sizeof(Elf32_Dyn), dyn, sizeof(Elf32_Dyn));
	}

	info.nrels = info.totalrelsize / info.relsize;

	free(dyn);
}

/*
 * 解析指定符号
 */
Elf32_Addr find_symbol(int pid, struct link_map *map, char *sym_name)
{
	struct link_map *lm = (struct link_map *) malloc(sizeof(struct link_map));
	unsigned long sym_addr;
	char str[128];

	//SymInfo info;
	// get_sym_info(pid, map, info);
	sym_addr = find_symbol_in_linkmap(pid, map, sym_name);
	if (sym_addr)
		return sym_addr;

	if (!map->l_next)
		return 0;

	ptrace_read(pid, (unsigned long) map->l_next, lm, sizeof(struct link_map));

	sym_addr = find_symbol_in_linkmap(pid, lm, sym_name);

	while (!sym_addr && lm->l_next)
	{
		ptrace_read(pid, (unsigned long) lm->l_next, lm, sizeof(struct link_map));

		ptrace_readstr(pid, (unsigned long) lm->l_name, str, sizeof(str));

		if (str[0] == '\0')
			continue;

		printf("Finding in file [%s]\n", str);

		if ((sym_addr = find_symbol_in_linkmap(pid, lm, sym_name)))
			break;
	}

	return sym_addr;
}

Elf32_Addr find_symbol_in_linkmap(int pid, struct link_map *lm,
		char *sym_name)
{
	Elf32_Addr ret = 0;
	Elf32_Off off;

	string lib_name; 				// The dynamic library file name.

	lib_name = p_maps.get_libfile_by_addr(lm->l_addr, pid);

	off = find_symbol_in_file(lib_name.c_str(), sym_name);
	if(off == 0)
	{
		return ret;
	}

	ret = p_maps.get_vaddr_by_off(lm->l_addr, off, pid);

	return ret;
}

Elf32_Off find_symbol_in_file(const char *file_name, char *sym_name)
{
	Elf32_Ehdr e_hdr;				// ELF Header.
	Elf32_Shdr s_hdr;				// ELF Section Header.
	Elf32_Sym sym;					// ELF Symbol Header.

	Elf32_Addr ret = 0;

	string lib_name; 				// The dynamic library file name.

	char *sec_name_str_table;		// Section Name String Table.
	char *sym_name_str_table;		// Symbol Name String Table.

	Elf32_Off symTblFileOff;		// Symbol Table File Offset.
	Elf32_Word symTblNum;			// Symbol Table entry Number.

	Elf32_Off symNameStrTblFileOff;	// Symbol Name String Table File Offset.
	Elf32_Word symNameStrTblSize;	// Symbol Name String Table Size.

	ifstream ifs;

	ifs.open(file_name);

	// Read ELF header.
	ifs.seekg(ifs.beg);
	ifs.read((char *)&e_hdr, sizeof(e_hdr));
	// ptrace_read(pid, lm->l_addr, &e_hdr, sizeof(e_hdr));

	if(e_hdr.e_type != ET_DYN && e_hdr.e_type != ET_EXEC)
	{
		return 0;
	}

	// Read the section header that points to the "Section Name String Table".
	ifs.seekg(e_hdr.e_shoff + e_hdr.e_shstrndx * sizeof(Elf32_Shdr), ifs.beg);
	ifs.read((char *)&s_hdr, sizeof(Elf32_Shdr));
	//ptrace_read(pid, lm->l_addr + e_hdr.e_shoff + e_hdr.e_shstrndx * sizeof(Elf32_Shdr), &s_hdr, sizeof(Elf32_Shdr));

	sec_name_str_table = new char[s_hdr.sh_size + 1];

	// Read in section name string table.
	ifs.seekg(s_hdr.sh_offset, ifs.beg);
	ifs.read(sec_name_str_table, s_hdr.sh_size);
	//ptrace_read(pid, lm->l_addr + s_hdr.sh_offset, sec_name_str_table, s_hdr.sh_size);

	//off = e_hdr.e_shoff;
	ifs.seekg(e_hdr.e_shoff, ifs.beg);
	for(int i = 0;i<e_hdr.e_shnum;i++)
	{
		ifs.read((char *)&s_hdr, e_hdr.e_shentsize);
		//ptrace_read(pid, lm->l_addr + e_hdr.e_shoff + e_hdr.e_shentsize * i, &s_hdr, sizeof(s_hdr));

		if(strcmp(sec_name_str_table + s_hdr.sh_name, ".dynsym") == 0)
		{
			symTblFileOff = s_hdr.sh_offset;
			symTblNum = s_hdr.sh_size / s_hdr.sh_entsize;
		}

		if(strcmp(sec_name_str_table + s_hdr.sh_name, ".dynstr") == 0)
		{
			symNameStrTblFileOff = s_hdr.sh_offset;
			symNameStrTblSize = s_hdr.sh_size;
		}
	}

	sym_name_str_table = new char[symNameStrTblSize + 1];
	ifs.seekg(symNameStrTblFileOff, ifs.beg);
	ifs.read(sym_name_str_table, symNameStrTblSize);
	//ptrace_read(pid, lm->l_addr + symNameStrTblFileOff, sym_name_str_table, symNameStrTblSize);
	ifs.seekg(symTblFileOff, ifs.beg);
	for(unsigned i = 0;i<symTblNum;i++)
	{

		ifs.read((char *)&sym, sizeof(Elf32_Sym));
		//ptrace_read(pid, lm->l_addr + symTblFileOff + sizeof(Elf32_Sym) * i, &sym, sizeof(sym));

		//printf("%s\n", sym_name_str_table + sym.st_name);
		if(strcmp(sym_name_str_table + sym.st_name, sym_name) == 0)
		{
			// offset = sym.st_value - Elf32_Shdr.sh_addr + Elf32_Shdr.sh_offset;
			ifs.seekg(e_hdr.e_shoff + sym.st_shndx * e_hdr.e_shentsize, ifs.beg);
			ifs.read((char *)&s_hdr, sizeof(Elf32_Shdr));
			ret = sym.st_value - s_hdr.sh_addr + s_hdr.sh_offset;
			break;
		}
	}

	delete sec_name_str_table;
	delete sym_name_str_table;

	ifs.close();

	return ret;
}

Elf32_Addr find_sym_in_rel_linkmap(int pid, struct link_map *lm,
		char *sym_name)
{
	Elf32_Addr ret = 0;

	Elf32_Ehdr ehdr;
	Elf32_Rel rel;
	Elf32_Sym sym;

	char str[1024];

	DynInfo info;

	ptrace_read(pid, lm->l_addr, &ehdr, sizeof(Elf32_Ehdr));
	if(ehdr.e_type != ET_DYN && ehdr.e_type != ET_EXEC)
	{
		return 0;
	}

	get_dyn_info(pid, lm, info);
	//print_hex((char *)&info, sizeof(info));

	Elf32_Word i;

	for (i = 0; i < info.nrels; i++)
	{
		ptrace_read(pid, (unsigned long) (info.jmprel + i * sizeof(Elf32_Rel)), &rel,
				sizeof(Elf32_Rel));
		if (ELF32_R_SYM(rel.r_info))
		{
			ptrace_read(pid, info.symtab + ELF32_R_SYM(rel.r_info)
					* sizeof(Elf32_Sym), &sym, sizeof(Elf32_Sym));
			ptrace_readstr(pid, info.strtab + sym.st_name, str, sizeof(str));
			if (strcmp(str, sym_name) == 0)
			{
				break;
			}
		}
	}

	if (i == info.nrels)
		ret = 0;
	else
	{
		//printf("[[0x%08x]]ret\n", lm->l_addr + rel.r_offset);
		ret = lm->l_addr + rel.r_offset;
	}

	return ret;
}

//Elf32_Addr find_sym_in_rel_linkmap(int pid, struct link_map *lm,
//		char *sym_name)
/* 查找符号的重定位地址 */
Elf32_Addr find_sym_in_rel(int pid, struct link_map *map, char *sym_name)
{
	struct link_map lm;

	Elf32_Addr sym_addr = 0;
	sym_addr = find_sym_in_rel_linkmap(pid, map, sym_name);

	if(sym_addr)
	{
		return sym_addr;
	}

	if(!map->l_next)
	{
		return 0;
	}

	ptrace_read(pid, (Elf32_Addr)map->l_next, &lm, sizeof(lm));
	sym_addr = find_sym_in_rel_linkmap(pid, &lm, sym_name);

	while(!sym_addr && lm.l_next)
	{
		ptrace_read(pid, (Elf32_Addr)lm.l_next, &lm, sizeof(lm));

		if((sym_addr = find_sym_in_rel_linkmap(pid, &lm, sym_name)))
			break;
	}

	return sym_addr;
}

/*
 在进程自身的映象中（即不包括动态共享库，无须遍历link_map链表）获得各种动态信息
 */
void get_dyn_info(int pid, struct link_map *map, DynInfo &info)
{
	const Elf32_Addr BASE_ADDR = map->l_addr;

	Elf32_Ehdr ehdr;
	Elf32_Phdr phdr;
	Elf32_Dyn *dyn = (Elf32_Dyn *) malloc(sizeof(Elf32_Dyn));

	Elf32_Addr phdr_addr;
	int i = 0;

	// Read the Elf header of the given process, and store it into "ehdr".
	ptrace_read(pid, BASE_ADDR, &ehdr, sizeof(Elf32_Ehdr));

	phdr_addr = BASE_ADDR + ehdr.e_phoff;

	// Read the Program header, and store it into "phdr".
	ptrace_read(pid, phdr_addr, &phdr, sizeof(Elf32_Phdr));

	// Read and find the Program header with type "PT_DYNAMIC".
	int ii = 1;
	while (phdr.p_type != PT_DYNAMIC && ii < ehdr.e_phnum)
	{
		ptrace_read(pid, phdr_addr += sizeof(Elf32_Phdr), &phdr,
			sizeof(Elf32_Phdr));
		++ii;
	}
	if(ii == ehdr.e_phnum)
	{
		printf("get_dyn_info error!\n");
		return;
	}

	info.dyn_addr = BASE_ADDR + phdr.p_vaddr;

	i = 0;
	ptrace_read(pid, info.dyn_addr + i * sizeof(Elf32_Dyn), dyn, sizeof(Elf32_Dyn));

	i++;
	while (dyn->d_tag)
	{
		//printf("Trace here!\n");
		switch (dyn->d_tag)
		{
		case DT_SYMTAB:
			//puts("DT_SYMTAB");
			info.symtab = dyn->d_un.d_ptr;
			//printf("DynInfo.symtab = 0x%08x\n", info.symtab);
			break;
		case DT_STRTAB:
			info.strtab = dyn->d_un.d_ptr;
			//puts("DT_STRTAB");
			//printf("DynInfo.strtab = 0x%08x\n", info.strtab);
			break;
		case DT_JMPREL:
			info.jmprel = dyn->d_un.d_ptr;
			//puts("DT_JMPREL");
			//printf("DynInfo.jmprel = 0x%08x\n", info.jmprel);
			break;
		case DT_PLTRELSZ:
			info.totalrelsize = dyn->d_un.d_val;
			//puts("DT_PLTRELSZ");
			//printf("DynInfo.totalrelsize = 0x%08x\n", info.totalrelsize);
			break;
		case DT_RELAENT:
			info.relsize = dyn->d_un.d_val;
			//puts("DT_RELAENT");
			//printf("DynInfo.relsize = %d\n", info.relsize);
			break;
		case DT_RELENT:
			info.relsize = dyn->d_un.d_val;
			//puts("DT_RELENT");
			//printf("DynInfo.relsize = %d\n", info.relsize);
			break;
		}

		ptrace_read(pid, info.dyn_addr + i * sizeof(Elf32_Dyn), dyn,
				sizeof(Elf32_Dyn));
		i++;
	}


	info.nrels = info.totalrelsize / info.relsize;

	free(dyn);
}

}
