/**
 * @file   safe_system.h
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief  This is the header file for the safe_system function.
 * 
 * Copyright (C) 2014 Kieran Colford
 *
 * This file is part of Compiler.
 *
 * Compiler is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Compiler is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Compiler; see the file COPYING.  If not see
 * <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SAFE_SYSTEM_H
#define SAFE_SYSTEM_H

/** 
 * This is a routine that forks the calling process and then calls
 * exec to run another program (while the original program waits for
 * it to return).
 * 
 * @param args A NULL terminated argument vector.
 * 
 * @return The return of the program.
 */
extern int safe_system (const char *args[]);

#endif
