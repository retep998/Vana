/*
Copyright (C) 2008-2010 Vana Development Team

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

#define IV_NO_PATCH_LOCATION 0x0d
#define IV_PATCH_LOCATION 0x0e

#define SMSG_AUTHENTICATION 0x00
#define SMSG_WORLD_STATUS 0x03
#define SMSG_ACCOUNT_GENDER_DONE 0x04
#define SMSG_MESSAGE_TRY_AGAIN 0x05 // Shows a popup with "Try again!" :P
#define SMSG_PIN 0x06
#define SMSG_PIN_ASSIGNED 0x07
#define SMSG_PLAYER_GLOBAL_LIST 0x08
#define SMSG_WORLD_LIST 0x0A
#define SMSG_PLAYER_LIST 0x0B
#define SMSG_CHANNEL_CONNECT 0x0C
#define SMSG_PLAYER_NAME_CHECK 0x0D
#define SMSG_PLAYER_CREATE 0x0E
#define SMSG_PLAYER_DELETE 0x0F
#define SMSG_CHANNEL_CHANGE 0x10
#define SMSG_PING 0x11
#define SMSG_CHANNEL_SELECT 0x14
#define SMSG_LOGIN_RETURN 0x16

#define SMSG_INVENTORY_ITEM_MOVE 0x1D
#define SMSG_INVENTORY_SLOT_UPDATE 0x1E
#define SMSG_PLAYER_UPDATE 0x1F
#define SMSG_SKILL_USE 0x20
#define SMSG_SKILL_CANCEL 0x21
#define SMSG_TEMP_STATS 0x22 // This beauty can change your stats and it won't change back until you restart your client or send the packet 0x20 :D
#define SMSG_TEMP_STATS_RESET 0x23 // Bui thinks it's from Balrog (setting your stats to 1 thing)
#define SMSG_SKILL_ADD 0x24
#define SMSG_FAME 0x26
#define SMSG_NOTICE 0x27 //Bottom-right corner notice
#define SMSG_CLIENT_UPDATE 0x28 // This thing takes the client to the gamepatches page @ the nexon page
#define SMSG_NOTE 0x29 // The real note system.
#define SMSG_TELEPORT_ROCK 0x2A
#define SMSG_ANTI_MACRO 0x2B
#define SMSG_REPORT_MORE 0x2D // More report shit >_>
#define SMSG_QUEST_COMPLETED 0x31
#define SMSG_PLAYER_STORE_MESSAGE 0x32 // For store messages
#define SMSG_SKILLBOOK 0x33
#define SMSG_REPORT_USER 0x34
#define SMSG_REACHED_TRADE_MESOS_LIMIT 0x36 // "Players that are Level 15 and below may only trade 1 million mesos per day. You have reached the limit today, please try again tomorrow"
#define SMSG_BBS 0x3B
#define SMSG_PLAYER_INFO 0x3D
#define SMSG_PARTY 0x3E
#define SMSG_BUDDY 0x3F
#define SMSG_GUILD 0x41
#define SMSG_ALLIANCE 0x42
#define SMSG_PORTAL_ACTION 0x43
#define SMSG_MESSAGE 0x44
#define SMSG_OWL_OF_MINERVA 0x46
#define SMSG_WEDDING_PROPOSAL_WHISHLIST 0x48 // Holds both whishlist and proposal
#define SMSG_WEDDING 0x49 // Holds wedding and proposal things like "You are now engaged."...
#define SMSG_WEDDING_MORE 0x4a // Holds wedding stuff too.....
#define SMSG_YELLOW_MESSAGE 0x4d // Used by bosses like Stumpy and Mano
#define SMSG_ARIANT_CATCH_RESULT 0x4f // Used by the Ariant match when using the Element Rock
#define SMSG_PLAYER_NPC_SHOW 0x51
#define SMSG_MONSTER_BOOK_ADD 0x52
#define SMSG_MONSTER_BOOK_COVER 0x54
#define SMSG_PLAYER_LEVEL_UPDATE 0x63 // Byte after it: 2 = guild, everything else = family
#define SMSG_POPUP_MESSAGE 0x6D // I don't know where it is used, but it's like the admin warning? Might be part of megaphone
#define SMSG_MEGAPHONE_MESSAGES 0x6E // Wait line shit and such? Megaphone message too... (when byte is not 0x46 or 0x47)
#define SMSG_AVATAR_MEGAPHONE 0x6F
#define SMSG_NAME_CANCELLATION_REQUEST 0x71 // It's gone before you know it (when it was successfull)
#define SMSG_TRANSFER_CANCELLATION_REQUEST 0x72 // It's gone before you know it (when it was successfull)
#define SMSG_GM_BAN 0x74
#define SMSG_SILVER_BOX_REWARD 0x75 // Silver Box event?
#define SMSG_NEWYEAR_CARDS 0x76 // I want to code this!
#define SMSG_RANDOM_TRANSFORMATION_POTION 0x77
#define SMSG_NAME_CANCELLATION_REQUEST_NOT_REQUESTED 0x78 // 1 thing, 1 whole header
#define SMSG_MACRO_LIST 0x7C
#define SMSG_CHANGE_MAP 0x7D
#define SMSG_ENTER_CASHSHOP 0x7F
#define SMSG_PORTAL_BLOCKED 0x83
#define SMSG_CHANNEL_BLOCKED 0x84 // For messages like 'This server is unavailable'
#define SMSG_MAP_FORCE_EQUIPMENT 0x85 // Wear apple suit on training maps
#define SMSG_MESSAGE_GROUP 0x86 // Groups like party  buddies and guild
#define SMSG_COMMAND 0x87 // Respond to / commands i.e. "/find"
#define SMSG_SPOUSE_CHAT 0x88 // Some weird colored chat... Sources say Spouse chat.
#define SMSG_MESSAGE_CANNOT_USE 0x89 // "You can't use it here in this map." Note: it needs a byte after it of '0'.
#define SMSG_MAP_EFFECT 0x8A // Change Music/Boss HP Bars
#define SMSG_MAP_WEATHER_EFFECT 0x8E // Used by weather NX items
#define SMSG_CASH_SONG 0x8F // Used for cash items only, plays a song... :D
#define SMSG_GM 0x90 // Hide, other miscellaneous junk
#define SMSG_OX_QUIZ_QUESTION 0x91 // Shows the OX quiz question (with a timer)
#define SMSG_EVENT_INSTRUCTION 0x92 // GM event instructions - Ola Ola  etc.
#define SMSG_TIMER 0x93 // For places with clock i.e. ship station
#define SMSG_GM_WARNING 0x98 // Shows a popup with the message from a GM/an Admin
#define SMSG_TIMER_OFF 0x9A // Makes timer disappear from map
#define SMSG_MAP_PLAYER_SPAWN 0xA0
#define SMSG_MAP_PLAYER_DESPAWN 0xA1
#define SMSG_PLAYER_CHAT 0xA2
#define SMSG_CHALKBOARD 0xA4
#define SMSG_SCROLL_USE 0xA7
#define SMSG_PET_SUMMON 0xA8
#define SMSG_PET_MOVEMENT 0xAA
#define SMSG_PET_MESSAGE 0xAB
#define SMSG_PET_NAME_CHANGE 0xAC
#define SMSG_PET_SHOW 0xAD
#define SMSG_PET_ANIMATION 0xAE
#define SMSG_SUMMON_SPAWN 0xAF
#define SMSG_SUMMON_DESPAWN 0xB0
#define SMSG_SUMMON_MOVEMENT 0xB1
#define SMSG_SUMMON_ATTACK 0xB2
#define SMSG_SUMMON_DAMAGE 0xB3
#define SMSG_PLAYER_MOVEMENT 0xB9
#define SMSG_ATTACK_MELEE 0xBA
#define SMSG_ATTACK_RANGED 0xBB
#define SMSG_ATTACK_MAGIC 0xBC
#define SMSG_ATTACK_ENERGY_CHARGE 0xBD
#define SMSG_SPECIAL_SKILL 0xBE
#define SMSG_SPECIAL_SKILL_END 0xBF
#define SMSG_PLAYER_DAMAGE 0xC0
#define SMSG_EMOTE 0xC1
#define SMSG_ITEM_EFFECT 0xC2
#define SMSG_CHAIR_SIT 0xC4
#define SMSG_PLAYER_CHANGE_LOOK 0xC5
#define SMSG_SKILL_SHOW 0xC6
#define SMSG_3RD_PARTY_SKILL 0xC7 // From other player
#define SMSG_3RD_PARTY_BUFF_END 0xC8 // From other player
#define SMSG_PARTY_HP_DISPLAY 0xC9
#define SMSG_GUILD_NAME 0xCA // Updates guildname from character :D
#define SMSG_GUILD_EMBLEM 0xCB // Updates guildemblem from character :D
#define SMSG_PIRATE_BOMB 0xCC // Drops the bomb of the pirate skill at pos 0,0. The client disconnects right after it landed...
#define SMSG_CHAIR 0xCD
#define SMSG_THEATRICS 0xCE // Gain item notice in chat
#define SMSG_MAP_TELEPORT 0xcf // Instantaneous transfer
#define SMSG_MESOBAG_SUCCESS 0xd1 // "You recieved ... mesos from mesobag"
#define SMSG_MESOBAG_FAILURE 0xd2 // If the mesobag has failed?
#define SMSG_QUEST_UPDATE 0xD3
#define SMSG_BUBBLE 0xC8
#define SMSG_SOUND 0xca
#define SMSG_COMBO_COUNTER 0xe1
#define SMSG_SKILL_COOLDOWN 0xEA
#define SMSG_MOB_SHOW 0xEC
#define SMSG_MOB_DEATH 0xED
#define SMSG_MOB_CONTROL 0xEE
#define SMSG_MOB_CONTROL_MOVEMENT 0xEF
#define SMSG_MOB_MOVEMENT 0xF0
#define SMSG_MOB_STATUS_ADDITION 0xF2
#define SMSG_MOB_STATUS_REMOVE 0xF3
#define SMSG_MOB_DAMAGE 0xF6
#define SMSG_MOB_HP_DISPLAY 0xFA
#define SMSG_MOB_DRAGGED 0xFB // New in .56, Monster Magnet'd mobs get their own packet
#define SMSG_NPC_SHOW 0x101
#define SMSG_NPC_CONTROL 0x103
#define SMSG_NPC_ANIMATE 0x104
#define SMSG_NPC_SHOW_EFFECT 0x105
#define SMSG_DROP_ITEM 0x10C
#define SMSG_DROP_PICKUP 0x10D
#define SMSG_MESSAGE_FLY_DISABLED 0x10E // "You can't fly it here" O.o Might be from these kites
#define SMSG_MIST_SPAWN 0x111
#define SMSG_MIST_DESPAWN 0x112
#define SMSG_MYSTIC_DOOR_SPAWN 0x113
#define SMSG_MYSTIC_DOOR_DESPAWN 0x114
#define SMSG_REACTOR_TRIGGER 0x115
#define SMSG_REACTOR_SPAWN 0x117
#define SMSG_REACTOR_DESPAWN 0x118
#define SMSG_HORNTAIL_CAVE 0x12E // Horntail Cave close warning. [0D 01] [SPAWNED (byte)] [TIME (int)] . If SPAWNED = 0 and TIME = 0 then cave is closed
#define SMSG_ZAKUM_SHRINE 0x12F // Zakum Shrine close warning. See SMSG_HORNTAIL_CAVE
#define SMSG_NPC_TALK 0x130
#define SMSG_SHOP 0x131
#define SMSG_ITEM_PURCHASED 0x132
#define SMSG_STORAGE 0x135
#define SMSG_MESSENGER_ROOM 0x139 // Chatroom / Maple Messenger
#define SMSG_PLAYER_ROOM 0x13A // Trades  player shops  minigames
#define SMSG_PACKAGE_DELIVERY 0x13C // Duey :D
#define SMSG_CASHSHOP_NX 0x13E
#define SMSG_CASHSHOP_OPERATION 0x13F
#define SMSG_CASHSHOP_NAMECHANGE_RESULT 0x141
#define SMSG_KEYMAP 0x14F
#define SMSG_MAPLETV_ON 0x155
#define SMSG_MAPLETV_OFF 0x156
#define SMSG_HAMMER 0x162