/*
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) Oren Sokoler (https://github.com/orenskl)
 *
 */

#ifndef _OSFILE_PICO_HPP_
#define _OSFILE_PICO_HPP_

extern "C" {

struct OsFile;
typedef OsFile* OsFile_Handle;

const char OsFile_separator_char      = '/';
const char OsFile_path_separator_char = ':';

}

#endif /* _OSFILE_PICO_HPP_ */
