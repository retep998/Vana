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
#include "TelnetSession.h"
#include "AbstractTelnetConnection.h"
#include "Database.h"
#include "IpUtilities.h"
#include "SessionManager.h"
#include <boost/bind.hpp>
#include <iostream>

TelnetSession::TelnetSession(boost::asio::io_service &io_service,
						   SessionManagerPtr sessionManager,
						   AbstractTelnetConnection *player) :
AbstractSession(sessionManager),
m_socket(io_service),
m_player(player),
m_removeLastChar(false)
{
}

void TelnetSession::start() {
	m_sessionManager->start(shared_from_this());
}

void TelnetSession::handle_start() {
	m_player->setSession(this);
	m_player->setIp(m_socket.remote_endpoint().address().to_v4().to_ulong());

	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM ipbans WHERE ip = " << mysqlpp::quote << IpUtilities::ipToString(getIp());
	mysqlpp::StoreQueryResult store = query.store();

	if (store.num_rows() != 0) {
		send("THIS IP IS BANNED.");
		disconnect();
	}
	else {
		m_inputStream.clear();
		m_inputStream.str("");
		m_inputStreamSize = 0;
		m_player->sendConnectedMessage();
		start_read();
	}
}

void TelnetSession::stop() {
	m_sessionManager->stop(shared_from_this());
}

void TelnetSession::disconnect() {
	stop();
}

void TelnetSession::handle_stop() {
	boost::system::error_code ec;
	m_socket.close(ec);
	if (ec) {
		std::cout << "FAILURE TO CLOSE TELNET SESSION (" << ec.value() << "): " << ec.message() << std::endl;
	}
}

void TelnetSession::send(const unsigned char *buf, int32_t len) {
	boost::mutex::scoped_lock l(m_sendMutex);
	unsigned char *buffer = new unsigned char[len];
	m_sendPacket.reset(buffer);

	memcpy(buffer, buf, len);
	boost::asio::async_write(m_socket, boost::asio::buffer(buffer, len),
		boost::bind(&TelnetSession::handle_write, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void TelnetSession::send(const string &data, bool newline, bool starttext) {
	std::string x;
	if (starttext) {
		x += "Server> ";
	}
	x += data;
	if (newline) {
		x += "\r\n";
	}
	send(reinterpret_cast<const unsigned char*>(x.c_str()), x.length());
}

void TelnetSession::send() {
	std::string x;
	x = "\r\n";
	send(reinterpret_cast<const unsigned char*>(x.c_str()), x.length());
}

void TelnetSession::start_read() {
	m_buffer.reset(new unsigned char[1]);

	boost::asio::async_read(m_socket,
		boost::asio::buffer(m_buffer.get(), 1),
		boost::bind(
			&TelnetSession::handle_read, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void TelnetSession::handle_write(const boost::system::error_code &error, size_t bytes_transferred) {
	boost::mutex::scoped_lock l(m_sendMutex);
	if (error) {
		disconnect();
	}
}

void TelnetSession::handle_read(const boost::system::error_code &error, size_t bytes_transferred) {
	if (!error) {
		if (m_buffer[0] == Backspace) {
			// Deleting a character.
			if (m_inputStreamSize > 0) {
				string data = m_inputStream.str();
				data = data.substr(0, m_inputStreamSize - 1);
				m_inputStream.clear();
				m_inputStream.str("");
				m_inputStream << data;
				m_inputStreamSize--;
				if (m_removeLastChar) {
					// Enter space instead.
					send(" ", false, false);
				}
			}
			else {
				// Lol ur doing it rong, nothing to delete!
				// So let the user hear that he's doing it rong.
				send("\a ", false, false);
			}
		}
		else if (m_buffer[0] == LineFeed /*0x0A*/ || m_buffer[0] == CarriageReturn /*0x0D*/) {
			// Windows sends: 0A 0D (\r\n)
			// Mac sends: 0A 00 (\r)
			// Linux sends: 0D 00 (\n)
			string data = m_inputStream.str();
			if (!data.empty()) {
				m_player->handleRequest(data);
			}
			m_inputStream.clear();
			m_inputStream.str("");
			m_inputStreamSize = 0;
		}
		else if (m_inputStreamSize >= maxBufferLen) {
			disconnect();
			return;
		}
		else {
			if ((m_buffer[0] >= 0x20 && m_buffer[0] <= 0x7e) || (m_buffer[0] >= 0x80 && m_buffer[0] <= 0xa5)) {
				++m_inputStreamSize;
				m_inputStream << m_buffer[0];
				if (m_removeLastChar) {
					// Uncomment for star replacement:
					//  send("\b*", false, false);
					// This will remove the character and show nothing (safest)
					send("\b \b", false, false);
				}
			}
			else {
				// Unknown character; remove.
				// Let the user hear a beep, then remove the character.
				send("\a\b ", false, false);
			}
		}
		start_read();
	}
	else {
		disconnect();
	}
}

uint32_t TelnetSession::getIp() const {
	return m_player->getIp();
}

void TelnetSession::SetRemoveLastCharacter(bool on) {
	this->m_removeLastChar = on;
}