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
#define RECV_DELETE_CHAR 0x17
#define RECV_PONG 0x18
#define RECV_RELOG 0x1c
#define RECV_CHANGE_MAP 0x23
#define RECV_CHANGE_CHANNEL 0x24
#define RECV_CASH_BUTTON 0x25
#define RECV_MOVE_PLAYER 0x26
#define RECV_CHAIR_ACTION 0x27
#define RECV_USE_CHAIR 0x28
#define RECV_ATTACK_MELEE 0x29
#define RECV_ATTACK_RANGED 0x2a
#define RECV_ATTACK_SPELL 0x2b
#define RECV_ATTACK_ENERGYCHARGE 0x2c
#define RECV_DAMAGE_PLAYER 0x2d
#define RECV_CHAT 0x2e
#define RECV_FACE_EXPRESSION 0x30
#define RECV_USE_ITEM_EFFECT 0x31
#define RECV_NPC_TALK 0x36
#define RECV_NPC_TALK_CONT 0x38 // i.e. clicking next on NPC
#define RECV_SHOP_ENTER 0x39
#define RECV_USE_STORAGE 0x3a
#define RECV_MOVE_ITEM 0x42
#define RECV_USE_ITEM 0x43
#define RECV_CANCEL_ITEM 0x44
#define RECV_USE_SUMMON_BAG 0x46
#define RECV_PET_FEED 0x47
#define RECV_USE_CASH_ITEM 0x49
#define RECV_USE_SKILLBOOK 0x4b
#define RECV_USE_TELEPORT_ROCK 0x4d // Only the one in your Use
#define RECV_USE_RETURN_SCROLL 0x4e
#define RECV_USE_SCROLL 0x4f
#define RECV_ADD_STAT 0x50
#define RECV_HEAL_PLAYER 0x51
#define RECV_ADD_SKILL 0x52
#define RECV_USE_SKILL 0x53
#define RECV_CANCEL_SKILL 0x54
#define RECV_SPECIAL_SKILL 0x55 // Chakra, Pierce, Monster Magnet, Big Bang
#define RECV_DROP_MESO 0x56
#define RECV_FAME 0x57
#define RECV_GET_PLAYER_INFO 0x59
#define RECV_PET_SUMMON 0x5a
#define RECV_CHANGE_MAP_SPECIAL 0x5c
#define RECV_TELEPORT_ROCK_FUNCTION 0x5e
#define RECV_GET_QUEST 0x62
#define RECV_SKILL_MACRO 0x65
#define RECV_GROUP_CHAT 0x6b // Party, buddy and guild chat
#define RECV_COMMAND 0x6c // / commands like "/find"
#define RECV_PLAYER_ROOM_ACTION 0x6f // Trades, player shops, minigames, etc.
#define RECV_PARTY_ACTION 0x70
#define RECV_BUDDYLIST 0x76
#define RECV_KEYMAP 0x7b
#define RECV_MTS_BUTTON 0x87
#define RECV_PET_MOVE 0x8c
#define RECV_PET_CHAT 0x8d
#define RECV_PET_COMMAND 0x8e
#define RECV_PET_LOOT 0x8f
#define RECV_MOVE_SUMMON 0x94
#define RECV_ATTACK_SUMMON 0x95
#define RECV_DAMAGE_SUMMON 0x96
#define RECV_CONTROL_MOB 0x9d
#define RECV_FRIENDLY_MOB_DAMAGED 0xa1
#define RECV_MOB_BOMB_EXPLOSION 0xa2
#define RECV_MOB_DAMAGE_MOB 0xa3 // For Hypnotize
#define RECV_ANIMATE_NPC 0xa6
#define RECV_LOOT_ITEM 0xab
#define RECV_HIT_REACTOR 0xae