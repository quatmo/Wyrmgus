//       _________ __                 __
//      /   _____//  |_____________ _/  |______     ____  __ __  ______
//      \_____  \\   __\_  __ \__  \\   __\__  \   / ___\|  |  \/  ___/
//      /        \|  |  |  | \// __ \|  |  / __ \_/ /_/  >  |  /\___ |
//     /_______  /|__|  |__|  (____  /__| (____  /\___  /|____//____  >
//             \/                  \/          \//_____/            \/
//  ______________________                           ______________________
//                        T H E   W A R   B E G I N S
//           Stratagus - A free fantasy real time strategy game engine
//
/**@name action_patrol.cpp - The patrol action. */
//
//      (c) Copyright 1998-2018 by Lutz Sammer, Jimmy Salmon and Andrettin
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

#include "action/action_patrol.h"

#include "animation.h"
//Wyrmgus start
#include "commands.h"
//Wyrmgus end
#include "iolib.h"
#include "map.h"
#include "pathfinder.h"
#include "script.h"
//Wyrmgus start
#include "tileset.h"
//Wyrmgus end
#include "ui.h"
#include "unit.h"
//Wyrmgus start
#include "unit_find.h"
//Wyrmgus end
#include "unittype.h"
#include "video.h"

/*----------------------------------------------------------------------------
--  Functions
----------------------------------------------------------------------------*/

//Wyrmgus start
///* static */ COrder *COrder::NewActionPatrol(const Vec2i &currentPos, const Vec2i &dest)
/* static */ COrder *COrder::NewActionPatrol(const Vec2i &currentPos, const Vec2i &dest, int current_z, int dest_z)
//Wyrmgus end
{
	//Wyrmgus start
//	Assert(Map.Info.IsPointOnMap(currentPos));
//	Assert(Map.Info.IsPointOnMap(dest));
	Assert(Map.Info.IsPointOnMap(currentPos, current_z));
	Assert(Map.Info.IsPointOnMap(dest, dest_z));
	//Wyrmgus end

	COrder_Patrol *order = new COrder_Patrol();

	order->goalPos = dest;
	order->WayPoint = currentPos;
	//Wyrmgus start
	order->MapLayer = dest_z;
	order->WayPointMapLayer = current_z;
	//Wyrmgus end
	return order;
}


/* virtual */ void COrder_Patrol::Save(CFile &file, const CUnit &unit) const
{
	file.printf("{\"action-patrol\",");

	if (this->Finished) {
		file.printf(" \"finished\", ");
	}
	file.printf(" \"tile\", {%d, %d},", this->goalPos.x, this->goalPos.y);
	//Wyrmgus start
	file.printf(" \"map-layer\", %d,", this->MapLayer);
	//Wyrmgus end
	file.printf(" \"range\", %d,", this->Range);

	if (this->WaitingCycle != 0) {
		file.printf(" \"waiting-cycle\", %d,", this->WaitingCycle);
	}
	//Wyrmgus start
//	file.printf(" \"patrol\", {%d, %d}", this->WayPoint.x, this->WayPoint.y);
	file.printf(" \"patrol\", {%d, %d},", this->WayPoint.x, this->WayPoint.y);
	file.printf(" \"patrol-map-layer\", %d", this->WayPointMapLayer);
	//Wyrmgus end
	file.printf("}");
}

/* virtual */ bool COrder_Patrol::ParseSpecificData(lua_State *l, int &j, const char *value, const CUnit &unit)
{
	if (!strcmp(value, "patrol")) {
		++j;
		lua_rawgeti(l, -1, j + 1);
		CclGetPos(l, &this->WayPoint.x , &this->WayPoint.y);
		lua_pop(l, 1);
	//Wyrmgus start
	} else if (!strcmp(value, "patrol-map-layer")) {
		++j;
		this->WayPointMapLayer = LuaToNumber(l, -1, j + 1);
	//Wyrmgus end
	} else if (!strcmp(value, "waiting-cycle")) {
		++j;
		this->WaitingCycle = LuaToNumber(l, -1, j + 1);
	} else if (!strcmp(value, "range")) {
		++j;
		this->Range = LuaToNumber(l, -1, j + 1);
	} else if (!strcmp(value, "tile")) {
		++j;
		lua_rawgeti(l, -1, j + 1);
		CclGetPos(l, &this->goalPos.x , &this->goalPos.y);
		lua_pop(l, 1);
	//Wyrmgus start
	} else if (!strcmp(value, "map-layer")) {
		++j;
		this->MapLayer = LuaToNumber(l, -1, j + 1);
	//Wyrmgus end
	} else {
		return false;
	}
	return true;
}

