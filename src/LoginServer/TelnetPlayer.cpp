/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "TelnetPlayer.h"
#include "Database.h"
#include "LoginServer.h"
#include "IpUtilities.h"
#include "MiscUtilities.h"
#include "Randomizer.h"
#include "StringUtilities.h"
#include "World.h"
#include "Worlds.h"
#include <boost/lexical_cast.hpp>
#include <boost/tr1/regex.hpp>
#include <iostream>
#include <stdexcept>

using boost::lexical_cast;
using std::tr1::cmatch;
using std::tr1::regex;
using std::tr1::regex_match;
using StringUtilities::atob;

TelnetPlayer::~TelnetPlayer() {
	if (m_loggedOn) {
		LoginServer::Instance()->log(LogTypes::Login, "Administrator logged off from the telnet server.");
	}
}

void TelnetPlayer::sendConnectedMessage() {
	getSession()->send("Welcome to the LoginServer Telnet server.");
	getSession()->send("Please login with your user credentials.");
	getSession()->send("Username: ", false);
}

void TelnetPlayer::realHandleRequest(const string &data) {
	char *chat = const_cast<char *>(data.c_str());
	string command = strtok(chat, " ");
	string args = data.length() > command.length() + 1 ? data.substr(command.length() + 1) : "";
	regex re; // Regular expression for use by commands with more complicated structures
	cmatch matches; // Regular expressions match for such commands

	if (!m_loggedOn) {
		if (!m_gotUsername) {
			string username = command;
			if (username.length() < 4 || username.length() > 12) {
				getSession()->send("Invalid username length.");
				getSession()->send("Please provide your username: ", false);
			}
			else {
				m_username = username;
				m_gotUsername = true;
				getSession()->SetRemoveLastCharacter(true);
				getSession()->send("Password: ", false);
			}
		}
		else if (!m_gotPass) {
			string password = command;
			if (password.length() < 4 || password.length() > 12) {
				getSession()->send("Invalid password length.");
				getSession()->send("Please provide your password: ", false);
			}
			else {
				mysqlpp::Query query = Database::getCharDB().query();
				query << "SELECT id, salt, password, admin FROM users WHERE username = " << mysqlpp::quote << m_username << " AND ban_expire < NOW()";
				mysqlpp::StoreQueryResult res = query.store();
				if (!res.empty()) {
					if (res[0]["salt"].is_null()) {
						// We have an unsalted password here
						if (res[0]["password"] == password) {
							// We have a valid password here, so let's hash the password
							string salt = Randomizer::Instance()->generateSalt(10);
							string hashed_pass = MiscUtilities::hashPassword(password, password);
							query << "UPDATE users SET password = " << mysqlpp::quote << hashed_pass << ", salt = " << mysqlpp::quote << salt << " WHERE id = " << res[0]["id"];
							query.exec();
							
							getSession()->send("Credentials accepted. Please provide the LoginServer telnet password.");
							getSession()->send("Password: ", false);
							m_gotPass = true;
						}
						else {
							getSession()->send("You've entered either an invalid username or password. Your will be disconnected from the server.");
							getSession()->disconnect();
							LoginServer::Instance()->log(LogTypes::LoginAuthFailure, "User tried to login into the telnet server and provided an incorrect password. Connection terminated. Username: " + m_username);
						}
					}
					else if (res[0]["password"] != MiscUtilities::hashPassword(password, string(res[0]["salt"].data()))) {
						getSession()->send("You've entered either an invalid username or password. Your will be disconnected from the server.");
						getSession()->disconnect();

						LoginServer::Instance()->log(LogTypes::LoginAuthFailure, "User tried to login into the telnet server and provided an incorrect password. Connection terminated. Username: " + m_username);
					}
					else if (!atob(res[0]["admin"])) {
						getSession()->send("You cannot access the server with a non admin account.");
						getSession()->disconnect();

						LoginServer::Instance()->log(LogTypes::LoginAuthFailure, "User tried to login into the telnet server with a non admin account. Username: " + m_username);
					}
					else {
						getSession()->send("Credentials accepted. Please provide the LoginServer telnet password.");
						getSession()->send("Password: ", false);
						m_gotPass = true;
					}
				}
				else {
					LoginServer::Instance()->log(LogTypes::LoginAuthFailure, "User tried to login into the telnet server with a non existing account or a banned account. Username: " + m_username);
					getSession()->send("This user does not exist or is banned.");
					getSession()->disconnect();
				}
			}
		}
		else {
			if (command == LoginServer::Instance()->getTelnetPassword()) {
				m_loggedOn = true;
				getSession()->SetRemoveLastCharacter(false);
				getSession()->send("You are successfully logged on on the LoginServer telnet server!");
				getSession()->send("What do you want to do?");
				getSession()->send("> ", false, false);
				LoginServer::Instance()->log(LogTypes::Login, "Admin user logged on via telnet. Username: " + m_username);
			}
			else {
				getSession()->send("Incorrect telnet password.");
				getSession()->disconnect();
				LoginServer::Instance()->log(LogTypes::LoginAuthFailure, "User enter an incorrect telnet password. Username: " + m_username + ", Entered password: " + command);
			}
		}
	}
	else {
		if (command == "shutdown") {
			LoginServer::Instance()->log(LogTypes::Info, "Server shutting down by telnet command. Admin: " + m_username);
			getSession()->send("Server is shutting down.");
			LoginServer::Instance()->shutdown();
		}
		else if (command == "seteventmessage") {
			re = "(\\d+) ?(.+)?";
			if (regex_match(args.c_str(), matches, re)) {
				uint8_t worldid = atoi(string(matches[1]).c_str());
				string message = matches[2];
				if (World *world = Worlds::Instance()->getWorld(worldid)) {
					world->setEventMessage(message);
					getSession()->send("World event message set!");
				}
				else {
					getSession()->send("World server with ID " + boost::lexical_cast<string>(static_cast<int16_t>(worldid)) + " not found.");
				}
			}
			else {
				getSession()->send("Syntax: seteventmessage (worldid) {message}");
			}
		}
		else if (command == "geteventmessage") {
			re = "(\\d+)";
			if (regex_match(args.c_str(), matches, re)) {
				uint8_t worldid = atoi(string(matches[1]).c_str());
				if (World *world = Worlds::Instance()->getWorld(worldid)) {
					string message = world->getEventMessage();
					if (message.empty()) {
						getSession()->send("No event message set on this world.");
					}
					else {
						getSession()->send("World event message:\r\n" + message);
					}
				}
				else {
					getSession()->send("World server with ID " + boost::lexical_cast<string>(static_cast<int16_t>(worldid)) + " not found.");
				}
			}
			else {
				getSession()->send("Syntax: geteventmessage (worldid)");
			}
		}
		else if (command == "disconnectworld") {
			re = "(\\d+)";
			if (regex_match(args.c_str(), matches, re)) {
				uint8_t worldid = atoi(string(matches[1]).c_str());
				if (World *world = Worlds::Instance()->getWorld(worldid)) {
					if (world->isConnected()) {
						world->getConnection()->getSession()->disconnect();
						getSession()->send("Worldserver disconnected!");
					}
					else {
						getSession()->send("Worldserver isn't connected.");
					}
				}
				else {
					getSession()->send("Worldserver with ID " + boost::lexical_cast<string>(static_cast<int16_t>(worldid)) + " not found.");
				}
			}
			else {
				getSession()->send("Syntax: disconnectworld (worldid)");
			}
		}
		else if (command == "onlinelist") {
			mysqlpp::Query query = Database::getCharDB().query();
			query << "SELECT characters.name, users.online FROM characters LEFT JOIN users ON users.id = characters.userid WHERE users.online <> 0 AND characters.online <> 0";
			mysqlpp::StoreQueryResult res = query.store();

			if (res.empty()) {
				getSession()->send("There are no characters online at the moment.");
			}
			else {
				std::stringstream x;
				x << "List of online characters:" << "\r\n";
				x << "+--------------+----------+----------------+\r\n";
				x << "| " << std::setw(12) << std::left << "Name" << " | ";
				x << std::setw(8) << std::left << "World ID" << " | ";
				x << std::setw(14) << std::left << "Channel" << " |\r\n";
				x << "+--------------+----------+----------------+\r\n";

				string name;
				for (size_t i = 0; i < res.num_rows(); i++) {
					int16_t onlineid = atoi(res[i][1]) - 20000;
					int16_t world = onlineid / 100;
					int16_t channel = onlineid % 100;
					x << "| " << std::setw(12) << std::left << res[i][0].c_str() << " | ";
					x << std::setw(8) << std::left << world << " | ";
					x << std::setw(14) << std::left;
					if (channel == 50) {
						x << "Cashshop";
					}
					else {
						x << channel;
					}
					x << " |\r\n";
				}
				x << "+--------------+----------+----------------+\r\n";
				getSession()->send(x.str());
			}
		}
		else if (command == "banlist") {
			mysqlpp::Query query = Database::getCharDB().query();
			query << "SELECT username, ban_expire, ban_reason FROM users WHERE ban_expire > NOW()";
			mysqlpp::StoreQueryResult res = query.store();

			if (res.empty()) {
				getSession()->send("Nobody is banned at the moment.");
			}
			else {
				std::stringstream x;
				x << "Current banlist:" << "\r\n";
				x << "+--------------+---------------------+------------+\r\n";
				x << "| " << std::setw(12) << std::left << "Username" << " | ";
				x << std::setw(19) << std::left << "Ban expire" << " | ";
				x << "Ban reason" << " |\r\n";
				x << "+--------------+---------------------+------------+\r\n";

				string name;
				for (size_t i = 0; i < res.num_rows(); i++) {
					x << "| " << std::setw(12) << std::left << res[i][0].c_str() << " | ";
					x << std::setw(19) << std::left << mysqlpp::DateTime(res[i][1]).str() << " | ";
					x << std::setw(10) << (int16_t)res[i][2] << " |\r\n";
				}
				x << "+--------------+---------------------+------------+\r\n";
				getSession()->send(x.str());
			}
		}
		else if (command == "ipbanlist") {
			mysqlpp::Query query = Database::getCharDB().query();
			query << "SELECT id, ip FROM ipbans";
			mysqlpp::StoreQueryResult res = query.store();

			if (res.empty()) {
				getSession()->send("No IPs are banned at the moment.");
			}
			else {
				std::stringstream x;
				x << "Current banlist:" << "\r\n";
				x << std::setw(5) << std::left << "ID" << " | ";
				x << "IP" << "\r\n";

				string name;
				for (size_t i = 0; i < res.num_rows(); i++) {
					x << std::setw(5) << std::left << atoi(res[i][0]) << " | ";
					x << res[i][1].c_str() << "\r\n";
				}
				getSession()->send(x.str());
			}
		}
		else if (command == "banuser") {
			re = "(\\w+) (\\d+) ?(\\w+)?";
			if (regex_match(args.c_str(), matches, re)) {
				string targetname = matches[1];

				string reasonstring = matches[2];
				int8_t reason = reasonstring.length() > 0 ? atoi(reasonstring.c_str()) : 1;
				string length = matches[3];
				length = length.length() > 0 ? "DATE_ADD(NOW(), INTERVAL " + length + " DAY)" : "'9000-00-00 00:00:00'";

				mysqlpp::Query query = Database::getCharDB().query();
				query << "UPDATE users SET ban_reason = " << (int16_t) reason << ", ban_expire = " << length << " WHERE username = " << mysqlpp::quote << targetname;
				query.exec();
				if (query.affected_rows() == 1) {
					getSession()->send("This account is now banned.");
				}
				else {
					getSession()->send("Account not found.");
				}
			}
			else {
				getSession()->send("Syntax: banuser (username) (reason) {time in days}");
			}
		}
		else if (command == "unbanuser") {
			if (!args.empty()) {
				mysqlpp::Query query = Database::getCharDB().query();
				query << "UPDATE users SET ban_expire = '0000-00-00 00:00:00' WHERE username = " << mysqlpp::quote << args;
				query.exec();
				if (query.affected_rows() >= 1) {
					getSession()->send("Account unbanned!");
				}
				else {
					getSession()->send("This account is not banned.");
				}
			}
			else {
				getSession()->send("Syntax: unbanuser (username)");
			}
		}
		else if (command == "bancharacter") {
			re = "(\\w+) (\\d+) ?(\\w+)?";
			if (regex_match(args.c_str(), matches, re)) {
				string targetname = matches[1];

				string reasonstring = matches[2];
				int8_t reason = reasonstring.length() > 0 ? atoi(reasonstring.c_str()) : 1;
				string length = matches[3];
				length = length.length() > 0 ? "DATE_ADD(NOW(), INTERVAL " + length + " DAY)" : "'9000-00-00 00:00:00'";

				mysqlpp::Query query = Database::getCharDB().query();
				query << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.ban_reason = " << (int16_t) reason << ", users.ban_expire = " << length << " WHERE characters.name = " << mysqlpp::quote << targetname;
				query.exec();
				if (query.affected_rows() == 1) {
					getSession()->send("The account of the character is now banned.");
				}
				else {
					getSession()->send("Character not found.");
				}
			}
			else {
				getSession()->send("Syntax: bancharacter (username) (reason) {time in days}");
			}
		}
		else if (command == "unbancharacter") {
			if (!args.empty()) {
				mysqlpp::Query query = Database::getCharDB().query();
				query << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.ban_expire = '0000-00-00 00:00:00' WHERE characters.name = " << mysqlpp::quote << args;
				query.exec();
				if (query.affected_rows() >= 1) {
					getSession()->send("Character's account unbanned!");
				}
				else {
					getSession()->send("This character is not banned.");
				}
			}
			else {
				getSession()->send("Syntax: unbancharacter (username)");
			}
		}
		else if (command == "banip") {
			if (!args.empty()) {
				mysqlpp::Query query = Database::getCharDB().query();
				query << "INSERT INTO ipbans VALUES (NULL, " << mysqlpp::quote << args << ");";
				query.exec();
				getSession()->send("This IP is now banned!");
			}
			else {
				getSession()->send("Syntax: banip (ip)");
			}
		}
		else if (command == "unbanip") {
			if (!args.empty()) {
				mysqlpp::Query query = Database::getCharDB().query();
				query << "DELETE FROM ipbans WHERE ip = " << mysqlpp::quote << args;
				query.exec();
				if (query.affected_rows() >= 1) {
					getSession()->send("IP unbanned!");
				}
				else {
					getSession()->send("This IP is not banned.");
				}
			}
			else {
				getSession()->send("Syntax: unbanip (ip)");
			}
		}
		else if (command == "help") {
			getSession()->send("Commands:");
			getSession()->send("help - Shows this.");
			getSession()->send("exit - Disconnects you from the server.");
			getSession()->send("shutdown - Shuts down the server.");
			getSession()->send("seteventmessage (world id) {message} - Set the event message of a world. If the message argument is not set, the event message will be removed.");
			getSession()->send("geteventmessage (world id) - Shows you the event message of a world.");
			getSession()->send("disconnectworld (world id) - Disconnects a world from the loginserver.");
			getSession()->send("onlinelist - Displays a list of online characters, including world ID and channel.");
			getSession()->send("banlist - Shows a list of banned accounts, including ban expire and reason.");
			getSession()->send("ipbanlist - Shows a list of banned IP addresses.");
			getSession()->send("banuser (username) (reason) {time in days} - Bans a user. The {time in days} argument is optional and if it is not set, it will perm ban the account.");
			getSession()->send("unbanuser (username) - Unbans a user.");
			getSession()->send("bancharacter (charname) (reason) {time in days} - Bans a character. The {time in days} argument is optional and if it is not set, it will perm ban the character.");
			getSession()->send("unbancharacter (charname) - Unbans a character.");
			getSession()->send("banip (IP address) - Bans an IP address.");
			getSession()->send("unbanip (IP address) - Unbans an IP address.");
		}
		else {
			getSession()->send("Unknown command: " + data);
		}
		getSession()->send("> ", false, false);
	}
}