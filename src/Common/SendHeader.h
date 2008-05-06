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

#define SEND_IV 0x0d
#define MAPLE_VERSION 53

// Login
#define SEND_LOGIN_INFO_REPLY 0x00
#define SEND_LOGIN_PROCESS 0x0d
#define SEND_PIN_ASSIGNED 0x11
#define SEND_SHOW_WORLD 0x05
#define SEND_SHOW_CHANNEL 0x12
#define SEND_CHANNEL_SELECT 0x10
#define SEND_SHOW_CHARACTERS 0x13
#define SEND_SHOW_CHARACTER 0x07
#define SEND_CHECK_NAME 0x06
#define SEND_DELETE_CHAR 0x08
#define SEND_CHANNEL_SERVER_INFO 0x04
#define SEND_RETURN_TO_LOGIN 0x13

// Channel
#define SEND_MOVE_ITEM 0x18
#define SEND_SHOW_MESSENGER 0x19 // Also known as avatar megaphone
#define SEND_FINISH_QUEST 0x1f
#define SEND_UPDATE_STAT 0x23
#define SEND_CANCEL_SKILL 0x24
#define SEND_PLAYER_INFO 0x2a
#define SEND_ADD_SKILL 0x2f
#define SEND_NOTICE 0x2d
#define SEND_FAME 0x31
#define SEND_NOTE 0x32 // Bottom-right corner notice
#define SEND_USE_SKILL 0x3a
#define SEND_CHANGE_MAP 0x4d
#define SEND_MAKE_APPLE 0x5b // Wear apple suit
#define SEND_COMMAND_RESPOND 0x5e // Respond to / commands i.e. "/find"
#define SEND_SHOW_PLAYER 0x65
#define SEND_STOP_CHAIR 0x66
#define SEND_GAIN_ITEM 0x67 // Gain item notice in chat
#define SEND_UPDATE_QUEST 0x6c
#define SEND_REMOVE_PLAYER 0x70
#define SEND_CHAT 0x71
#define SEND_USE_SCROLL 0x7a
#define SEND_MOVE_PLAYER 0x84
#define SEND_SHOW_SKILL 0x85
#define SEND_SHOW_OTHERS_SKILL 0x86 // From other player
#define SEND_DAMAGE_MOB 0x87
#define SEND_CANCEL_OTHERS_BUFF 0x8a // From other player
#define SEND_FACE_EXPERIMENT 0x8c
#define SEND_DAMAGE_MOB_RANGED 0x8d
#define SEND_DAMAGE_PLAYER 0x89
#define SEND_SIT_CHAIR 0x91
#define SEND_UPDATE_CHAR_LOOK 0x92
#define SEND_DAMAGE_MOB_SKILL 0x93
#define SEND_SHOW_MOB 0x96
#define SEND_MOVE_MOB 0x97
#define SEND_SHOW_MOB_HP 0x98
#define SEND_MOVE_MOB2 0x9c
#define SEND_CONTROL_MOB 0xa4
#define SEND_KILL_MOB 0xa5
#define SEND_SHOW_NPC 0xa7
#define SEND_SHOW_NPC2 0xac
#define SEND_DROP_ITEM 0xb8
#define SEND_TAKE_DROP 0xb9
#define SEND_NPC_TALK 0xc2
#define SEND_SHOP_OPEN 0xd6
#define SEND_SHOP_BOUGHT 0xd7
#define SEND_KEYMAP 0xf6
