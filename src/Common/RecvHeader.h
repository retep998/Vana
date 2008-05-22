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

#define RECV_PONG 0x0a

//Login
#define RECV_LOGIN_INFO 0x1b
#define RECV_CHANNEL_SELECT 0x19
#define RECV_WORLD_SELECT 0x13
#define RECV_LOGIN_PROCESS 0x03
#define RECV_SHOW_WORLD 0x18
#define RECV_SHOW_WORLD2 0x02 // Click back after select channel
#define RECV_GET_CHANNEL_SERVER_INFO 0x16
#define RECV_CHECK_CHAR_NAME 0x09
#define RECV_CREATE_CHAR 0x0e
#define RECV_DELETE_CHAR 0x0f
#define RECV_RETURN_TO_LOGIN 0x1a
#define RECV_SET_GENDER 0x17
#define RECV_REGISTER_PIN 0x05

//Channel
#define RECV_CHANNEL_LOGIN 0x14
#define RECV_NPC_TALK_CONT 0x21 // i.e. clicking next on NPC
#define RECV_SHOP_ENTER 0x22
#define RECV_NPC_TALK 0x23
#define RECV_CHANGE_CHANNEL 0x27
#define RECV_DAMAGE_PLAYER 0x2a
#define RECV_STOP_CHAIR 0x2b
#define RECV_CHAT 0x2c
#define RECV_USE_CHAIR 0x2d
#define RECV_DAMAGE_MOB_SPELL 0x2e
#define RECV_CHANGE_MAP 0x2f
#define RECV_MOVE_PLAYER 0x35
#define RECV_DAMAGE_MOB_RANGED 0x36
#define RECV_GET_PLAYER_INFO 0x44
#define RECV_CHANGE_MAP_SPECIAL 0x47
#define RECV_CANCEL_ITEM 0x49
#define RECV_USE_SUMMON_BAG 0x4b
#define RECV_ADD_SKILL 0x4d
#define RECV_CANCEL_SKILL 0x4e
#define RECV_USE_SKILL 0x51
#define RECV_USE_MEGAPHONE 0x53
#define RECV_USE_SKILLBOOK 0x55
#define RECV_COMMAND 0x58 // / commands like "/find"
#define RECV_DAMAGE_MOB 0x59
#define RECV_FACE_EXPERIMENT 0x5c
#define RECV_USE_ITEM_EFFECT 0x5d
#define RECV_MOVE_ITEM 0x62
#define RECV_USE_ITEM 0x63
#define RECV_USE_RETURN_SCROLL 0x64
#define RECV_USE_SCROLL 0x65
#define RECV_ADD_STAT 0x66
#define RECV_HEAL_PLAYER 0x67
#define RECV_DROP_MESO 0x68
#define RECV_FAME 0x69
#define RECV_GET_QUEST 0x6b
#define RECV_KEYMAP 0x75
#define RECV_LOOT_ITEM 0x89
#define RECV_CONTROL_MOB 0x9d
#define RECV_CONTROL_MOB_SKILL 0xa0
