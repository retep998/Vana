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

#define SMSG_AUTHENTICATION 0x00
#define SMSG_WORLD_STATUS 0x03
#define SMSG_ACCOUNT_GENDER_DONE 0x04
#define SMSG_PIN 0x06
#define SMSG_PIN_ASSIGNED 0x07
#define SMSG_PLAYER_GLOBAL_LIST 0x08
#define SMSG_WORLD_LIST 0x0a
#define SMSG_PLAYER_LIST 0x0b
#define SMSG_CHANNEL_CONNECT 0x0c
#define SMSG_PLAYER_NAME_CHECK 0x0d
#define SMSG_PLAYER_CREATE 0x0e
#define SMSG_PLAYER_DELETE 0x0f
#define SMSG_CHANNEL_CHANGE 0x10
#define SMSG_PING 0x11
#define SMSG_CHANNEL_SELECT 0x14
#define SMSG_LOGIN_RETURN 0x16
#define SMSG_INVENTORY_ITEM_MOVE 0x1a
#define SMSG_INVENTORY_SLOT_UPDATE 0x1b
#define SMSG_PLAYER_UPDATE 0x1c
#define SMSG_SKILL_USE 0x1d
#define SMSG_SKILL_CANCEL 0x1e
#define SMSG_SKILL_ADD 0x21
#define SMSG_FAME 0x23
#define SMSG_NOTE 0x24 // Bottom-right corner notice
#define SMSG_TELEPORT_ROCK 0x27 // Teleport Rock, VIP Teleport Rock
#define SMSG_QUEST_COMPLETED 0x2e
#define SMSG_SKILLBOOK 0x30 // Skillbooks
#define SMSG_ITEM_EFFECT 0x31
#define SMSG_BBS 0x38
#define SMSG_PLAYER_INFO 0x3a
#define SMSG_PARTY 0x3b
#define SMSG_BUDDY 0x3c
#define SMSG_GUILD 0x3e
#define SMSG_ALLIANCE 0x3f
#define SMSG_MESSAGE 0x41
#define SMSG_MONSTER_BOOK_ADD 0x4f
#define SMSG_MONSTER_BOOK_COVER 0x50
#define SMSG_PLAYER_LEVEL_UPDATE 0x63 // Byte after it: 2 = guild, everything else = family
#define SMSG_MESSENGER 0x67 // Also known as avatar megaphone
#define SMSG_MACRO_LIST 0x71
#define SMSG_CHANGE_MAP 0x72
#define SMSG_PORTAL_BLOCKED 0x77
#define SMSG_CHANNEL_BLOCKED 0x78 // For messages like 'This server is unavailable'
#define SMSG_MAP_FORCE_EQUIPMENT 0x79 // Wear apple suit on training maps, all equips except bathrobe off for Showa spa, etc.
#define SMSG_MESSAGE_GROUP 0x7a // Groups like party, buddies and guild
#define SMSG_COMMAND 0x7b // Respond to / commands i.e. "/find"
#define SMSG_MAP_EFFECT 0x7e // Change Music/Boss HP Bars
#define SMSG_CASH_SONG 0x83 // Used for cash items only, plays a song... :D
#define SMSG_GM 0x84 // Hide, other miscellaneous junk
#define SMSG_EVENT_INSTRUCTION 0x86 // GM event instructions - Ola Ola, etc.
#define SMSG_TIMER 0x87 // For places with clock i.e. ship station
#define SMSG_GM_WARNING 0x8c // Shows a popup with the message from a GM/an Admin
#define SMSG_TIMER_OFF 0x8e // Makes timer disappear from map
#define SMSG_MAP_SPAWN_PLAYER 0x91
#define SMSG_MAP_REMOVE_PLAYER 0x92
#define SMSG_PLAYER_CHAT 0x93
#define SMSG_CHALKBOARD 0x95
#define SMSG_SCROLL_USE 0x98
#define SMSG_PET_SUMMON 0x99
#define SMSG_PET_MOVEMENT 0x9b
#define SMSG_PET_MESSAGE 0x9c
#define SMSG_PET_NAME_CHANGE 0x9d
#define SMSG_PET_SHOW 0x9e
#define SMSG_PET_ANIMATION 0x9f
#define SMSG_SUMMON_SPAWN 0xa0
#define SMSG_SUMMON_DESPAWN 0xa1
#define SMSG_SUMMON_MOVEMENT 0xa2
#define SMSG_SUMMON_ATTACK 0xa3
#define SMSG_SUMMON_DAMAGE 0xa4
#define SMSG_PLAYER_MOVEMENT 0xa7
#define SMSG_ATTACK_MELEE 0xa8
#define SMSG_ATTACK_RANGED 0xa9
#define SMSG_ATTACK_MAGIC 0xaa
#define SMSG_SPECIAL_SKILL 0xac
#define SMSG_SPECIAL_SKILL_END 0xad
#define SMSG_PLAYER_DAMAGE 0xae
#define SMSG_EMOTE 0xaf
#define SMSG_CHAIR_SIT 0xb2
#define SMSG_PLAYER_CHANGE_LOOK 0xb3
#define SMSG_SKILL_SHOW 0xb4
#define SMSG_3RD_PARTY_SKILL 0xb5 // From other player
#define SMSG_3RD_PARTY_BUFF_END 0xb6 // From other player
#define SMSG_PARTY_HP_DISPLAY 0xb7
#define SMSG_GUILD_NAME 0xb8 // Updates guildname from character :D
#define SMSG_GUILD_EMBLEM 0xb9 // Updates guildemblem from character :D
#define SMSG_CHAIR 0xbb
#define SMSG_THEATRICS 0xbc // Gain item notice in chat
#define SMSG_MAP_TELEPORT 0xbd // Instantaneous transfer
#define SMSG_MESOBAG_SUCCESS 0xbf // "You recieved ... mesos from mesobag"
#define SMSG_MESOBAG_FAILURE 0xc0 // If the mesobag has failed?
#define SMSG_QUEST_UPDATE 0xc1
#define SMSG_BUBBLE 0xc4
#define SMSG_SKILL_COOLDOWN 0xce // Skill cooldown
#define SMSG_MOB_SHOW 0xd0
#define SMSG_MOB_DEATH 0xd1
#define SMSG_MOB_CONTROL 0xd2
#define SMSG_MOB_CONTROL_MOVEMENT 0xd3
#define SMSG_MOB_MOVEMENT 0xd4
#define SMSG_MOB_STATUS_ADDITION 0xd6
#define SMSG_MOB_STATUS_REMOVE 0xd7
#define SMSG_MOB_DAMAGE 0xda
#define SMSG_MOB_HP_DISPLAY 0xde
#define SMSG_MOB_DRAGGED 0xdf // New in .56, Monster Magnet'd mobs get their own packet
#define SMSG_NPC_SHOW 0xe3
#define SMSG_NPC_CONTROL 0xe5
#define SMSG_NPC_ANIMATE 0xe6
#define SMSG_NPC_SHOW_EFFECT 0xe7
#define SMSG_DROP_ITEM 0xee
#define SMSG_DROP_PICKUP 0xef
#define SMSG_MIST_SPAWN 0xf3
#define SMSG_MIST_DESPAWN 0xf4
#define SMSG_REACTOR_TRIGGER 0xf7
#define SMSG_REACTOR_SPAWN 0xf9
#define SMSG_REACTOR_DESPAWN 0xfa
#define SMSG_NPC_TALK 0x10e
#define SMSG_SHOP 0x10f
#define SMSG_ITEM_PURCHASED 0x110
#define SMSG_STORAGE 0x113
#define SMSG_PLAYER_ROOM 0x118 // Trades, player shops, minigames
#define SMSG_KEYMAP 0x12a
#define SMSG_MAPLETV_ON 0x130
#define SMSG_MAPLETV_OFF 0x131
#define SMSG_HAMMER 0x13d