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

const header_t CMSG_CLIENT_VERSION = 0x14;
const header_t CMSG_AUTHENTICATION = 0x15;
const header_t CMSG_WORLD_LIST_REFRESH = 0x18; // Click back after select channel
const header_t CMSG_PLAYER_LIST = 0x19;
const header_t CMSG_WORLD_STATUS = 0x1a;
const header_t CMSG_ACCOUNT_GENDER = 0x1c;
const header_t CMSG_PIN = 0x1d;
const header_t CMSG_REGISTER_PIN = 0x1e;
const header_t CMSG_WORLD_LIST = 0x1f;
const header_t CMSG_PLAYER_GLOBAL_LIST = 0x21;
const header_t CMSG_PLAYER_GLOBAL_LIST_CHANNEL_CONNECT = 0x22;
const header_t CMSG_REQUEST_NAME_CHANGE = 0x24;
const header_t CMSG_REQUEST_CHARACTER_TRANSFER = 0x26;
const header_t CMSG_CHANNEL_CONNECT = 0x27;
const header_t CMSG_PLAYER_LOAD = 0x28;
const header_t CMSG_PLAYER_NAME_CHECK = 0x29;
const header_t CMSG_PLAYER_CREATE = 0x2a;
const header_t CMSG_PLAYER_DELETE = 0x2d; // 0x2b = without pic?
const header_t CMSG_PONG = 0x2e;
const header_t CMSG_CLIENT_ERROR = 0x2f;
const header_t CMSG_LOGIN_RETURN = 0xF1d;
const header_t CMSG_CHANNEL_CONNECT_PIC = 0x33;
const header_t CMSG_PLAYER_GLOBAL_LIST_CHANNEL_CONNECT_PIC = 0x35;
const header_t CMSG_CLIENT_STARTED = 0x38;
const header_t CMSG_BACKUP_PACKET = 0x3b;
const header_t CMSG_REQUEST_SPECIAL_CHAR_CREATION = 0x3e; // For enabling 'special character creation'
const header_t CMSG_PLAYER_CREATE_SPECIAL = 0x3f;
const header_t CMSG_MAP_CHANGE = 0x42;
const header_t CMSG_CHANNEL_CHANGE = 0xF26;
const header_t CMSG_CASH_SHOP = 0xF27;
const header_t CMSG_PLAYER_MOVE = 0x49;
const header_t CMSG_CHAIR = 0x4B;
const header_t CMSG_USE_CHAIR = 0x4C;
const header_t CMSG_ATTACK_MELEE = 0x4D;
const header_t CMSG_ATTACK_RANGED = 0x4E;
const header_t CMSG_ATTACK_MAGIC = 0x4F;
const header_t CMSG_ATTACK_ENERGY_CHARGE = 0x50;
const header_t CMSG_PLAYER_DAMAGE = 0x52;
const header_t CMSG_PLAYER_CHAT = 0x54;
const header_t CMSG_CHALKBOARD = 0xF31;
const header_t CMSG_EMOTE = 0x56;
const header_t CMSG_ITEM_EFFECT = 0x57;
const header_t CMSG_REVIVE_EFFECT = 0x58;
const header_t CMSG_MONSTER_BOOK = 0xF38;
const header_t CMSG_NPC_TALK = 0x66;
const header_t CMSG_NPC_TALK_CONT = 0x68; // i.e. clicking next on NPC
const header_t CMSG_NPC_TALK_CONT_2 = 0x69;
const header_t CMSG_SHOP = 0xF3c;
const header_t CMSG_STORAGE = 0xF3d;
const header_t CMSG_ITEM_MOVE = 0x75;
const header_t CMSG_ITEM_USE = 0x79;
const header_t CMSG_ITEM_CANCEL = 0x7a;
const header_t CMSG_SUMMON_BAG_USE = 0x7c;
const header_t CMSG_PET_FOOD_USE = 0x7d;
const header_t CMSG_USE_SCRIPT_ITEM = 0x7F; // For items that runs scripts
const header_t CMSG_CASH_ITEM_USE = 0x85;
const header_t CMSG_SKILLBOOK_USE = 0x88;
const header_t CMSG_TELEPORT_ROCK_USE = 0x8F; // Only the one in your Use
const header_t CMSG_TOWN_SCROLL_USE = 0x90;
const header_t CMSG_SCROLL_USE = 0x91;
const header_t CMSG_STAT_ADDITION = 0x9A;
const header_t CMSG_MULTI_STAT_ADDITION = 0x9B; // The auto-assign button
const header_t CMSG_PLAYER_HEAL = 0x9C;
const header_t CMSG_SKILL_ADD = 0x9F;
const header_t CMSG_SKILL_USE = 0xA0;
const header_t CMSG_SKILL_CANCEL = 0xA1;
const header_t CMSG_SPECIAL_SKILL = 0xF5c; // Chakra, Pierce, Monster Magnet, Big Bang
const header_t CMSG_DROP_MESOS = 0xF5d;
const header_t CMSG_FAME = 0xF5e;
const header_t CMSG_PLAYER_INFO = 0xA5;
const header_t CMSG_PET_SUMMON = 0xA7;
const header_t CMSG_MAP_CHANGE_SPECIAL = 0xAF;
const header_t CMSG_MAP_TELEPORT = 0xF64;
const header_t CMSG_TELEPORT_ROCK = 0xB2;
const header_t CMSG_QUEST_OBTAIN = 0xB8;
const header_t CMSG_MACRO_LIST = 0xBC;
const header_t CMSG_USE_REWARD_ITEM = 0xF6f;
const header_t CMSG_MESSAGE_GROUP = 0xF75; // Party, buddy and guild chat
const header_t CMSG_COMMAND = 0xDE; // / commands like "/find"
const header_t CMSG_PLAYER_ROOM = 0xF79; // Trades, player shops, minigames, etc.
const header_t CMSG_PARTY = 0xE2;
const header_t CMSG_ADMIN_COMMAND = 0xE9;
const header_t CMSG_ADMIN_COMMAND_LOG = 0xEA; // '/(command) (parameters)'
const header_t CMSG_BUDDY = 0xEB;
const header_t CMSG_SEND_NOTE = 0xF81;
const header_t CMSG_MYSTIC_DOOR_ENTRY = 0xF83;
const header_t CMSG_KEYMAP = 0xF1;
const header_t CMSG_MTS = 0xF9a;
const header_t CMSG_ARAN_COMBO_COUNTER = 0x10E;
const header_t CMSG_PET_MOVEMENT = 0x123;
const header_t CMSG_PET_CHAT = 0x124;
const header_t CMSG_PET_COMMAND = 0x125;
const header_t CMSG_PET_LOOT = 0x126;
const header_t CMSG_PET_CONSUME_POTION = 0x127;
const header_t CMSG_SUMMON_MOVEMENT = 0xFa9;
const header_t CMSG_SUMMON_ATTACK = 0xFaa;
const header_t CMSG_SUMMON_DAMAGE = 0xFab;
const header_t CMSG_MOB_CONTROL = 0x14F;
const header_t CMSG_PLAYER_MOB_DISTANCE = 0x150;
const header_t CMSG_FRIENDLY_MOB_DAMAGE = 0xFb6;
const header_t CMSG_MOB_EXPLOSION = 0xFb7;
const header_t CMSG_MOB_TURNCOAT_DAMAGE = 0xFb8; // For Hypnotize
const header_t CMSG_NPC_ANIMATE = 0x15F;
const header_t CMSG_ITEM_LOOT = 0x164;
const header_t CMSG_REACTOR_HIT = 0xFc3;
const header_t CMSG_REACTOR_TOUCH = 0xFc4;
const header_t CMSG_CASHSHOP_CHECK_CASH = 0xFda;
const header_t CMSG_CASHSHOP_REDEEM_COUPON = 0xFdc;
const header_t CMSG_CASHSHOP_OPERATION = 0xFdb;
const header_t CMSG_ADMIN_MESSENGER = 0xFeb;
const header_t CMSG_HAMMER = 0xFf8;