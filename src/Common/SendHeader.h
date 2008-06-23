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
#define MAPLE_VERSION 56

#define SEND_PING 0x11

// Login
#define SEND_LOGIN_INFO_REPLY 0x00
#define SEND_LOGIN_PROCESS 0x06
#define SEND_PIN_ASSIGNED 0x07
#define SEND_GENDER_DONE 0x04
#define SEND_SHOW_WORLD 0x0a
#define SEND_SHOW_CHANNEL 0x03
#define SEND_CHANNEL_SELECT 0x14
#define SEND_SHOW_CHARACTERS 0x0b
#define SEND_SHOW_CHARACTER 0x0e
#define SEND_CHECK_NAME 0x0d
#define SEND_DELETE_CHAR 0x0f
#define SEND_CHANNEL_SERVER_INFO 0x0c

// Channel
#define SEND_CHANGE_CHANNEL 0x10
#define SEND_MOVE_ITEM 0x1a
#define SEND_SHOW_MESSENGER 0x54 // Also known as avatar megaphone
 #define SEND_FINISH_QUEST 0x1f
#define SEND_UPDATE_STAT 0x1c
 #define SEND_CANCEL_SKILL 0x24
#define SEND_PLAYER_INFO 0x3a
 #define SEND_ADD_SKILL 0x2f
#define SEND_NOTICE 0x41
#define SEND_FAME 0x23
#define SEND_NOTE 0x24 // Bottom-right corner notice
 #define SEND_USE_SKILLBOOK 0x34 // Skillbooks
#define SEND_PARTY_ACTION 0x3b
 #define SEND_USE_SKILL 0x3b
#define SEND_CHANGE_MAP 0x5b
 #define SEND_MAP_EFFECT 0x54 // Change Music/Boss HP Bars
 #define SEND_USE_WEATHER 0x55 // Weather effects
 #define SEND_GROUP_CHAT 0x56 // Groups like party, buddies and guild
 #define SEND_MAKE_APPLE 0x5c // Wear apple suit
 #define SEND_COMMAND_RESPOND 0x5f // Respond to / commands i.e. "/find"
#define SEND_TIME 0x6d // For places with clock i.e. ship station
#define SEND_SHOW_PLAYER 0x77
#define SEND_STOP_CHAIR 0x9f
#define SEND_GAIN_ITEM 0x24 // Gain item notice in chat
 #define SEND_UPDATE_QUEST 0x6d
#define SEND_REMOVE_PLAYER 0x78
#define SEND_CHAT 0x79
 #define SEND_USE_SCROLL 0x7b
#define SEND_MOVE_PLAYER 0x8c
 #define SEND_SHOW_SKILL 0x86
 #define SEND_SHOW_OTHERS_SKILL 0x87 // From other player
 #define SEND_DAMAGE_MOB 0x88
 #define SEND_CANCEL_OTHERS_BUFF 0x8b // From other player
#define SEND_FACE_EXPERIMENT 0x94
 #define SEND_DAMAGE_MOB_RANGED 0x8e
 #define SEND_SHOW_ITEM_EFFECT 0x8f
 #define SEND_DAMAGE_PLAYER 0x8a
 #define SEND_SPECIAL_SKILL_END 0x89
 #define SEND_SPECIAL_SKILL 0x91
#define SEND_SIT_CHAIR 0x96
 #define SEND_UPDATE_CHAR_LOOK 0x93
 #define SEND_DAMAGE_MOB_SPELL 0x94
#define SEND_SHOW_MOB 0xad
 #define SEND_MOVE_MOB 0x98
 #define SEND_SHOW_MOB_HP 0x99
 #define SEND_SHOW_MAGNET 0x9a
 #define SEND_MOVE_MOB2 0x9d
#define SEND_CONTROL_MOB 0xaf
 #define SEND_KILL_MOB 0xa6
#define SEND_SHOW_NPC 0xc0
#define SEND_SHOW_NPC2 0xc2
#define SEND_SHOW_REACTOR 0xd6
#define SEND_TRIGGER_REACTOR 0xd4
#define SEND_DESTROY_REACTOR 0xd7
#define SEND_DROP_ITEM 0xcb
#define SEND_TAKE_DROP 0xcc
#define SEND_NPC_TALK 0xea
#define SEND_SHOP_OPEN 0xeb
#define SEND_SHOP_BOUGHT 0xec
#define SEND_KEYMAP 0x104
