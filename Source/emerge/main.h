/*!
  @file main.h
  @brief header for emerge
  @author The Emerge Desktop Development Team

  @attention This file is part of Emerge Desktop.
  @attention Copyright (C) 2004-2013  The Emerge Desktop Development Team

  @attention Emerge Desktop is free software; you can redistribute it and/or
  modify  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  @attention Emerge Desktop is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  @attention You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  */

#ifndef __GUARD_412f5e09_af1f_4ab0_9fc9_3d8d31022bae
#define __GUARD_412f5e09_af1f_4ab0_9fc9_3d8d31022bae

#define UNICODE 1

#define _NO_W32_PSEUDO_MODIFIERS

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

#include <stdio.h>
#include "../emergeLib/emergeFileRegistryLib.h"
#include "../emergeLib/emergeUtilityLib.h"

#endif
