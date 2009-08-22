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

#define SEND_LOGIN_INFO_REPLY 0x00
#define SEND_SHOW_CHANNEL 0x03
#define SEND_GENDER_DONE 0x04
#define SEND_LOGIN_PROCESS 0x06
#define SEND_PIN_ASSIGNED 0x07
#define SEND_VIEW_ALL_CHAR 0x08
#define SEND_SHOW_WORLD 0x0a
#define SEND_SHOW_CHARACTERS 0x0b
#define SEND_CHANNEL_SERVER_INFO 0x0c
#define SEND_CHECK_NAME 0x0d
#define SEND_SHOW_CHARACTER 0x0e
#define SEND_DELETE_CHAR 0x0f
#define SEND_CHANGE_CHANNEL 0x10
#define SEND_PING 0x11
#define SEND_CHANNEL_SELECT 0x14
#define SEND_RELOG_RESPONSE 0x16
#define SEND_MOVE_ITEM 0x1a
#define SEND_UPDATE_INVENTORY_SLOTS 0x1b
#define SEND_UPDATE_STAT 0x1c
#define SEND_USE_SKILL 0x1d
#define SEND_CANCEL_SKILL 0x1e
#define SEND_ADD_SKILL 0x21
#define SEND_FAME 0x23
#define SEND_NOTE 0x24 // Bottom-right corner notice
#define SEND_TELEPORT_ROCK_FUNCTION 0x27 // Teleport Rock, VIP Teleport Rock
#define SEND_FINISH_QUEST 0x2e
#define SEND_USE_SKILLBOOK 0x30 // Skillbooks
#define SEND_SHOW_ITEM_EFFECT 0x31
#define SEND_PLAYER_INFO 0x3a
#define SEND_PARTY_ACTION 0x3b
#define SEND_BUDDYLIST 0x3c
#define SEND_NOTICE 0x41
#define SEND_MONSTERBOOK_ADD 0x4f
#define SEND_MONSTERBOOK_CHANGE_COVER 0x50
#define SEND_SHOW_MESSENGER 0x67 // Also known as avatar megaphone
#define SEND_SKILL_MACRO 0x71
#define SEND_CHANGE_MAP 0x72
#define SEND_FORCE_MAP_EQUIP 0x79 // Wear apple suit on training maps, all equips except bathrobe off for Showa spa, etc.
#define SEND_GROUP_CHAT 0x7a // Groups like party, buddies and guild
#define SEND_COMMAND_RESPOND 0x7b // Respond to / commands i.e. "/find"
#define SEND_MAP_EFFECT 0x7e // Change Music/Boss HP Bars
#define SEND_GM_FUNCTION 0x84 // Hide, other miscellaneous junk
#define SEND_GM_EVENT_INSTRUCTIONS 0x86 // GM event instructions - Ola Ola, etc.
#define SEND_TIME 0x87 // For places with clock i.e. ship station
#define SEND_STOP_TIME 0x8e // Makes timer disappear from map
#define SEND_SHOW_PLAYER 0x91
#define SEND_REMOVE_PLAYER 0x92
#define SEND_CHAT 0x93
#define SEND_USE_SCROLL 0x98
#define SEND_PET_SUMMONED 0x99
#define SEND_PET_SHOW_MOVING 0x9b
#define SEND_PET_SHOW_CHAT 0x9c
#define SEND_PET_NAME_CHANGE 0x9d
#define SEND_PET_SHOW 0x9e
#define SEND_PET_ANIMATION 0x9f
#define SEND_SPAWN_SUMMON 0xa0
#define SEND_REMOVE_SUMMON 0xa1
#define SEND_MOVE_SUMMON 0xa2
#define SEND_ATTACK_SUMMON 0xa3
#define SEND_DAMAGE_SUMMON 0xa4
#define SEND_MOVE_PLAYER 0xa7
#define SEND_ATTACK_MELEE 0xa8
#define SEND_ATTACK_RANGED 0xa9
#define SEND_ATTACK_SPELL 0xaa
#define SEND_SPECIAL_SKILL 0xac
#define SEND_SPECIAL_SKILL_END 0xad
#define SEND_DAMAGE_PLAYER 0xae
#define SEND_FACE_EXPRESSION 0xaf
#define SEND_SIT_CHAIR 0xb2
#define SEND_UPDATE_CHAR_LOOK 0xb3
#define SEND_SHOW_SKILL 0xb4
#define SEND_SHOW_OTHERS_SKILL 0xb5 // From other player
#define SEND_CANCEL_OTHERS_BUFF 0xb6 // From other player
#define SEND_PARTY_HP_BAR 0xb7
#define SEND_CHAIR_ACTION 0xbb
#define SEND_GAIN_ITEM 0xbc // Gain item notice in chat
#define SEND_MAP_TELEPORT 0xbd // Instantaneous transfer
#define SEND_UPDATE_QUEST 0xc1
#define SEND_INSTRUCTION_BUBBLE 0xc4
#define SEND_COOLDOWN 0xce // Skill cooldown
#define SEND_SHOW_MOB 0xd0
#define SEND_KILL_MOB 0xd1
#define SEND_CONTROL_MOB 0xd2
#define SEND_MOVE_MOB 0xd3
#define SEND_MOVE_MOB_RESPONSE 0xd4
#define SEND_APPLY_MOB_STATUS 0xd6
#define SEND_REMOVE_MOB_STATUS 0xd7
#define SEND_DAMAGE_MOB 0xda
#define SEND_SHOW_MOB_HP 0xde
#define SEND_SHOW_DRAGGED 0xdf // New in .56, Monster Magnet'd mobs get their own packet
#define SEND_SHOW_NPC 0xe3
#define SEND_CONTROL_NPC 0xe5
#define SEND_ANIMATE_NPC 0xe6
#define SEND_DROP_ITEM 0xee
#define SEND_TAKE_DROP 0xef
#define SEND_SPAWN_MIST 0xf3
#define SEND_REMOVE_MIST 0xf4
#define SEND_TRIGGER_REACTOR 0xf7
#define SEND_SHOW_REACTOR 0xf9
#define SEND_DESTROY_REACTOR 0xfa
#define SEND_NPC_TALK 0x10e
#define SEND_SHOP_OPEN 0x10f
#define SEND_SHOP_BOUGHT 0x110
#define SEND_STORAGE_ACTION 0x113
#define SEND_PLAYER_ROOM_ACTION 0x118 // Trades, player shops, minigames
#define SEND_KEYMAP 0x12a
#define SEND_SHOW_MAPLETV 0x130
#define SEND_END_MAPLETV 0x131
