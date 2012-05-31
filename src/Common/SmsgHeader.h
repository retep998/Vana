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

const header_t SMSG_AUTHENTICATION = 0x00;
// 0x01 = Trial Auth
const header_t SMSG_AUTHENTICATION_2 = 0x02;
const header_t SMSG_WORLD_STATUS = 0x03;
const header_t SMSG_ACCOUNT_GENDER_DONE = 0x04;
const header_t SMSG_MESSAGE_TRY_AGAIN = 0x05; // Shows a popup with "Try again!" :P
const header_t SMSG_PIN = 0x06;
const header_t SMSG_PIN_ASSIGNED = 0x07;
const header_t SMSG_PLAYER_GLOBAL_LIST = 0x08;
// 0x09 = Errors + connect to server?
const header_t SMSG_WORLD_LIST = 0x0a;
const header_t SMSG_PLAYER_LIST = 0x0b;
const header_t SMSG_CHANNEL_CONNECT = 0x0c;
const header_t SMSG_PLAYER_NAME_CHECK = 0x0d;
const header_t SMSG_PLAYER_CREATE = 0x0e;
const header_t SMSG_PLAYER_DELETE = 0x0f;
const header_t SMSG_CHANNEL_CHANGE = 0x10;
const header_t SMSG_PING = 0x11;
const header_t SMSG_LOGIN_RETURN = 0x18;
const header_t SMSG_LAST_SELECTED_WORLD = 0x1B;
const header_t SMSG_RECOMMENDED_WORLDS = 0x1C;
const header_t SMSG_PLAYER_CREATE_SPECIAL_ENABLE = 0x1D; // 0x1E?
const header_t SMSG_INVENTORY_ITEM_MOVE = 0x20;
const header_t SMSG_INVENTORY_SLOT_UPDATE = 0x21;
const header_t SMSG_PLAYER_UPDATE = 0x22;
const header_t SMSG_SKILL_USE = 0x23;
const header_t SMSG_SKILL_CANCEL = 0x24;
const header_t SMSG_FORCED_STATS_SET = 0x25; // This beauty can change your stats and it won't change back until you restart your client or send the packet 0xF20 :D
const header_t SMSG_FORCED_STATS_RESET = 0x26; // Bui thinks it's from Balrog (setting your stats to 1 thing)
const header_t SMSG_SKILL_ADD = 0x27;
const header_t SMSG_FAME = 0x2B;
const header_t SMSG_NOTICE = 0x2C; // Bottom-right corner notice
const header_t SMSG_CLIENT_UPDATE = 0x2D; // This thing takes the client to the gamepatches page @ the nexon page
const header_t SMSG_NOTE = 0x2E; // The real note system
const header_t SMSG_TELEPORT_ROCK = 0x2F; // Teleport Rock, VIP Teleport Rock
const header_t SMSG_ANTI_MACRO = 0x30;
const header_t SMSG_REPORT_MORE = 0x31; // More report shit >_>
const header_t SMSG_QUEST_COMPLETED = 0x37;
const header_t SMSG_PLAYER_STORE_MESSAGE = 0x38;
const header_t SMSG_SKILLBOOK = 0x39;
const header_t SMSG_REPORT_USER = 0x43;
const header_t SMSG_REACHED_TRADE_MESOS_LIMIT = 0x44; // "Players that are Level 15 and below may only trade 1 million mesos per day. You have reached the limit today, please try again tomorrow"
const header_t SMSG_PLAYER_INFO = 0x49;
const header_t SMSG_PARTY = 0x4A;
const header_t SMSG_BUDDY = 0x4B;
const header_t SMSG_GUILD = 0x52;
const header_t SMSG_ALLIANCE = 0x53;
const header_t SMSG_PORTAL_ACTION = 0x54;
const header_t SMSG_MESSAGE = 0x58;
const header_t SMSG_OWL_OF_MINERVA = 0x5A;
const header_t SMSG_WEDDING_PROPOSAL_WHISHLIST = 0xF45; // Holds both whishlist and proposal
const header_t SMSG_WEDDING = 0xF46; // Holds wedding and proposal things like "You are now engaged."...
const header_t SMSG_WEDDING_MORE = 0xF47; // Holds wedding stuff too.....
const header_t SMSG_YELLOW_MESSAGE = 0x61; // Used by bosses like Stumpy and Mano
const header_t SMSG_ARIANT_CATCH_RESULT = 0x63; // Used by the Ariant match when using the Element Rock
const header_t SMSG_PLAYER_NPC_SHOW = 0x65;
const header_t SMSG_MONSTER_BOOK_ADD = 0x68;
const header_t SMSG_MONSTER_BOOK_COVER = 0x69;
const header_t SMSG_PLAYER_LEVEL_UPDATE = 0x85; // Byte after it: 2 = guild, everything else = family
const header_t SMSG_PLAYER_MARRIED_UPDATE = 0x86; // Byte after it: 0 = Guild, 1 = Family, else = nothing (Shows "<'type'[name] is now married. Please congratualte them." lol)
const header_t SMGS_PLAYER_JOB_UPDATE = 0x87; // Byte after it: 0 = Guild, 1 = Family, else = nothing  (Shows "<'type'[name] has advanced to a(an) [jobid].")
const header_t SMSG_POPUP_MESSAGE = 0x89; // I don't know where it is used, but it's like the admin warning? Might be part of megaphone
const header_t SMSG_MEGAPHONE_MESSAGES = 0x8A; // Wait line shit and such? Megaphone message too... (when byte is not 0xF46 or 0xF47)
const header_t SMSG_AVATAR_MEGAPHONE = 0x8B;
const header_t SMSG_NAME_CANCELLATION_REQUEST = 0x8D; // It's gone before you know it (when it was successfull)
const header_t SMSG_TRANSFER_CANCELLATION_REQUEST = 0x8E; // It's gone before you know it (when it was successfull)
const header_t SMSG_GM_BAN = 0xF6c; // GMMapleStory with it's HACK reason bans
const header_t SMSG_SILVER_BOX_REWARD = 0xF6d; // Silver Box event?
const header_t SMSG_NEWYEAR_CARDS = 0xF6e; // I want to code this!
const header_t SMSG_RANDOM_TRANSFORMATION_POTION = 0xF6f;
const header_t SMSG_NAME_CANCELLATION_REQUEST_NOT_REQUESTED = 0xF70; // 1 thing, 1 whole header
const header_t SMSG_FAILED_RECEIVING_REWARD = 0xB7;
const header_t SMSG_MACRO_LIST = 0xB8;
const header_t SMSG_CHANGE_MAP = 0xB9;
const header_t SMSG_CONNECT_ITC = 0xBA;
const header_t SMSG_CONNECT_CASHSHOP = 0xBB;
// 0xBC = Back Effect: BYTE INT BYTE INT
// 0xBD = Map Object Visible: BYTE foreach [STRING BYTE]
const header_t SMSG_PORTAL_BLOCKED = 0xc0;
const header_t SMSG_CHANNEL_BLOCKED = 0xc1; // For messages like 'This server is unavailable'
const header_t SMSG_MAP_FORCE_EQUIPMENT = 0xc2; // Wear apple suit on training maps, all equips except bathrobe off for Showa spa, etc.
const header_t SMSG_MESSAGE_GROUP = 0xc3; // Groups like party, buddies and guild
const header_t SMSG_COMMAND = 0xc4; // Respond to / commands i.e. "/find"
const header_t SMSG_SPOUSE_CHAT = 0xc5; // Some weird colored chat... Sources say Spouse chat.
const header_t SMSG_MESSAGE_CANNOT_USE = 0xc6; // "You can't use it here in this map." Note: it needs a byte after it of '0'.
const header_t SMSG_MAP_EFFECT = 0xc7; // Change Music/Boss HP Bars
const header_t SMSG_MAP_WEATHER_EFFECT = 0xCB; // Used by weather NX items
const header_t SMSG_CASH_SONG = 0xCC; // Used for cash items only, plays a song... :D
const header_t SMSG_GM = 0xCD; // Hide, other miscellaneous junk
const header_t SMSG_OX_QUIZ_QUESTION = 0xCE; // Shows the OX quiz question (with a timer)
const header_t SMSG_EVENT_INSTRUCTION = 0xF86; // GM event instructions - Ola Ola, etc.
const header_t SMSG_TIMER = 0xF87; // For places with clock i.e. ship station
const header_t SMSG_SHIP = 0xF88; // Showing or removing (balrog) ships. Only works for maps with ships (else you'll only hear the horn)
const header_t SMSG_GM_WARNING = 0xF8c; // Shows a popup with the message from a GM/an Admin
const header_t SMSG_TIMER_OFF = 0xF8e; // Makes timer disappear from map
const header_t SMSG_MAP_PLAYER_SPAWN = 0xE6;
const header_t SMSG_MAP_PLAYER_DESPAWN = 0xE7;
const header_t SMSG_PLAYER_CHAT = 0xE8;
const header_t SMSG_PLAYER_CHAT_2 = 0xE9;
const header_t SMSG_CHALKBOARD = 0xEA;
const header_t SMSG_SCROLL_USE = 0xF98;
const header_t SMSG_PET_SUMMON = 0x104;
const header_t SMSG_PET_DESUMMON = 0x105;
const header_t SMSG_PET_MOVEMENT = 0x107;
const header_t SMSG_PET_MESSAGE = 0x108;
const header_t SMSG_PET_NAME_CHANGE = 0x109;
const header_t SMSG_PET_SHOW = 0x10A;
const header_t SMSG_PET_ANIMATION = 0x10B;
const header_t SMSG_SUMMON_SPAWN = 0xFa0;
const header_t SMSG_SUMMON_DESPAWN = 0xFa1;
const header_t SMSG_SUMMON_MOVEMENT = 0xFa2;
const header_t SMSG_SUMMON_ATTACK = 0xFa3;
const header_t SMSG_SUMMON_DAMAGE = 0xFa4;
const header_t SMSG_PLAYER_MOVEMENT = 0x11C;
const header_t SMSG_ATTACK_MELEE = 0x11E;
const header_t SMSG_ATTACK_RANGED = 0x11F;
const header_t SMSG_ATTACK_MAGIC = 0x120;
const header_t SMSG_ATTACK_ENERGYCHARGE = 0x121;
const header_t SMSG_SPECIAL_SKILL = 0xFac;
const header_t SMSG_SPECIAL_SKILL_END = 0xFad;
const header_t SMSG_PLAYER_DAMAGE = 0xFae;
const header_t SMSG_EMOTE = 0x126;
const header_t SMSG_ITEM_EFFECT = 0xFb0;
const header_t SMSG_CHAIR_SIT = 0x12D;
const header_t SMSG_PLAYER_CHANGE_LOOK = 0xFb3;
const header_t SMSG_SKILL_SHOW = 0xFb4;
const header_t SMSG_3RD_PARTY_SKILL = 0xFb5; // From other player 12F?
const header_t SMSG_3RD_PARTY_BUFF_END = 0xFb6; // From other player 130?
const header_t SMSG_PARTY_HP_DISPLAY = 0xFb7;
const header_t SMSG_PIRATE_BOMB = 0xFba; // Drops the bomb of the pirate skill at pos 0,0. The client disconnects right after it landed...
const header_t SMSG_CHAIR = 0x13B;
const header_t SMSG_THEATRICS = 0x13E; // Gain item notice in chat
const header_t SMSG_MAP_TELEPORT = 0xFbd; // Instantaneous transfer
const header_t SMSG_MESOBAG_SUCCESS = 0x141; // "You recieved ... mesos from mesobag"
const header_t SMSG_MESOBAG_FAILURE = 0x142; // If the mesobag has failed?
const header_t SMSG_MESOBAG_RANDOM_SUCCESS = 0x143;
const header_t SMSG_MESOBAG_RANDOM_FAILURE = 0x144;
const header_t SMSG_QUEST_UPDATE = 0x147;
const header_t SMSG_BUBBLE = 0x14A;
const header_t SMSG_SOUND = 0x14B;
const header_t SMSG_CHAT_MULTICOLOR = 0x160; // Multiple colors available!
const header_t SMSG_EVENT_BOARD = 0xFc9; // Korean event board :D
const header_t SMSG_CINEMATICS = 0x179;
const header_t SMSG_SKILL_COOLDOWN = 0x181; // Skill cooldown
const header_t SMSG_MOB_SHOW = 0x18D;
const header_t SMSG_MOB_DEATH = 0x18E;
const header_t SMSG_MOB_CONTROL = 0x18F;
const header_t SMSG_MOB_CONTROL_MOVEMENT = 0x190;
const header_t SMSG_MOB_MOVEMENT = 0x191;
const header_t SMSG_MOB_STATUS_ADDITION = 0xFd6;
const header_t SMSG_MOB_STATUS_REMOVE = 0xFd7;
const header_t SMSG_MOB_DAMAGE = 0xFda;
const header_t SMSG_MOB_HP_DISPLAY = 0x19B;
const header_t SMSG_MOB_DRAGGED = 0xFdf; // New in .56, Monster Magnet'd mobs get their own packet
const header_t SMSG_NPC_SHOW = 0x1AC;
const header_t SMSG_NPC_CONTROL = 0x1AF;
const header_t SMSG_NPC_ANIMATE = 0x1B0;
const header_t SMSG_NPC_SHOW_EFFECT = 0x1F1;
const header_t SMSG_DROP_ITEM = 0x1BB;
const header_t SMSG_DROP_PICKUP = 0x1BD;
const header_t SMSG_MESSAGE_FLY_DISABLED = 0xFf0; // "You can't fly it here" O.o Might be from these kites
const header_t SMSG_MIST_SPAWN = 0xFf3;
const header_t SMSG_MIST_DESPAWN = 0xFf4;
const header_t SMSG_MYSTIC_DOOR_SPAWN = 0xFf5;
const header_t SMSG_MYSTIC_DOOR_DESPAWN = 0xFf6;
const header_t SMSG_REACTOR_TRIGGER = 0xFf7;
const header_t SMSG_REACTOR_SPAWN = 0xFf9;
const header_t SMSG_REACTOR_DESPAWN = 0xFfa;
const header_t SMSG_HORNTAIL_CAVE = 0xF10c; // Horntail Cave close warning. [0D 01] [SPAWNED (byte)] [TIME (int)] . If SPAWNED = 0 and TIME = 0 then cave is closed
const header_t SMSG_ZAKUM_SHRINE = 0x208; // Zakum Shrine close warning. See SMSG_HORNTAIL_CAVE
const header_t SMSG_NPC_TALK = 0x209;
const header_t SMSG_SHOP = 0xF10f;
const header_t SMSG_ITEM_PURCHASED = 0xF110;
const header_t SMSG_STORAGE = 0xF113;
const header_t SMSG_MESSENGER_ROOM = 0xF117; // Chatroom / Maple Messenger
const header_t SMSG_PLAYER_ROOM = 0xF118; // Trades, player shops, minigames
const header_t SMSG_PACKAGE_DELIVERY = 0xF120; // Duey :D
const header_t SMSG_CASHSHOP_NX = 0xF122;
const header_t SMSG_CASHSHOP_OPERATION = 0xF123;
const header_t SMSG_CASHSHOP_NAMECHANGE_RESULT = 0xF125;
const header_t SMSG_KEYMAP = 0x22E;
const header_t SMSG_MAPLETV_ON = 0xF130;
const header_t SMSG_MAPLETV_OFF = 0xF131;
const header_t SMSG_HAMMER = 0xF13d;