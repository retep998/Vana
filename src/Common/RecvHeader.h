/*
Copyright (C) 2008 Vana Development Team

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

#define RECV_PONG 0x18

//Login
#define RECV_LOGIN_INFO 0x01
#define RECV_SHOW_WORLD2 0x04 // Click back after select channel
#define RECV_CHANNEL_SELECT 0x05
#define RECV_WORLD_SELECT 0x06
#define RECV_SET_GENDER 0x08
#define RECV_LOGIN_PROCESS 0x09
#define RECV_REGISTER_PIN 0x0a
#define RECV_SHOW_WORLD 0x0b
#define RECV_GET_CHANNEL_SERVER_INFO 0x13
#define RECV_CHECK_CHAR_NAME 0x15
#define RECV_CREATE_CHAR 0x16
#define RECV_DELETE_CHAR 0x17
#define RECV_RELOG 0x1c

//Channel
#define RECV_CHANNEL_LOGIN 0x14
#define RECV_CHANGE_MAP 0x23
#define RECV_CHANGE_CHANNEL 0x24
#define RECV_MOVE_PLAYER 0x26
#define RECV_STOP_CHAIR 0x27
#define RECV_USE_CHAIR 0x28
#define RECV_DAMAGE_MOB 0x29
#define RECV_DAMAGE_MOB_RANGED 0x2a
#define RECV_DAMAGE_MOB_SPELL 0x2b
#define RECV_DAMAGE_PLAYER 0x2d
#define RECV_CHAT 0x2e
#define RECV_FACE_EXPERIMENT 0x30
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
#define RECV_PET_SUMMON 0x5A
#define RECV_CHANGE_MAP_SPECIAL 0x5c
#define RECV_GET_QUEST 0x62
#define RECV_SKILL_MACRO 0x65
#define RECV_GROUP_CHAT 0x6a // Party, buddy and guild chat
#define RECV_COMMAND 0x6b // / commands like "/find"
#define RECV_SHOP_ACTION 0x6e // Trades, player shops, etc.
#define RECV_PARTY_ACTION 0x6f
#define RECV_KEYMAP 0x7a
#define RECV_PET_MOVE 0x89
#define RECV_PET_CHAT 0x8a
#define RECV_PET_COMMAND 0x8b
#define RECV_PET_LOOT 0x8c
#define RECV_LOOT_ITEM 0xa8
#define RECV_HIT_REACTOR 0xab
#define RECV_CONTROL_MOB 0x9a
