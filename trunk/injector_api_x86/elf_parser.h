/*
 * elc_parser.h
 *
 *  Created on: Aug 16, 2010
 *      Author: fify
 */

#ifndef ELC_PARSER_H_
#define ELC_PARSER_H_

#include <elf.h>
#include <link.h>

namespace Injector
{


void get_linkmap(int pid, struct link_map &map);

/*
 取得给定link_map指向的SYMTAB、STRTAB、HASH、JMPREL、PLTRELSZ、RELAENT、RELENT信息
 这些地址信息将被保存到全局变量中，以方便使用
 */
//void get_sym_info(int pid, struct link_map *lm, SymInfo &info);

/*
 解析指定符号
 */
Elf32_Addr find_symbol(int pid, struct link_map *map, char *sym_name);
Elf32_Off find_symbol_in_file(const char *file_name, char *sym_name);

/* 查找符号的重定位地址 */
unsigned long find_sym_in_rel(int pid, char *sym_name);

/*
 在进程自身的映象中（即不包括动态共享库，无须遍历link_map链表）获得各种动态信息
 */
//void get_dyn_info(int pid, Elf32_Addr dyn_addr, DynInfo &info);

}

#endif /* ELC_PARSER_H_ */
