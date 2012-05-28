/*
Copyright (C) 2008-2012 Vana Development Team

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

// Order later
#define CMSG_BACKUP_PACKET 0x3b

#define CMSG_CLIENT_VERSION 0x14
#define CMSG_AUTHENTICATION 0x15
#define CMSG_WORLD_LIST_REFRESH 0x18 // Click back after select channel
#define CMSG_PLAYER_LIST 0x19
#define CMSG_WORLD_STATUS 0x1a
#define CMSG_ACCOUNT_GENDER 0x1c
#define CMSG_PIN 0x1d
#define CMSG_REGISTER_PIN 0x1e
#define CMSG_WORLD_LIST 0x1f
#define CMSG_PLAYER_GLOBAL_LIST 0x21
#define CMSG_PLAYER_GLOBAL_LIST_CHANNEL_CONNECT 0x22
#define CMSG_REQUEST_NAME_CHANGE 0x24
#define CMSG_REQUEST_CHARACTER_TRANSFER 0x26
#define CMSG_CHANNEL_CONNECT 0x27
#define CMSG_PLAYER_LOAD 0x28
#define CMSG_PLAYER_NAME_CHECK 0x29
#define CMSG_PLAYER_CREATE 0x2a
#define CMSG_PLAYER_DELETE 0x2d // 0x2b = without pic?
#define CMSG_PONG 0x2e
#define CMSG_CLIENT_ERROR 0x2f
#define CMSG_LOGIN_RETURN 0xF1d
#define CMSG_CHANNEL_CONNECT_PIC 0x33
#define CMSG_PLAYER_GLOBAL_LIST_CHANNEL_CONNECT_PIC 0x35
#define CMSG_CLIENT_STARTED 0x38
#define CMSG_REQUEST_SPECIAL_CHAR_CREATION 0x3e // For enabling 'special character creation'
#define CMSG_PLAYER_CREATE_SPECIAL 0x3f
#define CMSG_MAP_CHANGE 0x42
#define CMSG_CHANNEL_CHANGE 0xF26
#define CMSG_CASH_SHOP 0xF27
#define CMSG_PLAYER_MOVE 0x49
#define CMSG_CHAIR 0x4B
#define CMSG_USE_CHAIR 0x4C
#define CMSG_ATTACK_MELEE 0x4D
#define CMSG_ATTACK_RANGED 0x4E
#define CMSG_ATTACK_MAGIC 0x4F
#define CMSG_ATTACK_ENERGY_CHARGE 0x50
#define CMSG_PLAYER_DAMAGE 0x52
#define CMSG_PLAYER_CHAT 0x54
#define CMSG_CHALKBOARD 0xF31
#define CMSG_EMOTE 0x56
#define CMSG_ITEM_EFFECT 0x57
#define CMSG_REVIVE_EFFECT 0x58
#define CMSG_MONSTER_BOOK 0xF38
#define CMSG_NPC_TALK 0x66
#define CMSG_NPC_TALK_CONT 0x68 // i.e. clicking next on NPC
#define CMSG_NPC_TALK_CONT_2 0x69
#define CMSG_SHOP 0xF3c
#define CMSG_STORAGE 0xF3d
#define CMSG_ITEM_MOVE 0x75
#define CMSG_ITEM_USE 0x76
#define CMSG_ITEM_CANCEL 0x77
#define CMSG_SUMMON_BAG_USE 0xF4a
#define CMSG_PET_FOOD_USE 0xF4b
#define CMSG_USE_SCRIPT_ITEM 0xF4d // For items that runs scripts
#define CMSG_CASH_ITEM_USE 0xF4e
#define CMSG_SKILLBOOK_USE 0xF51
#define CMSG_TELEPORT_ROCK_USE 0xF53 // Only the one in your Use
#define CMSG_TOWN_SCROLL_USE 0xF54
#define CMSG_SCROLL_USE 0xF55
#define CMSG_STAT_ADDITION 0xF56
#define CMSG_MULTI_STAT_ADDITION 0xF57 // The auto-assign button
#define CMSG_PLAYER_HEAL 0xF58
#define CMSG_SKILL_ADD 0x9F
#define CMSG_SKILL_USE 0xA0
#define CMSG_SKILL_CANCEL 0xA1
#define CMSG_SPECIAL_SKILL 0xF5c // Chakra, Pierce, Monster Magnet, Big Bang
#define CMSG_DROP_MESOS 0xF5d
#define CMSG_FAME 0xF5e
#define CMSG_PLAYER_INFO 0xA5
#define CMSG_PET_SUMMON 0xF61
#define CMSG_MAP_CHANGE_SPECIAL 0xAF
#define CMSG_MAP_TELEPORT 0xF64
#define CMSG_TELEPORT_ROCK 0xB2
#define CMSG_QUEST_OBTAIN 0xB8
#define CMSG_MACRO_LIST 0xBC
#define CMSG_USE_REWARD_ITEM 0xF6f
#define CMSG_MESSAGE_GROUP 0xF75 // Party, buddy and guild chat
#define CMSG_COMMAND 0xF76 // / commands like "/find"
#define CMSG_PLAYER_ROOM 0xF79 // Trades, player shops, minigames, etc.
#define CMSG_PARTY 0xE2
#define CMSG_ADMIN_COMMAND 0xE8
#define CMSG_ADMIN_COMMAND_LOG 0xE9 // '/(command) (parameters)'
#define CMSG_BUDDY 0xEB
#define CMSG_SEND_NOTE 0xF81
#define CMSG_MYSTIC_DOOR_ENTRY 0xF83
#define CMSG_KEYMAP 0xF1
#define CMSG_MTS 0xF9a
#define CMSG_PET_MOVEMENT 0xFa1
#define CMSG_PET_CHAT 0xFa2
#define CMSG_PET_COMMAND 0xFa3
#define CMSG_PET_LOOT 0xFa4
#define CMSG_PET_CONSUME_POTION 0xFa5
#define CMSG_SUMMON_MOVEMENT 0xFa9
#define CMSG_SUMMON_ATTACK 0xFaa
#define CMSG_SUMMON_DAMAGE 0xFab
#define CMSG_MOB_CONTROL 0x14F
#define CMSG_PLAYER_MOB_DISTANCE 0x150
#define CMSG_FRIENDLY_MOB_DAMAGE 0xFb6
#define CMSG_MOB_EXPLOSION 0xFb7
#define CMSG_MOB_TURNCOAT_DAMAGE 0xFb8 // For Hypnotize
#define CMSG_NPC_ANIMATE 0x15F
#define CMSG_ITEM_LOOT 0xFc0
#define CMSG_REACTOR_HIT 0xFc3
#define CMSG_REACTOR_TOUCH 0xFc4
#define CMSG_CASHSHOP_CHECK_CASH 0xFda
#define CMSG_CASHSHOP_REDEEM_COUPON 0xFdc
#define CMSG_CASHSHOP_OPERATION 0xFdb
#define CMSG_ADMIN_MESSENGER 0xFeb
#define CMSG_HAMMER 0xFf8