/*
Copyright (C) 2008-2009 Vana Development Team

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

#define RECV_LOGIN_INFO 0x01
#define RECV_SHOW_WORLD2 0x04 // Click back after select channel
#define RECV_CHANNEL_SELECT 0x05
#define RECV_WORLD_SELECT 0x06
#define RECV_SET_GENDER 0x08
#define RECV_LOGIN_PROCESS 0x09
#define RECV_REGISTER_PIN 0x0a
#define RECV_SHOW_WORLD 0x0b
#define RECV_VIEW_ALL_CHAR 0x0d
#define RECV_VIEW_ALL_CHAR_GET_CHANNEL_SERVER_INFO 0x0e
#define RECV_GET_CHANNEL_SERVER_INFO 0x13
#define RECV_CHANNEL_LOGIN 0x14
#define RECV_CHECK_CHAR_NAME 0x15
#define RECV_CREATE_CHAR 0x16
#define RECV_DELETE_CHAR 0x18
#define RECV_PONG 0x19
#define RECV_RELOG 0x1d
#define RECV_CHANGE_MAP 0x25
#define RECV_CHANGE_CHANNEL 0x26
#define RECV_CASH_BUTTON 0x27
#define RECV_MOVE_PLAYER 0x28
#define RECV_CHAIR_ACTION 0x29
#define RECV_USE_CHAIR 0x2a
#define RECV_ATTACK_MELEE 0x2b
#define RECV_ATTACK_RANGED 0x2c
#define RECV_ATTACK_SPELL 0x2d
#define RECV_ATTACK_ENERGYCHARGE 0x2e
#define RECV_DAMAGE_PLAYER 0x2f
#define RECV_CHAT 0x30
#define RECV_FACE_EXPRESSION 0x32
#define RECV_USE_ITEM_EFFECT 0x33
#define RECV_MONSTERBOOK 0x38
#define RECV_NPC_TALK 0x39
#define RECV_NPC_TALK_CONT 0x3b // i.e. clicking next on NPC
#define RECV_SHOP_ENTER 0x3c
#define RECV_USE_STORAGE 0x3d
#define RECV_MOVE_ITEM 0x46
#define RECV_USE_ITEM 0x47
#define RECV_CANCEL_ITEM 0x48
#define RECV_USE_SUMMON_BAG 0x4a
#define RECV_PET_FEED 0x4b
#define RECV_USE_CASH_ITEM 0x4e
#define RECV_USE_SKILLBOOK 0x51
#define RECV_USE_TELEPORT_ROCK 0x53 // Only the one in your Use
#define RECV_USE_RETURN_SCROLL 0x54
#define RECV_USE_SCROLL 0x55
#define RECV_ADD_STAT 0x56
#define RECV_ADD_STAT_MULTI 0x57 // The auto-assign button
#define RECV_HEAL_PLAYER 0x58
#define RECV_ADD_SKILL 0x59
#define RECV_USE_SKILL 0x5a
#define RECV_CANCEL_SKILL 0x5b
#define RECV_SPECIAL_SKILL 0x5c // Chakra, Pierce, Monster Magnet, Big Bang
#define RECV_DROP_MESO 0x5d
#define RECV_FAME 0x5e
#define RECV_GET_PLAYER_INFO 0x60
#define RECV_PET_SUMMON 0x61
#define RECV_CHANGE_MAP_SPECIAL 0x63
#define RECV_USE_MAP_TELEPORT 0x64
#define RECV_TELEPORT_ROCK_FUNCTION 0x65
#define RECV_GET_QUEST 0x6a
#define RECV_SKILL_MACRO 0x6d
#define RECV_GROUP_CHAT 0x75 // Party, buddy and guild chat
#define RECV_COMMAND 0x76 // / commands like "/find"
#define RECV_PLAYER_ROOM_ACTION 0x79 // Trades, player shops, minigames, etc.
#define RECV_PARTY_ACTION 0x7a
#define RECV_BUDDYLIST 0x80
#define RECV_KEYMAP 0x85
#define RECV_MTS_BUTTON 0x9a
#define RECV_PET_MOVE 0xa1
#define RECV_PET_CHAT 0xa2
#define RECV_PET_COMMAND 0xa3
#define RECV_PET_LOOT 0xa4
#define RECV_MOVE_SUMMON 0xa9
#define RECV_ATTACK_SUMMON 0xaa
#define RECV_DAMAGE_SUMMON 0xab
#define RECV_CONTROL_MOB 0xb2
#define RECV_DISTANCE_TO_MOB 0xb3
#define RECV_FRIENDLY_MOB_DAMAGED 0xb6
#define RECV_MOB_BOMB_EXPLOSION 0xb7
#define RECV_MOB_DAMAGE_MOB 0xb8 // For Hypnotize
#define RECV_ANIMATE_NPC 0xbb
#define RECV_LOOT_ITEM 0xc0
#define RECV_HIT_REACTOR 0xc3
#define RECV_TOUCH_REACTOR 0xc4