/*
Copyright (C) 2008-2016 Vana Development Team

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

#define CMSG_PLAYER_LOAD 0x14
#define CMSG_MAP_CHANGE 0x25
#define CMSG_CHANNEL_CHANGE 0x26
#define CMSG_CASH_SHOP 0x27
#define CMSG_PLAYER_MOVE 0x28
#define CMSG_CHAIR 0x29
#define CMSG_USE_CHAIR 0x2a
#define CMSG_ATTACK_MELEE 0x2b
#define CMSG_ATTACK_RANGED 0x2c
#define CMSG_ATTACK_MAGIC 0x2d
#define CMSG_ATTACK_ENERGY_CHARGE 0x2e
#define CMSG_PLAYER_DAMAGE 0x2f
#define CMSG_PLAYER_CHAT 0x30
#define CMSG_CHALKBOARD 0x31
#define CMSG_EMOTE 0x32
#define CMSG_ITEM_EFFECT 0x33
#define CMSG_REVIVE_EFFECT 0x34
#define CMSG_MONSTER_BOOK 0x38
#define CMSG_NPC_TALK 0x39
#define CMSG_NPC_TALK_CONT 0x3b // i.e. clicking next on NPC
#define CMSG_SHOP 0x3c
#define CMSG_STORAGE 0x3d
#define CMSG_ITEM_MOVE 0x46
#define CMSG_ITEM_USE 0x47
#define CMSG_ITEM_CANCEL 0x48
#define CMSG_SUMMON_BAG_USE 0x4a
#define CMSG_PET_FOOD_USE 0x4b
#define CMSG_USE_SCRIPT_ITEM 0x4d // For items that runs scripts
#define CMSG_CASH_ITEM_USE 0x4e
#define CMSG_SKILLBOOK_USE 0x51
#define CMSG_TELEPORT_ROCK_USE 0x53 // Only the one in your Use
#define CMSG_TOWN_SCROLL_USE 0x54
#define CMSG_SCROLL_USE 0x55
#define CMSG_STAT_ADDITION 0x56
#define CMSG_MULTI_STAT_ADDITION 0x57 // The auto-assign button
#define CMSG_PLAYER_HEAL 0x58
#define CMSG_SKILL_ADD 0x59
#define CMSG_SKILL_USE 0x5a
#define CMSG_SKILL_CANCEL 0x5b
#define CMSG_SPECIAL_SKILL 0x5c // Chakra, Pierce, Monster Magnet, Big Bang
#define CMSG_DROP_MESOS 0x5d
#define CMSG_FAME 0x5e
#define CMSG_PLAYER_INFO 0x60
#define CMSG_PET_SUMMON 0x61
#define CMSG_MAP_CHANGE_SPECIAL 0x63
#define CMSG_MAP_TELEPORT 0x64
#define CMSG_TELEPORT_ROCK 0x65
#define CMSG_QUEST_OBTAIN 0x6a
#define CMSG_BUFF_ACKNOWLEDGE 0x6b
#define CMSG_BOMB_SKILL_USE 0x6c
#define CMSG_MACRO_LIST 0x6d
#define CMSG_USE_REWARD_ITEM 0x6f
#define CMSG_USE_REMOTE_GACHAPON_TICKET 0x73 // int itemid, int index; Sent when clicking one of the 10 buttons in the UI
#define CMSG_ADMIN_BROADCAST 0x74 // Commands like '/alertall'
#define CMSG_MESSAGE_GROUP 0x75 // Party, buddy and guild chat
#define CMSG_COMMAND 0x76 // / commands like "/find"
#define CMSG_PLAYER_CHATROOM 0x78 // Messenger system
#define CMSG_PLAYER_ROOM 0x79 // Trades, player shops, minigames, etc.
#define CMSG_PARTY 0x7a
#define CMSG_ADMIN_COMMAND 0x7e
#define CMSG_ADMIN_COMMAND_LOG 0x7f // '/(command) (parameters)'
#define CMSG_BUDDY 0x80
#define CMSG_SEND_NOTE 0x81
#define CMSG_MYSTIC_DOOR_ENTRY 0x83
#define CMSG_KEYMAP 0x85
#define CMSG_STALK_BEGIN 0x8c // Click on yourself in the minimap, and you'll send this packet.
#define CMSG_ALLIANCE 0x8d
#define CMSG_ALLIANCE_DENIED 0x8e
#define CMSG_BBS 0x99
#define CMSG_MTS 0x9a
#define CMSG_MOB_CRC_KEY_CHANGED_REPLY 0x9b
#define CMSG_USE_EXP_UP_ITEM 0x9c // int tickcount, short pos, int itemid
#define CMSG_USE_TEMP_EXP_UP_ITEM 0x9d // int tickcount
#define CMSG_BUFF_ITEM_USE 0x9f
#define CMSG_PET_MOVEMENT 0xa1
#define CMSG_PET_CHAT 0xa2
#define CMSG_PET_COMMAND 0xa3
#define CMSG_PET_LOOT 0xa4
#define CMSG_PET_CONSUME_POTION 0xa5
#define CMSG_PET_UPDATE_EXCEPTION_LIST 0xa6
#define CMSG_SUMMON_MOVEMENT 0xa9
#define CMSG_SUMMON_ATTACK 0xaa
#define CMSG_SUMMON_DAMAGE 0xab
#define CMSG_SUMMON_SKILL 0xac
#define CMSG_MOB_CONTROL 0xb2
#define CMSG_PLAYER_MOB_DISTANCE 0xb3
#define CMSG_FRIENDLY_MOB_DAMAGE 0xb6
#define CMSG_MOB_EXPLOSION 0xb7
#define CMSG_MOB_TURNCOAT_DAMAGE 0xb8 // For Hypnotize
#define CMSG_NPC_ANIMATE 0xbb
#define CMSG_ITEM_LOOT 0xc0
#define CMSG_REACTOR_HIT 0xc3
#define CMSG_REACTOR_TOUCH 0xc4
#define CMSG_RESET_NLCPQ 0xc5 // Send on field init. Not sure why, probably an ugly hack. Exists in v.95 and appeared around v.75
#define CMSG_EVENT_START 0xc8 // Sent when admin uses /start
#define CMSG_PLAYER_BOAT_MAP 0xd2 // Packet format [map ID:4] [?:1], seems like this gets sent when a player spawns on a boat map, the ? I got was never anything other than 0
#define CMSG_PLAYER_UNK_MAP2 0xd5 // These almost seem like acknowledgement packets given the lack of any data
#define CMSG_CASHSHOP_CHECK_CASH 0xda
#define CMSG_CASHSHOP_REDEEM_COUPON 0xdc
#define CMSG_CASHSHOP_OPERATION 0xdb
#define CMSG_ADMIN_MESSENGER 0xeb
#define CMSG_HAMMER 0xf8
