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

#define CMSG_AUTHENTICATION 0x01
#define CMSG_WORLD_LIST_REFRESH 0x04 // Click back after select channel
#define CMSG_PLAYER_LIST 0x05
#define CMSG_WORLD_STATUS 0x06
#define CMSG_ACCOUNT_GENDER 0x08
#define CMSG_PIN 0x09
#define CMSG_REGISTER_PIN 0x0a
#define CMSG_WORLD_LIST 0x0b
#define CMSG_PLAYER_GLOBAL_LIST 0x0d
#define CMSG_PLAYER_GLOBAL_LIST_CHANNEL_CONNECT 0x0e
#define CMSG_PLAYER_LOAD 0x14
#define CMSG_PLAYER_NAME_CHECK 0x15
#define CMSG_PLAYER_CREATE 0x16
#define CMSG_PLAYER_DELETE 0x17
#define CMSG_PONG 0x18
#define CMSG_PIC 0x1e
#define CMSG_CHANNEL_CONNECT 0x1D
#define CMSG_CLIENT_ERROR 0x21
#define CMSG_LOGIN_RETURN 0xFFF1d
#define CMSG_MAP_CHANGE 0x26
#define CMSG_CHANNEL_CHANGE 0x27
#define CMSG_CASH_SHOP 0x28
#define CMSG_PLAYER_MOVE 0x29
#define CMSG_CHAIR 0x2a
#define CMSG_USE_CHAIR 0x2b
#define CMSG_ATTACK_MELEE 0x2c
#define CMSG_ATTACK_RANGED 0x2d
#define CMSG_ATTACK_MAGIC 0x2e
#define CMSG_ATTACK_ENERGY_CHARGE 0x2f
#define CMSG_PLAYER_DAMAGE 0x30
#define CMSG_PLAYER_CHAT 0x31
#define CMSG_CHALKBOARD 0x32
#define CMSG_EMOTE 0x33
#define CMSG_ITEM_EFFECT 0x34
#define CMSG_REVIVE_EFFECT 0x35
#define CMSG_MONSTER_BOOK 0x39
#define CMSG_NPC_TALK 0x3a
#define CMSG_NPC_TALK_CONT 0x3c // i.e. clicking next on NPC
#define CMSG_SHOP 0x3d
#define CMSG_STORAGE 0x3e
#define CMSG_ITEM_MOVE 0x47
#define CMSG_ITEM_USE 0x48
#define CMSG_ITEM_CANCEL 0x49
#define CMSG_SUMMON_BAG_USE 0x4b
#define CMSG_PET_FOOD_USE 0x4c
#define CMSG_USE_SCRIPT_ITEM 0x4e // For items that runs scripts
#define CMSG_CASH_ITEM_USE 0x4f
#define CMSG_SKILLBOOK_USE 0x52
#define CMSG_TELEPORT_ROCK_USE 0x54 // Only the one in your Use
#define CMSG_TOWN_SCROLL_USE 0x55
#define CMSG_SCROLL_USE 0x56
#define CMSG_STAT_ADDITION 0x57
#define CMSG_MULTI_STAT_ADDITION 0x58 // The auto-assign button
#define CMSG_PLAYER_HEAL 0x59
#define CMSG_SKILL_ADD 0x5a
#define CMSG_SKILL_USE 0x5b
#define CMSG_SKILL_CANCEL 0x5c
#define CMSG_SPECIAL_SKILL 0x5d // Chakra, Pierce, Monster Magnet, Big Bang
#define CMSG_DROP_MESOS 0x5e
#define CMSG_FAME 0x5f
#define CMSG_PLAYER_INFO 0x61
#define CMSG_PET_SUMMON 0x62
#define CMSG_MAP_CHANGE_SPECIAL 0x64
#define CMSG_MAP_TELEPORT 0x65
#define CMSG_TELEPORT_ROCK 0x66
#define CMSG_QUEST_OBTAIN 0x6b
#define CMSG_MACRO_LIST 0x6e
#define CMSG_USE_REWARD_ITEM 0x70
#define CMSG_MESSAGE_GROUP 0x76 // Party, buddy and guild chat
#define CMSG_COMMAND 0x77 // / commands like "/find"
#define CMSG_PLAYER_ROOM 0x7a // Trades, player shops, minigames, etc.
#define CMSG_PARTY 0x7b
#define CMSG_GUILD 0x7d
#define CMSG_GUILD_DENIED 0x7e
#define CMSG_ADMIN_COMMAND 0x7f
#define CMSG_ADMIN_COMMAND_LOG 0x80 // '/(command) (parameters)'
#define CMSG_BUDDY 0x81
#define CMSG_KEYMAP 0x86
#define CMSG_ALLIANCE 0x8e
#define CMSG_ALLIANCE_DENIED 0x8f
#define CMSG_BBS 0x9b
#define CMSG_MTS 0x9c
#define CMSG_COMBO_COUNTER_UP 0xa3
#define CMSG_PET_MOVEMENT 0xa7
#define CMSG_PET_CHAT 0xa8
#define CMSG_PET_COMMAND 0xa9
#define CMSG_PET_LOOT 0xaa
#define CMSG_PET_CONSUME_POTION 0xab
#define CMSG_SUMMON_MOVEMENT 0xaf
#define CMSG_SUMMON_ATTACK 0xb0
#define CMSG_SUMMON_DAMAGE 0xb1
#define CMSG_MOB_CONTROL 0xbc
#define CMSG_PLAYER_MOB_DISTANCE 0xbd
#define CMSG_FRIENDLY_MOB_DAMAGE 0xc0
#define CMSG_MOB_EXPLOSION 0xc1
#define CMSG_MOB_TURNCOAT_DAMAGE 0xc2 // For Hypnotize
#define CMSG_NPC_ANIMATE 0xc5
#define CMSG_ITEM_LOOT 0xca
#define CMSG_REACTOR_HIT 0xcd
#define CMSG_REACTOR_TOUCH 0xce
#define CMSG_ADMIN_MESSENGER 0xf5
#define CMSG_HAMMER 0x102