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

#define SMSG_AUTHENTICATION 0x00
// 0x01 = Trial Auth
#define SMSG_AUTHENTICATION_2 0x02
#define SMSG_WORLD_STATUS 0x03
#define SMSG_ACCOUNT_GENDER_DONE 0x04
#define SMSG_MESSAGE_TRY_AGAIN 0x05 // Shows a popup with "Try again!" :P
#define SMSG_PIN 0x06
#define SMSG_PIN_ASSIGNED 0x07
#define SMSG_PLAYER_GLOBAL_LIST 0x08
// 0x09 = Errors + connect to server?
#define SMSG_WORLD_LIST 0x0a
#define SMSG_PLAYER_LIST 0x0b
#define SMSG_CHANNEL_CONNECT 0x0c
#define SMSG_PLAYER_NAME_CHECK 0x0d
#define SMSG_PLAYER_CREATE 0x0e
#define SMSG_PLAYER_DELETE 0x0f
#define SMSG_CHANNEL_CHANGE 0x10
#define SMSG_PING 0x11
#define SMSG_LOGIN_RETURN 0x18
#define SMSG_LAST_SELECTED_WORLD 0x1B
#define SMSG_RECOMMENDED_WORLDS 0x1C
#define SMSG_PLAYER_CREATE_SPECIAL_ENABLE 0x1D // 0x1E?
#define SMSG_INVENTORY_ITEM_MOVE 0x20
#define SMSG_INVENTORY_SLOT_UPDATE 0x21
#define SMSG_PLAYER_UPDATE 0x22
#define SMSG_SKILL_USE 0x23
#define SMSG_SKILL_CANCEL 0x24
#define SMSG_FORCED_STATS_SET 0x25 // This beauty can change your stats and it won't change back until you restart your client or send the packet 0xF20 :D
#define SMSG_FORCED_STATS_RESET 0x26 // Bui thinks it's from Balrog (setting your stats to 1 thing)
#define SMSG_SKILL_ADD 0x27
#define SMSG_FAME 0x2B
#define SMSG_NOTICE 0x2C // Bottom-right corner notice
#define SMSG_CLIENT_UPDATE 0x2D // This thing takes the client to the gamepatches page @ the nexon page
#define SMSG_NOTE 0x2E // The real note system
#define SMSG_TELEPORT_ROCK 0x2F // Teleport Rock, VIP Teleport Rock
#define SMSG_ANTI_MACRO 0x30
#define SMSG_REPORT_MORE 0x31 // More report shit >_>
#define SMSG_QUEST_COMPLETED 0x37
#define SMSG_PLAYER_STORE_MESSAGE 0x38
#define SMSG_SKILLBOOK 0x39
#define SMSG_REPORT_USER 0x43
#define SMSG_REACHED_TRADE_MESOS_LIMIT 0x44 // "Players that are Level 15 and below may only trade 1 million mesos per day. You have reached the limit today, please try again tomorrow"
#define SMSG_PLAYER_INFO 0x49
#define SMSG_PARTY 0x4A
#define SMSG_BUDDY 0x4B
#define SMSG_GUILD 0x52
#define SMSG_ALLIANCE 0x53
#define SMSG_PORTAL_ACTION 0x54
#define SMSG_MESSAGE 0x58
#define SMSG_OWL_OF_MINERVA 0x5A
#define SMSG_WEDDING_PROPOSAL_WHISHLIST 0xF45 // Holds both whishlist and proposal
#define SMSG_WEDDING 0xF46 // Holds wedding and proposal things like "You are now engaged."...
#define SMSG_WEDDING_MORE 0xF47 // Holds wedding stuff too.....
#define SMSG_YELLOW_MESSAGE 0x61 // Used by bosses like Stumpy and Mano
#define SMSG_ARIANT_CATCH_RESULT 0x63 // Used by the Ariant match when using the Element Rock
#define SMSG_PLAYER_NPC_SHOW 0x65
#define SMSG_MONSTER_BOOK_ADD 0x68
#define SMSG_MONSTER_BOOK_COVER 0x69
#define SMSG_PLAYER_LEVEL_UPDATE 0x85 // Byte after it: 2 = guild, everything else = family
#define SMSG_PLAYER_MARRIED_UPDATE 0x86 // Byte after it: 0 = Guild, 1 = Family, else = nothing (Shows "<'type'[name] is now married. Please congratualte them." lol)
#define SMGS_PLAYER_JOB_UPDATE 0x87 // Byte after it: 0 = Guild, 1 = Family, else = nothing  (Shows "<'type'[name] has advanced to a(an) [jobid].")
#define SMSG_POPUP_MESSAGE 0x89 // I don't know where it is used, but it's like the admin warning? Might be part of megaphone
#define SMSG_MEGAPHONE_MESSAGES 0x8A // Wait line shit and such? Megaphone message too... (when byte is not 0xF46 or 0xF47)
#define SMSG_AVATAR_MEGAPHONE 0x8B
#define SMSG_NAME_CANCELLATION_REQUEST 0x8D // It's gone before you know it (when it was successfull)
#define SMSG_TRANSFER_CANCELLATION_REQUEST 0x8E // It's gone before you know it (when it was successfull)
#define SMSG_GM_BAN 0xF6c // GMMapleStory with it's HACK reason bans
#define SMSG_SILVER_BOX_REWARD 0xF6d // Silver Box event?
#define SMSG_NEWYEAR_CARDS 0xF6e // I want to code this!
#define SMSG_RANDOM_TRANSFORMATION_POTION 0xF6f
#define SMSG_NAME_CANCELLATION_REQUEST_NOT_REQUESTED 0xF70 // 1 thing, 1 whole header
#define SMSG_FAILED_RECEIVING_REWARD 0xB7
#define SMSG_MACRO_LIST 0xB8
#define SMSG_CHANGE_MAP 0xB9
#define SMSG_CONNECT_ITC 0xBA
#define SMSG_CONNECT_CASHSHOP 0xBB
// 0xBC = Back Effect: BYTE INT BYTE INT
// 0xBD = Map Object Visible: BYTE foreach [STRING BYTE]
#define SMSG_PORTAL_BLOCKED 0xc0
#define SMSG_CHANNEL_BLOCKED 0xc1 // For messages like 'This server is unavailable'
#define SMSG_MAP_FORCE_EQUIPMENT 0xc2 // Wear apple suit on training maps, all equips except bathrobe off for Showa spa, etc.
#define SMSG_MESSAGE_GROUP 0xc3 // Groups like party, buddies and guild
#define SMSG_COMMAND 0xc4 // Respond to / commands i.e. "/find"
#define SMSG_SPOUSE_CHAT 0xc5 // Some weird colored chat... Sources say Spouse chat.
#define SMSG_MESSAGE_CANNOT_USE 0xc6 // "You can't use it here in this map." Note: it needs a byte after it of '0'.
#define SMSG_MAP_EFFECT 0xc7 // Change Music/Boss HP Bars
#define SMSG_MAP_WEATHER_EFFECT 0xCB // Used by weather NX items
#define SMSG_CASH_SONG 0xCC // Used for cash items only, plays a song... :D
#define SMSG_GM 0xCD // Hide, other miscellaneous junk
#define SMSG_OX_QUIZ_QUESTION 0xCE // Shows the OX quiz question (with a timer)
#define SMSG_EVENT_INSTRUCTION 0xF86 // GM event instructions - Ola Ola, etc.
#define SMSG_TIMER 0xF87 // For places with clock i.e. ship station
#define SMSG_SHIP 0xF88 // Showing or removing (balrog) ships. Only works for maps with ships (else you'll only hear the horn)
#define SMSG_GM_WARNING 0xF8c // Shows a popup with the message from a GM/an Admin
#define SMSG_TIMER_OFF 0xF8e // Makes timer disappear from map
#define SMSG_MAP_PLAYER_SPAWN 0xE6
#define SMSG_MAP_PLAYER_DESPAWN 0xE7
#define SMSG_PLAYER_CHAT 0xE8
#define SMSG_PLAYER_CHAT_2 0xE9
#define SMSG_CHALKBOARD 0xEA
#define SMSG_SCROLL_USE 0xF98
#define SMSG_PET_SUMMON 0xF99
#define SMSG_PET_MOVEMENT 0xF9b
#define SMSG_PET_MESSAGE 0xF9c
#define SMSG_PET_NAME_CHANGE 0xF9d
#define SMSG_PET_SHOW 0xF9e
#define SMSG_PET_ANIMATION 0xF9f
#define SMSG_SUMMON_SPAWN 0xFa0
#define SMSG_SUMMON_DESPAWN 0xFa1
#define SMSG_SUMMON_MOVEMENT 0xFa2
#define SMSG_SUMMON_ATTACK 0xFa3
#define SMSG_SUMMON_DAMAGE 0xFa4
#define SMSG_PLAYER_MOVEMENT 0x11C
#define SMSG_ATTACK_MELEE 0x11E
#define SMSG_ATTACK_RANGED 0x11F
#define SMSG_ATTACK_MAGIC 0x120
#define SMSG_ATTACK_ENERGYCHARGE 0x121
#define SMSG_SPECIAL_SKILL 0xFac
#define SMSG_SPECIAL_SKILL_END 0xFad
#define SMSG_PLAYER_DAMAGE 0xFae
#define SMSG_EMOTE 0x126
#define SMSG_ITEM_EFFECT 0xFb0
#define SMSG_CHAIR_SIT 0x12D
#define SMSG_PLAYER_CHANGE_LOOK 0xFb3
#define SMSG_SKILL_SHOW 0xFb4
#define SMSG_3RD_PARTY_SKILL 0xFb5 // From other player 12F?
#define SMSG_3RD_PARTY_BUFF_END 0xFb6 // From other player 130?
#define SMSG_PARTY_HP_DISPLAY 0xFb7
#define SMSG_PIRATE_BOMB 0xFba // Drops the bomb of the pirate skill at pos 0,0. The client disconnects right after it landed...
#define SMSG_CHAIR 0x13B
#define SMSG_THEATRICS 0x13E // Gain item notice in chat
#define SMSG_MAP_TELEPORT 0xFbd // Instantaneous transfer
#define SMSG_MESOBAG_SUCCESS 0x141 // "You recieved ... mesos from mesobag"
#define SMSG_MESOBAG_FAILURE 0x142 // If the mesobag has failed?
#define SMSG_MESOBAG_RANDOM_SUCCESS 0x143
#define SMSG_MESOBAG_RANDOM_FAILURE 0x144
#define SMSG_QUEST_UPDATE 0x147
#define SMSG_BUBBLE 0x14A
#define SMSG_SOUND 0x14B
#define SMSG_CHAT_MULTICOLOR 0x160 // Multiple colors available!
#define SMSG_EVENT_BOARD 0xFc9 // Korean event board :D
#define SMSG_SKILL_COOLDOWN 0x181 // Skill cooldown
#define SMSG_MOB_SHOW 0x18D
#define SMSG_MOB_DEATH 0x18E
#define SMSG_MOB_CONTROL 0x18F
#define SMSG_MOB_CONTROL_MOVEMENT 0x190
#define SMSG_MOB_MOVEMENT 0x191
#define SMSG_MOB_STATUS_ADDITION 0xFd6
#define SMSG_MOB_STATUS_REMOVE 0xFd7
#define SMSG_MOB_DAMAGE 0xFda
#define SMSG_MOB_HP_DISPLAY 0x19B
#define SMSG_MOB_DRAGGED 0xFdf // New in .56, Monster Magnet'd mobs get their own packet
#define SMSG_NPC_SHOW 0x1AC
#define SMSG_NPC_CONTROL 0x1AF
#define SMSG_NPC_ANIMATE 0x1B0
#define SMSG_NPC_SHOW_EFFECT 0x1F1
#define SMSG_DROP_ITEM 0x1BB
#define SMSG_DROP_PICKUP 0x1BD
#define SMSG_MESSAGE_FLY_DISABLED 0xFf0 // "You can't fly it here" O.o Might be from these kites
#define SMSG_MIST_SPAWN 0xFf3
#define SMSG_MIST_DESPAWN 0xFf4
#define SMSG_MYSTIC_DOOR_SPAWN 0xFf5
#define SMSG_MYSTIC_DOOR_DESPAWN 0xFf6
#define SMSG_REACTOR_TRIGGER 0xFf7
#define SMSG_REACTOR_SPAWN 0xFf9
#define SMSG_REACTOR_DESPAWN 0xFfa
#define SMSG_HORNTAIL_CAVE 0xF10c // Horntail Cave close warning. [0D 01] [SPAWNED (byte)] [TIME (int)] . If SPAWNED = 0 and TIME = 0 then cave is closed
#define SMSG_ZAKUM_SHRINE 0x208 // Zakum Shrine close warning. See SMSG_HORNTAIL_CAVE
#define SMSG_NPC_TALK 0x209
#define SMSG_SHOP 0xF10f
#define SMSG_ITEM_PURCHASED 0xF110
#define SMSG_STORAGE 0xF113
#define SMSG_MESSENGER_ROOM 0xF117 // Chatroom / Maple Messenger
#define SMSG_PLAYER_ROOM 0xF118 // Trades, player shops, minigames
#define SMSG_PACKAGE_DELIVERY 0xF120 // Duey :D
#define SMSG_CASHSHOP_NX 0xF122
#define SMSG_CASHSHOP_OPERATION 0xF123
#define SMSG_CASHSHOP_NAMECHANGE_RESULT 0xF125
#define SMSG_KEYMAP 0x22E
#define SMSG_MAPLETV_ON 0xF130
#define SMSG_MAPLETV_OFF 0xF131
#define SMSG_HAMMER 0xF13d