/*
Copyright (C) 2008-2010 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#pragma once

#define IV_NO_PATCH_LOCATION 0x0d
#define IV_PATCH_LOCATION 0x0e

#define SMSG_AUTHENTICATION 0x00 //
#define SMSG_WORLD_STATUS 0x03 //
#define SMSG_ACCOUNT_GENDER_DONE 0x04 //
#define SMSG_PIN 0x06 //
#define SMSG_PIN_ASSIGNED 0x07 //
#define SMSG_PLAYER_GLOBAL_LIST 0x08 //
#define SMSG_WORLD_LIST 0x0A //
#define SMSG_PLAYER_LIST 0x0B //
#define SMSG_CHANNEL_CONNECT 0x0C //
#define SMSG_PLAYER_NAME_CHECK 0x0D //
#define SMSG_PLAYER_CREATE 0x0E //
#define SMSG_PLAYER_DELETE 0x0F //
#define SMSG_CHANNEL_CHANGE 0x10 //
#define SMSG_PING 0x11 //
#define SMSG_CHANNEL_SELECT 0x14 //
#define SMSG_LOGIN_RETURN 0x16 //??? I don't know!!!?!
#define SMSG_INVENTORY_ITEM_MOVE 0x1D //
#define SMSG_INVENTORY_SLOT_UPDATE 0x1E //
#define SMSG_PLAYER_UPDATE 0x1F //
#define SMSG_SKILL_USE 0x20 //
#define SMSG_SKILL_CANCEL 0x21 //
#define SMSG_SKILL_ADD 0x24 //
#define SMSG_FAME 0x26 //
#define SMSG_NOTE 0x27 //Bottom-right corner notice
#define SMSG_TELEPORT_ROCK 0x2A //
#define SMSG_QUEST_COMPLETED 0x31 //
#define SMSG_SKILLBOOK 0x33 //
#define SMSG_ITEM_EFFECT 0x34 //
#define SMSG_BBS 0x3B //
#define SMSG_PLAYER_INFO 0x3D //
#define SMSG_PARTY 0x3E //
#define SMSG_BUDDY 0x3F //
#define SMSG_GUILD 0x41 //
#define SMSG_ALLIANCE 0x42 //
#define SMSG_MESSAGE 0x44 //
#define SMSG_MONSTER_BOOK_ADD 0x52 //
#define SMSG_MONSTER_BOOK_COVER 0x54 //
#define SMSG_PLAYER_LEVEL_UPDATE 0x63 // Byte after it: 2 = guild, everything else = family
#define SMSG_FAMILY_BUFF_LIST 0x64 //+2
#define SMSG_MESSENGER 0x6F //+2
#define SMSG_GM_POLICE 0x74 //+2
#define SMSG_MACRO_LIST 0x7C //+2
#define SMSG_CHANGE_MAP 0x7D //+2
#define SMSG_ENTER_CASHSHOP 0x7F //+2
#define SMSG_PORTAL_BLOCKED 0x83 //+2
#define SMSG_CHANNEL_BLOCKED 0x84 // For messages like 'This server is unavailable'
#define SMSG_MAP_FORCE_EQUIPMENT 0x85 //+2 // Wear apple suit on training maps
#define SMSG_MESSAGE_GROUP 0x86 //+2 // Groups like party  buddies and guild
#define SMSG_COMMAND 0x87 //+2 // Respond to / commands i.e. "/find"
#define SMSG_MAP_EFFECT 0x8A //+2 // Change Music/Boss HP Bars
#define SMSG_CASH_SONG 0x8F // Used for cash items only, plays a song... :D
#define SMSG_GM 0x90 // Hide, other miscellaneous junk
#define SMSG_EVENT_INSTRUCTION 0x92 //+2 // GM event instructions - Ola Ola  etc.
#define SMSG_TIMER 0x93 //+2 // For places with clock i.e. ship station
#define SMSG_GM_WARNING 0x98 // Shows a popup with the message from a GM/an Admin
#define SMSG_TIMER_OFF 0x9A //+2 // Makes timer disappear from map
#define SMSG_MAP_SPAWN_PLAYER 0xA0 //+3
#define SMSG_MAP_REMOVE_PLAYER 0xA1 //+3
#define SMSG_PLAYER_CHAT 0xA2 //+3
#define SMSG_CHALKBOARD 0xA4
#define SMSG_SCROLL_USE 0xA7 //+3
#define SMSG_PET_SUMMON 0xA8 //+3
#define SMSG_PET_MOVEMENT 0xAA //+3
#define SMSG_PET_MESSAGE 0xAB //+3
#define SMSG_PET_NAME_CHANGE 0xAC //+3
#define SMSG_PET_SHOW 0xAD //+3
#define SMSG_PET_ANIMATION 0xAE //+3
#define SMSG_SUMMON_SPAWN 0xAF //+3
#define SMSG_SUMMON_DESPAWN 0xB0 //+3
#define SMSG_SUMMON_MOVEMENT 0xB1 //+3
#define SMSG_SUMMON_ATTACK 0xB2 //+3
#define SMSG_SUMMON_DAMAGE 0xB3 //+3
#define SMSG_PLAYER_MOVEMENT 0xB9 //+6
#define SMSG_ATTACK_MELEE 0xBA //+6
#define SMSG_ATTACK_RANGED 0xBB //+6
#define SMSG_ATTACK_MAGIC 0xBC //+6
#define SMSG_ATTACK_ENERGY_CHARGE 0xBD //+6
#define SMSG_SPECIAL_SKILL 0xBE //+6
#define SMSG_SPECIAL_SKILL_END 0xBF //+6
#define SMSG_PLAYER_DAMAGE 0xC0 //+6
#define SMSG_EMOTE 0xC1 //+6
#define SMSG_CHAIR_SIT 0xC4 //+6
#define SMSG_PLAYER_CHANGE_LOOK 0xC5 //+6
#define SMSG_SKILL_SHOW 0xC6 //+6
#define SMSG_3RD_PARTY_SKILL 0xC7 //+6 // From other player
#define SMSG_3RD_PARTY_BUFF_END 0xC8 //+6 // From other player
#define SMSG_PARTY_HP_DISPLAY 0xC9 //+6
#define SMSG_GUILD_NAME 0xCa // Updates guildname from character :D
#define SMSG_GUILD_EMBLEM 0xCb // Updates guildemblem from character :D
#define SMSG_CHAIR 0xCD //+6
#define SMSG_THEATRICS 0xCE //+6 // Gain item notice in chat
#define SMSG_MAP_TELEPORT 0xcf // Instantaneous transfer
#define SMSG_MESOBAG_SUCCESS 0xd1 // "You recieved ... mesos from mesobag"
#define SMSG_MESOBAG_FAILURE 0xd2 // If the mesobag has failed?
#define SMSG_QUEST_UPDATE 0xD3 //+6
#define SMSG_BUBBLE 0xC8 //OUTDATED
#define SMSG_SOUND 0xca
#define SMSG_COMBO_COUNTER 0xe1
#define SMSG_SKILL_COOLDOWN 0xEA //+8 was E2
#define SMSG_MOB_SHOW 0xEC //+8
#define SMSG_MOB_DEATH 0xED //+8
#define SMSG_MOB_CONTROL 0xEE //+8
#define SMSG_MOB_CONTROL_MOVEMENT 0xEF //+8
#define SMSG_MOB_MOVEMENT 0xF0 //+8
#define SMSG_MOB_STATUS_ADDITION 0xF2 //+8
#define SMSG_MOB_STATUS_REMOVE 0xF3 //+8
#define SMSG_MOB_DAMAGE 0xF6 //+8
#define SMSG_MOB_HP_DISPLAY 0xFA //+8
#define SMSG_MOB_DRAGGED 0xFB //+8 //GDone?(+14)? OLD 78 GUESS!!!! //Monster Magnet'd
#define SMSG_NPC_SHOW 0x101 //+8
#define SMSG_NPC_CONTROL 0x103 //+8
#define SMSG_NPC_ANIMATE 0x104 //+8
#define SMSG_NPC_SHOW_EFFECT 0x105
#define SMSG_DROP_ITEM 0x10C //+8
#define SMSG_DROP_PICKUP 0x10D //+8
#define SMSG_MIST_SPAWN 0x111 //+8
#define SMSG_MIST_DESPAWN 0x112 //+8
#define SMSG_REACTOR_TRIGGER 0x115 //+8
#define SMSG_REACTOR_SPAWN 0x117 //+8
#define SMSG_REACTOR_DESPAWN 0x118 //+8
#define SMSG_NPC_TALK 0x130 //+8
#define SMSG_SHOP 0x131 //+8
#define SMSG_ITEM_PURCHASED 0x132 //+8
#define SMSG_STORAGE 0x135 //+8
#define SMSG_PLAYER_ROOM 0x13A //+8 // Trades  player shops  minigames
#define SMSG_KEYMAP 0x14F //+10
#define SMSG_MAPLETV_ON 0x155 //+10 (GUESS) GDone(+14)  guess was 13D   OLDGUESS!!!!! (+13) was 0x130
#define SMSG_MAPLETV_OFF 0x156 //+10 (GUESS) GDone(+14)  guess was 13E   OLDGUESS!!!!! (+13) was 0x131
#define SMSG_HAMMER 0x162