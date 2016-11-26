/*
Copyright (C) 2008-2016 Vana Development Team

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

#define SMSG_CHANNEL_CHANGE 0x10
#define SMSG_INVENTORY_OPERATION 0x1a
#define SMSG_INVENTORY_SLOT_UPDATE 0x1b
#define SMSG_PLAYER_UPDATE 0x1c
#define SMSG_SKILL_USE 0x1d
#define SMSG_SKILL_CANCEL 0x1e
#define SMSG_TEMP_STATS 0x1f // This beauty can change your stats and it won't change back until you restart your client or send the packet 0x20 :D
#define SMSG_TEMP_STATS_RESET 0x20 // Bui thinks it's from Balrog (setting your stats to 1 thing)
#define SMSG_SKILL_ADD 0x21
#define SMSG_EXCL_REQUEST 0x22 // Basically an 'unstuck' command
#define SMSG_FAME 0x23
#define SMSG_NOTICE 0x24 // Bottom-right corner notice
#define SMSG_CLIENT_UPDATE 0x25 // This thing takes the client to the gamepatches page @ the nexon page
#define SMSG_NOTE 0x26 // The real note system
#define SMSG_TELEPORT_ROCK 0x27 // Teleport Rock, VIP Teleport Rock
#define SMSG_ANTI_MACRO 0x28
// 0x29 does not exist?
#define SMSG_CLAIM_RESULT 0x2a
#define SMSG_CLAIM_SERVER_AVAILABLE_TIME 0x2b // byte startHour, byte endHour
#define SMSG_CLAIM_SERVER_STATUS_CHANGED 0x2c // bool isAvailable
#define SMSG_SET_TAMINGMOB_INFO 0x2d // int characterid, int level, int exp, int fatigue, bool isLevelUp (broadcast to map)
#define SMSG_QUEST_COMPLETED 0x2e
#define SMSG_PLAYER_STORE_MESSAGE 0x2f
#define SMSG_SKILLBOOK 0x30
#define SMSG_GATHER_ITEM_RESULT 0x31
#define SMSG_SORT_ITEM_RESULT 0x32
// 0x33 does not exist
#define SMSG_REPORT_USER 0x34
// 0x35 does not exist
#define SMSG_REACHED_TRADE_MESOS_LIMIT 0x36 // "Players that are Level 15 and below may only trade 1 million mesos per day. You have reached the limit today, please try again tomorrow"
#define SMSG_SET_GENDER 0x37
#define SMSG_GUILD_BBS 0x38
// 0x39 does not exist
#define SMSG_PLAYER_INFO 0x3a
#define SMSG_PARTY 0x3b
#define SMSG_BUDDY 0x3c
// 0x3d does not exist
#define SMSG_GUILD 0x3e
#define SMSG_ALLIANCE 0x3f
#define SMSG_PORTAL_ACTION 0x40
#define SMSG_MESSAGE 0x41
#define SMSG_INCUBATOR 0x42
#define SMSG_OWL_OF_MINERVA 0x43
#define SMSG_SHOP_LINK_RESULT 0x44 // Note, forwards call to different function, no good disasm!
#define SMSG_WEDDING_PROPOSAL_WHISHLIST 0x45 // Holds both whishlist and proposal
#define SMSG_WEDDING 0x46 // Holds wedding and proposal things like "You are now engaged."...
#define SMSG_WEDDING_GIFT_RESULT 0x47 // Holds wedding stuff too.....
#define SMSG_NOTIFY_MARRIED_PARTNER_MAP_TRANSFER 0x48
#define SMSG_USE_CASH_PET_FOOD 0x49 // if (failed) { bool true } else { bool false, byte pet index }
#define SMSG_SET_WEEK_EVENT_MESSAGE 0x4a
#define SMSG_SET_POTION_DISCOUNT_RATE 0x4b // byte percentage; Reduces potion prices by {percentage}%
#define SMSG_ARIANT_CATCH_RESULT 0x4c // Used by the Ariant match when using the Element Rock
#define SMSG_PLAYER_NPC_RESULT 0x4d // Doesnt have any effect other than reading 1 byte.
#define SMSG_PLAYER_NPC_SHOW 0x4e
#define SMSG_MONSTER_BOOK_ADD 0x4f
#define SMSG_MONSTER_BOOK_COVER 0x50
#define SMSG_HOUR_CHANGED 0x51 // short dayOfWeek, short hour (not sure, not used)
#define SMSG_MINIMAP_ON_OFF 0x52 // bool isOn
#define SMSG_UPDATE_CONSULT_AUTH_KEY 0x53 // string key; Most likely related to the GM board?
#define SMSG_CLASS_COMPETITION_AUTH_KEY 0x54 // string key; Not sure what this is about
#define SMSG_SESSION_VALUE 0x55 // string key, string value; Seems to be only setting the 'energy' session key. More in newer versions
#define SMSG_BONUS_EXP_RATE_CHANGE 0x56 // int equipslot, int hour, int bonusexp (ignored, used in recent maple versions); Shows notification of equip giving additional exp after N hours
#define SMSG_FAMILY_MEMBERS 0x57
#define SMSG_FAMILY_WINDOW_TOP 0x58
#define SMSG_FAMILY_ERROR 0x59 // all kinds of family errors
#define SMSG_FAMILY_INVITE 0x5a
#define SMSG_FAMILY_ACCEPTED_SENIOR 0x5b
#define SMSG_FAMILY_ACCEPTED_JUNIOR 0x5c
#define SMSG_FAMILY_WINDOW_TEXT 0x5d
#define SMSG_FAMILY_BUFF_NOTES 0x5e
#define SMSG_FAMILY_LOGGED_IN 0x5f
#define SMSG_FAMILY_BUFF 0x60
#define SMSG_FAMILY_ASK_WARP 0x61
#define SMSG_OPEN_GM_BOARD 0x62
#define SMSG_PLAYER_LEVEL_UPDATE 0x63 // if (guild) { byte 2 } else { byte 2 (family) }, int level, string name
// 0x64 does not exist
#define SMSG_MAPLETV_USE_RESULT 0x65 // string message
#define SMSG_AVATAR_MEGAPHONE_RESULT 0x66 // 0x46 = 'Waiting line is longer than 15 seconds', 0x47 = 'Megaphone only available for chars over lvl 10', everything else: your own message
#define SMSG_AVATAR_MEGAPHONE 0x67
#define SMSG_AVATAR_MEGAPHONE_CLEAR 0x68
#define SMSG_NAME_CANCELLATION_REQUEST 0x69 // It's gone before you know it (when it was successful)
#define SMSG_TRANSFER_CANCELLATION_REQUEST 0x6a // It's gone before you know it (when it was successful)
#define SMSG_DESTROY_SHOP_RESULT 0x6b // Either 'Unknown error occurred', 'Your shop has been closed by a gm, get your stuff at fred' or a custom message
#define SMSG_GM_BAN 0x6c // GMMapleStory with it's HACK reason bans
#define SMSG_SILVER_BOX_REWARD 0x6d // Silver Box event?
#define SMSG_NEWYEAR_CARDS 0x6e // I want to code this!
#define SMSG_RANDOM_TRANSFORMATION_POTION 0x6f
#define SMSG_NAME_CANCELLATION_REQUEST_NOT_REQUESTED 0x70 // 1 thing, 1 whole header
#define SMSG_MACRO_LIST 0x71
#define SMSG_CHANGE_MAP 0x72
#define SMSG_CONNECT_MTS 0x73
#define SMSG_CONNECT_CASHSHOP 0x74
#define SMSG_MAP_SET_BACK_EFFECT 0x75 // byte effect, int mapid, byte pageId, int duration
#define SMSG_MAP_RESET_BACK_EFFECT 0x76
#define SMSG_PORTAL_BLOCKED 0x77
#define SMSG_CHANNEL_BLOCKED 0x78 // For messages like 'This server is unavailable'
#define SMSG_MAP_FORCE_EQUIPMENT 0x79 // Wear apple suit on training maps, all equips except bathrobe off for Showa spa, etc.
#define SMSG_MESSAGE_GROUP 0x7a // Groups like party, buddies and guild
#define SMSG_COMMAND 0x7b // Respond to / commands i.e. "/find"
#define SMSG_SPOUSE_CHAT 0x7c // Spouse/couple chat
#define SMSG_SUMMON_ITEM_UNAVAILABLE 0x7d // "You can't use it here in this map." Note: it needs a byte after it of '0'.
#define SMSG_MAP_EFFECT 0x7e // Change Music/Boss HP Bars
#define SMSG_MAP_OBSTACLE_TOGGLE 0x7f // string name, int state
#define SMSG_MAP_OBSTACLE_TOGGLE_MULTIPLE 0x80 // int amount, foreach { string name, int state }
#define SMSG_MAP_OBSTACLE_RESET 0x81 // Resets all obstacle states
#define SMSG_MAP_WEATHER_EFFECT 0x82 // Used by weather NX items
#define SMSG_CASH_SONG 0x83 // Used for cash items only, plays a song... :D
#define SMSG_GM 0x84 // Hide, other miscellaneous junk
#define SMSG_OX_QUIZ_QUESTION 0x85 // Shows the OX quiz question (with a timer)
#define SMSG_EVENT_INSTRUCTION 0x86 // GM event instructions - Ola Ola, etc.
#define SMSG_TIMER 0x87 // For places with clock i.e. ship station
#define SMSG_SHIP 0x88 // Showing or removing (balrog) ships. Only works for maps with ships (else you'll only hear the horn)
#define SMSG_MAP_CONTIMOVE_STATE 0x89 // Same as SMSG_SHIP, but a bit different
#define SMSG_MODQUESTTIME_CLEAR 0x8a // Clears values for MODQUESTTIME, disabling or enabling quests in on specific times
#define SMSG_MODQUESTTIME_INIT 0x8b // byte amount, foreach { int questid, filetime start, filetime end }
#define SMSG_GM_WARNING 0x8c // Shows a popup with the message from a GM/an Admin
#define SMSG_MAP_OBJECT_STATE_SET 0x8d // Same as SMSG_MAP_OBSTACLE_TOGGLE
#define SMSG_TIMER_OFF 0x8e // Makes timer disappear from map
#define SMSG_MAP_ARIANT_RESULT 0x8f // Shows (in Ariant map?) the scores
#define SMSG_STALK_RESULT 0x90 // int amount, foreach { int charid, if (remove) { bool true } else { bool false, string name, int x, int y } }; Shows arrows of the people out of bounds. Useful against hackers
#define SMSG_MAP_PLAYER_SPAWN 0x91
#define SMSG_MAP_PLAYER_DESPAWN 0x92
#define SMSG_PLAYER_CHAT 0x93
#define SMSG_UNKNOWN_PLAYER_CHAT 0x94 // same format as SMSG_PLAYER_CHAT, but with additional string playername; you can chat without being in the map, for example
#define SMSG_CHALKBOARD 0x95
#define SMSG_MINIROOM_BALLOON 0x96 // For shops and such, hosted by people
#define SMSG_SET_CONSUME_ITEM_EFFECT 0x97 // int itemid; See Effect.wz\ItemEff.img\(itemid)
#define SMSG_SCROLL_USE 0x98
#define SMSG_PET_SPAWN 0x99
#define SMSG_PET_DESPAWN 0x9a
#define SMSG_PET_MOVEMENT 0x9b
#define SMSG_PET_MESSAGE 0x9c
#define SMSG_PET_NAME_CHANGE 0x9d
#define SMSG_PET_SHOW 0x9e
#define SMSG_PET_ANIMATION 0x9f
#define SMSG_SUMMON_SPAWN 0xa0
#define SMSG_SUMMON_DESPAWN 0xa1
#define SMSG_SUMMON_MOVEMENT 0xa2
#define SMSG_SUMMON_ATTACK 0xa3
#define SMSG_SUMMON_DAMAGE 0xa4
#define SMSG_SUMMON_SKILL 0xa5
// 0xa6 does not exist
#define SMSG_PLAYER_MOVEMENT 0xa7
#define SMSG_ATTACK_MELEE 0xa8
#define SMSG_ATTACK_RANGED 0xa9
#define SMSG_ATTACK_MAGIC 0xaa
#define SMSG_ATTACK_ENERGYCHARGE 0xab
#define SMSG_CHARGE_OR_STATIONARY_SKILL 0xac
#define SMSG_CHARGE_OR_STATIONARY_SKILL_END 0xad
#define SMSG_PLAYER_DAMAGE 0xae
#define SMSG_EMOTE 0xaf
#define SMSG_ITEM_EFFECT 0xb0
#define SMSG_UPGRADE_TOMB_EFFECT 0xb1 // int itemid, int x, int y; only specific maps accept this (hardcoded?). Wheel of Destiny related!!!
#define SMSG_CHAIR_SIT 0xb2
#define SMSG_PLAYER_CHANGE_LOOK 0xb3
#define SMSG_SKILL_SHOW 0xb4
#define SMSG_3RD_PARTY_SKILL 0xb5 // From other player
#define SMSG_3RD_PARTY_BUFF_END 0xb6 // From other player
#define SMSG_PARTY_HP_DISPLAY 0xb7
#define SMSG_CHANGE_GUILDNAME 0xb8 // string guildname
#define SMSG_CHANGE_GUILDMARK 0xb9 // regular guildmark fields (short, byte, short, byte)
#define SMSG_3RD_PARTY_BOMB 0xba
#define SMSG_CHAIR 0xbb
#define SMSG_THEATRICS 0xbc // Gain item notice in chat
#define SMSG_MAP_TELEPORT 0xbd // Instantaneous transfer
// 0xbe does not exist?
#define SMSG_MESOBAG_SUCCESS 0xbf // "You received ... mesos from mesobag"
#define SMSG_MESOBAG_FAILURE 0xc0 // If the mesobag has failed?
#define SMSG_QUEST_UPDATE 0xc1
#define SMSG_NOTIFY_HP_DAMAGE_BY_FIELD 0xc2 // When the field gives you, for example, cold damage, send this opcode + amount of damage
// 0xc3 is empty
#define SMSG_BUBBLE 0xc4
#define SMSG_EVENT_SOUND 0xc5 // Play event (field) sound. Sounds under Sound.wz\Field.img\ are accepted
#define SMSG_SOUND 0xc6 // Play minigame sound
#define SMSG_MAKER 0xc7 // Creating items and such
// 0xc8 does not exist
#define SMSG_EVENT_BOARD 0xc9 // Korean event board :D
#define SMSG_TOGGLE_UI 0xca // byte hide
// 0xcb seems to be 'OnSetStandAloneMode', not sure what it does
#define SMSG_KNIGHT_CHARACTER_CREATE_RESULT 0xcc // int error (-1 = 'unknown error', 1 = 'already exists', 2 = 'no slots left', 3 = 'name not valid', 0 = 'success')
#define SMSG_RANDOM_EMOTE 0xcd // int itemid; Activates a random emote depending on the item used (for buffing). Items starting with 5281 have this effect
#define SMSG_SKILL_COOLDOWN 0xce // Skill cooldown

#define SMSG_MOB_SHOW 0xd0
#define SMSG_MOB_DEATH 0xd1
#define SMSG_MOB_CONTROL 0xd2
#define SMSG_MOB_CONTROL_MOVEMENT 0xd3
#define SMSG_MOB_MOVEMENT 0xd4
// 0xd5 does not exist
#define SMSG_MOB_STATUS_ADDITION 0xd6
#define SMSG_MOB_STATUS_REMOVE 0xd7
#define SMSG_MOB_SUSPEND_RESET 0xd8 // int mapmobid, bool suspedReset; Used when a boss is spawned through a reactor or something (zakum?). Should activate its attacking
#define SMSG_MOB_AFFECTED 0xd9 // int mapmobid, int skillid, short delay; Used for certain skills (powerguard, dispell, magic guard, some power up skill, maybe more)
#define SMSG_MOB_DAMAGE 0xda
#define SMSG_MOB_EFFECT_BY_SKILL 0xdb // int mapmobid, int skillid. Seems to activate a special animation caused by certain skills (e.g. 3210001 mortal blow), mainly those with the 'special' node
// 0xdc does not exist
#define SMSG_MOB_CRC_KEY_CHANGED 0xdd // int mobCrcKey; used for hack detection
#define SMSG_MOB_HP_DISPLAY 0xde
#define SMSG_MOB_DRAGGED 0xdf // New in .56, Monster Magnet'd mobs get their own packet
#define SMSG_MOB_EFFECT_BY_ITEM 0xe0 // int mapmobid, int itemid, bool success; Activated by 'bridle' items, such as 2270000 (Pheromone Perfume, to tame Wild Pig). Mapmobid is inside request packet
#define SMSG_MOB_ATTACK_MOB 0xe1 // int mapmobid, byte mobAttackIdx, int damage
// 0xe2 does not exist
#define SMSG_NPC_SHOW 0xe3
#define SMSG_NPC_HIDE 0xe4
#define SMSG_NPC_CONTROL 0xe5
#define SMSG_NPC_ANIMATE 0xe6
#define SMSG_NPC_SHOW_EFFECT 0xe7
#define SMSG_NPC_SPECIAL_EFFECT 0xe8 // int mapnpcid, string action; Runs 'action' animation of the npc
#define SMSG_NPC_SET_SCRIPT 0xe9 // byte amount, foreach { int npcid, string scriptname, int startdate, int enddate }; Use this for setting the npc scripts from the server side (overriding any local settings)
// 0xea does not exist
#define SMSG_MERCHANT_SPAWN 0xeb // int mapmerchantid, int itemid?, short x, short y, short fh, string nameTag, byte roomType, if (roomType > 0) { int roomid, string text, byte spectators?, byte currentUsers, byte maxUsers }
#define SMSG_MERCHANT_DESPAWN 0xec // int mapmerchantid
#define SMSG_MERCHANT_UPDATE_BALLOON 0xed // int mapmerchantid, byte roomType, if (roomType > 0) { int roomid, string text, byte spectators?, byte currentUsers, byte maxUsers }
#define SMSG_DROP_ITEM 0xee
#define SMSG_DROP_PICKUP 0xef
#define SMSG_KITE_CREATE_FAILED 0xf0 // "You can't fly it here" O.o Might be from these kites
#define SMSG_KITE_SPAWN 0xf1 // int mapkiteid, int itemid, string message, string characterName, short x, short y
#define SMSG_KITE_DESPAWN 0xf2 // int mapkiteid
#define SMSG_MIST_SPAWN 0xf3
#define SMSG_MIST_DESPAWN 0xf4
#define SMSG_MYSTIC_DOOR_SPAWN 0xf5
#define SMSG_MYSTIC_DOOR_DESPAWN 0xf6
#define SMSG_REACTOR_TRIGGER 0xf7
// 0xf8 does not exist; would be 'SMSG_REACTOR_MOVE', but not implemented in client
#define SMSG_REACTOR_SPAWN 0xf9
#define SMSG_REACTOR_DESPAWN 0xfa
#define SMSG_MAP_SNOWBALL_STATE 0xfb // byte state, int snowman1HpPercent, int snowman2HpPercent, short snowball1xpos, byte snowball1hp / 1000, short snowball2xpos, byte snowball2hp / 1000, short snowballdmg, short snowman1dmg, short snowman2dmg
#define SMSG_MAP_SNOWBALL_HIT 0xfc // byte team, short damage, short delay
#define SMSG_MAP_SNOWBALL_MESSAGE 0xfd // byte team, byte message
#define SMSG_MAP_SNOWBALL_TOUCH 0xfe // Gives knockback to local user
#define SMSG_MAP_COCONUT_HIT 0xff // short target?, short delay, byte state
#define SMSG_MAP_COCONUT_UPDATE_SCORE 0x100 // short scoreTeam1, short scoreTeam2
#define SMSG_MAP_GUILDBOSS_HEALER_MOVE 0x101 // short yValue
#define SMSG_MAP_GUILDBOSS_PULLEY_STATE_CHANGE 0x102 // byte state
#define SMSG_MAP_MONSTERCARNIVAL_ENTER 0x103 // byte team, short ownRestCP, short ownTotalCP, short teamRestCP, short teamTotalCP, short ??, short ??
#define SMSG_MAP_MONSTERCARNIVAL_UPDATE_PLAYER_CP 0x104 // short restCP, short totalCP
#define SMSG_MAP_MONSTERCARNIVAL_UPDATE_TEAM_CP 0x105 // byte team, short restCP, short totalCP
#define SMSG_MAP_MONSTERCARNIVAL_REQUEST_RESULT_1 0x106 // byte ?, byte ?, string characterName
#define SMSG_MAP_MONSTERCARNIVAL_REQUEST_RESULT_2 0x107 // byte ?; Different kinds of responses, like 'You dont have enough cp'
#define SMSG_MAP_MONSTERCARNIVAL_PLAYER_KILLED 0x108 // byte team, string characterName, byte cpLost
#define SMSG_MAP_MONSTERCARNIVAL_PLAYER_LEAVE 0x109 // byte partyMemberIdx (6 == leader), byte team, string characterName; Shows message when player 'charactername' leaves, or that 'charactername' is new leader (partyMemberIdx = 6)
#define SMSG_MAP_MONSTERCARNIVAL_RESULT 0x10a // byte resultcode
#define SMSG_MAP_ARIANT_SCORES 0x10b // byte amount, foreach { string name, int score (max 9999) }; Updates Ariant score table
#define SMSG_HORNTAIL_CAVE 0x10c // Horntail Cave close warning. [0D 01] [SPAWNED (byte)] [TIME (int)] . If SPAWNED = 0 and TIME = 0 then cave is closed
#define SMSG_ZAKUM_SHRINE 0x10d // Zakum Shrine close warning. See SMSG_HORNTAIL_CAVE
#define SMSG_NPC_TALK 0x10e
#define SMSG_SHOP 0x10f
#define SMSG_ITEM_PURCHASED 0x110
// 0x111 is also admin shop, but not implemented
#define SMSG_ADMIN_SHOP 0x112 // int npcId, short items, foreach { int sn, int itemid, int price, byte saleState, short maxPerSlot }, bool askItemWishList (asks if you want to add something, on close)
#define SMSG_STORAGE 0x113
#define SMSG_FREDRICK_ERROR 0x114 // byte errorcode; Fredrick (item store) errors
#define SMSG_FREDRICK_OPEN 0x115 // byte action, check the client yourself; This packet is a bit more complicated to write here. Shows you your shop items when your merchant is closed
#define SMSG_ROCK_PAPER_SCSISSORS_GAME 0x116
#define SMSG_MESSENGER_ROOM 0x117 // Chatroom / Maple Messenger
#define SMSG_PLAYER_ROOM 0x118 // Trades, player shops, minigames
#define SMSG_MAP_TOURNAMENT_MESSAGE 0x119 // byte message; Things like 'Congratuations, you are the CHAMP!' are done here
#define SMSG_MAP_TOURNAMENT_MATCH_TABLE 0x11a // 32*6 bytes table, byte state
#define SMSG_MAP_TOURNAMENT_SET_PRICE 0x11b // For 'set was successfull': { bool true, bool false }. For item info dialog: { bool true, bool true, int priceItemId1, int priceItemId2 }. For error: { bool false, bool false }
#define SMSG_MAP_TOURNAMENT_MATCH_RESULT 0x11c // byte round; You reached round N etc.. etcc..
// 0x11d is empty
#define SMSG_MAP_WEDDING_PROGRESS 0x11e // byte step, int groomCharacterID, int brideCharacterID
#define SMSG_MAP_WEDDING_END 0x11f
#define SMSG_PACKAGE_DELIVERY 0x120 // Duey :D
#define SMSG_CASHSHOP_CHARGE_PARAM 0x121 // string key; Used for charging your account on the website
#define SMSG_CASHSHOP_NX 0x122
#define SMSG_CASHSHOP_OPERATION 0x123
#define SMSG_CASHSHOP_PURCHASE_EXP 0x124 // bool purchased; Should be true when a EXP card is bought
#define SMSG_CASHSHOP_NAMECHANGE_CHECK_NAME 0x125
#define SMSG_CASHSHOP_NAMECHANGE_RESULT 0x126 // int ignored, byte errorcode, int birthdate
// 0x127 does not exist
#define SMSG_CASHSHOP_WORLDTRANSFER_RESULT 0x128 // int ignored, byte errorcode, int birthdate, bool worlds, if (worlds) { int worldcount, foreach { string worldname } }
// 0x129 does not exist
#define SMSG_KEYMAP 0x12a
#define SMSG_SET_PET_HP_ITEM 0x12b
#define SMSG_SET_PET_MP_ITEM 0x12c



#define SMSG_MAPLETV_ON 0x130
#define SMSG_MAPLETV_OFF 0x131
#define SMSG_MAPLETV_SEND_RESULT 0x132 // if (success) { bool false } else { bool true, byte errorcode (0 = 'non gm char tried to send gm message', 1 = 'entered wrong username', 2 = 'waiting line longer than an hour') }


#define SMSG_MTS_CHARGE_PARAM 0x135 // string key; Used for charging your account on the website
#define SMSG_MTS_QUERY_CASH_RESULT 0x136
#define SMSG_MTS_NORMAL_ITEM_RESULT 0x137
#define SMSG_CHARACTERSALE_NAME_CHECK 0x138 // string name, byte errorcode; Name is ignored, it seems. if (errorcode < 0) { "Unknown error {errorcode}" } else if (errorcode > 0) { "name is being used" } else { something? }
#define SMSG_CHARACTERSALE_CREATE_RESULT 0x139 // byte resultcode, int errorcode; Shows dialog wether creation was okay. if (resultcode == 47) { if (errorcode != 0) { "Unknown error {errorcode}" } else { "Creation has completed" } }; errorcode 49 == "Cannot use the name", errorcode 48 == "Unknown error {errorcode}"


// 0x13c does not exist
#define SMSG_HAMMER 0x13d
// 0x13e does not exist