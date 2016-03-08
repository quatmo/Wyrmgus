//       _________ __                 __
//      /   _____//  |_____________ _/  |______     ____  __ __  ______
//      \_____  \\   __\_  __ \__  \\   __\__  \   / ___\|  |  \/  ___/
//      /        \|  |  |  | \// __ \|  |  / __ \_/ /_/  >  |  /\___ |
//     /_______  /|__|  |__|  (____  /__| (____  /\___  /|____//____  >
//             \/                  \/          \//_____/            \/
//  ______________________                           ______________________
//                        T H E   W A R   B E G I N S
//         Stratagus - A free fantasy real time strategy game engine
//
/**@name province.cpp - The provinces. */
//
//      (c) Copyright 2016 by Andrettin
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; only version 2 of the License.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//      02111-1307, USA.
//

//@{

/*----------------------------------------------------------------------------
--  Includes
----------------------------------------------------------------------------*/

#include "stratagus.h"

#include "province.h"

#include <ctype.h>

#include <string>
#include <map>

/*----------------------------------------------------------------------------
--  Variables
----------------------------------------------------------------------------*/

std::vector<CWorld *> Worlds;
std::vector<CProvince *> Provinces;

/*----------------------------------------------------------------------------
--  Functions
----------------------------------------------------------------------------*/

void CleanWorlds()
{
	for (size_t i = 0; i < Worlds.size(); ++i) {
		for (size_t j = 0; j < Worlds[i]->Provinces.size(); ++j) {
			delete Worlds[i]->Provinces[j];
		}
		Worlds[i]->Provinces.clear();
		
		delete Worlds[i];
	}
	Worlds.clear();
}

CWorld *GetWorld(std::string world_name)
{
	for (size_t i = 0; i < Worlds.size(); ++i) {
		if (world_name == Worlds[i]->Name) {
			return Worlds[i];
		}
	}
	return NULL;
}

CProvince *GetProvince(std::string province_name)
{
	for (size_t i = 0; i < Provinces.size(); ++i) {
		if (province_name == Provinces[i]->Name) {
			return Provinces[i];
		}
	}
	return NULL;
}

//@}
