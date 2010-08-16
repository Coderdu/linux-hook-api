/*
 * utils.h
 *
 *  Created on: Aug 13, 2010
 *      Author: fify
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <fstream>
using std::fstream;

void print_sec_headers(fstream &fs);
void print_sym_table(fstream &fs);

#endif /* UTILS_H_ */
