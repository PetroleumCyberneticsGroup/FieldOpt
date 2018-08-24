/***********************************************************
 Copyright (C) 2017
 Mathias C. Bellout <mathias.bellout@ntnu.no>

 Created by bellout on 20170730.

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

#ifndef FIELDOPT_COLORS_H
#define FIELDOPT_COLORS_H

// FONT
#define FRED      "\x1b[31m"
#define FGREEN    "\x1b[32m"
#define FYELLOW   "\x1b[33m"
#define FBLUE     "\x1b[34m"
#define FMAGENTA  "\x1b[35m"
#define FCYAN     "\x1b[36m"
#define FLGRAY     "\x1b[90m"	
#define FLRED      "\x1b[91m"	
#define FLGREEN    "\x1b[92m"	
#define FLYELLOW   "\x1b[93m"	
#define FLBLUE     "\x1b[94m"	
#define FLMAGENTA  "\x1b[95m"	
#define FLCYAN     "\x1b[96m"	

#define END        "\x1b[0m"

// BACKGROUND
#define BRED       "\x1b[41m"	
#define BGREEN     "\x1b[42m"	
#define BYELLOW    "\x1b[43m"	
#define BBLUE      "\x1b[44m"	
#define BMAGENTA   "\x1b[45m"	
#define BCYAN      "\x1b[46m"	
#define BLGRAY     "\x1b[100m"	
#define BLRED      "\x1b[101m"	
#define BLGREEN    "\x1b[102m"	
#define BLYELLOW   "\x1b[103m"	
#define BLBLUE     "\x1b[104m"	
#define BLMAGENTA  "\x1b[105m"	
#define BLCYAN     "\x1b[106m"	
#define BLWHITE    "\x1b[107m"	

// TEXT ATTRIBUTES
#define BLDON      "\x1b[1m"	// Bold on (enable foreground intensity)
#define BLDOFF     "\x1b[21m"	// Bold off(disable foreground intensity)
#define ULON       "\x1b[4m"	// Underline on"
#define ULOFF      "\x1b[24m"	// Underline off"
#define BLKON      "\x1b[5m"    // Blink on(enable background intensity)
#define BLKOFF     "\x1b[25m"	// Blink off(disable background intensity)

#define AEND       "\x1b[0m" // All attributes off(color at startup)

#endif //FIELDOPT_COLORS_H