/* virtual */ bool COrder_Patrol::IsValid() const
{
	return true;
}

/* virtual */ PixelPos COrder_Patrol::Show(const CViewport &vp, const PixelPos &lastScreenPos) const
{
	//Wyrmgus start
	if (this->MapLayer != CurrentMapLayer) {
		return lastScreenPos;
	}
	//Wyrmgus end

	const PixelPos pos1 = vp.TilePosToScreen_Center(this->goalPos);
	const PixelPos pos2 = vp.TilePosToScreen_Center(this->WayPoint);

	//Wyrmgus start
//	Video.DrawLineClip(ColorGreen, lastScreenPos, pos1);
//	Video.FillCircleClip(ColorBlue, pos1, 2);
//	Video.DrawLineClip(ColorBlue, pos1, pos2);
//	Video.FillCircleClip(ColorBlue, pos2, 3);
	if (Preference.ShowPathlines) {
		Video.DrawLineClip(ColorGreen, lastScreenPos, pos1);
		Video.FillCircleClip(ColorBlue, pos1, 2);
		Video.DrawLineClip(ColorBlue, pos1, pos2);
		Video.FillCircleClip(ColorBlue, pos2, 3);
	}
	//Wyrmgus end
	return pos2;
}

/* virtual */ void COrder_Patrol::UpdatePathFinderData(PathFinderInput &input)
{
	input.SetMinRange(0);
	input.SetMaxRange(this->Range);
	const Vec2i tileSize(0, 0);
	//Wyrmgus start
//	input.SetGoal(this->goalPos, tileSize);
	input.SetGoal(this->goalPos, tileSize, this->MapLayer);
	//Wyrmgus end
}


/* virtual */ void COrder_Patrol::Execute(CUnit &unit)
{
	if (unit.Wait) {
		if (!unit.Waiting) {
			unit.Waiting = 1;
			unit.WaitBackup = unit.Anim;
		}
		//Wyrmgus start
//		UnitShowAnimation(unit, unit.Type->Animations->Still);
		UnitShowAnimation(unit, unit.GetAnimations()->Still);
		//Wyrmgus end
		unit.Wait--;
		return;
	}
	if (unit.Waiting) {
		unit.Anim = unit.WaitBackup;
		unit.Waiting = 0;
	}

	switch (DoActionMove(unit)) {
		case PF_FAILED:
			this->WaitingCycle = 0;
			break;
		case PF_UNREACHABLE:
			//Wyrmgus start
			if ((Map.Field(unit.tilePos, unit.MapLayer)->Flags & MapFieldBridge) && !unit.Type->BoolFlag[BRIDGE_INDEX].value && unit.Type->UnitType == UnitTypeLand) {
				std::vector<CUnit *> table;
				Select(unit.tilePos, unit.tilePos, table, unit.MapLayer);
				for (size_t i = 0; i != table.size(); ++i) {
					if (!table[i]->Removed && table[i]->Type->BoolFlag[BRIDGE_INDEX].value && table[i]->CanMove()) {
						if (table[i]->CurrentAction() == UnitActionStill) {
							CommandStopUnit(*table[i]);
							CommandMove(*table[i], this->goalPos, FlushCommands, this->MapLayer);
						}
						return;
					}
				}
			}
			//Wyrmgus end
			// Increase range and try again
			this->WaitingCycle = 1;
			this->Range++;
			break;

		case PF_REACHED:
			this->WaitingCycle = 1;
			this->Range = 0;
			std::swap(this->WayPoint, this->goalPos);
			//Wyrmgus start
			std::swap(this->WayPointMapLayer, this->MapLayer);
			//Wyrmgus end

			break;
		case PF_WAIT:
			// Wait for a while then give up
			this->WaitingCycle++;
			if (this->WaitingCycle == 5) {
				this->WaitingCycle = 0;
				this->Range = 0;
				std::swap(this->WayPoint, this->goalPos);
				//Wyrmgus start
				std::swap(this->WayPointMapLayer, this->MapLayer);
				//Wyrmgus end

				unit.pathFinderData->output.Cycles = 0; //moving counter
			}
			break;
		default: // moving
			this->WaitingCycle = 0;
			break;
	}

	if (!unit.Anim.Unbreakable) {
		if (AutoAttack(unit) || AutoRepair(unit) || AutoCast(unit)) {
			this->Finished = true;
		}
	}
}

//@}
