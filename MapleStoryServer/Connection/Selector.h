#ifndef SELECTOR_H
#define SELECTOR_H

#include <Winsock2.h>
#include <hash_map>

using namespace stdext;

class Selector {
public:

	class SelectHandler {
	public:
		virtual void handle (Selector* selector, int socket) = 0;
	};

	Selector ();
	~Selector();

	void registerSocket 
		(int socket, 
		bool selectRead, bool selectWrite, bool selectError, 
		SelectHandler* handler);

	void unregisterSocket (int socket);
	void selectThread();

private:
	bool terminate;
	fd_set readfds;
    fd_set writefds;
	fd_set errorfds;
	struct timeval timeout;
	hash_map<int, SelectHandler*> handlers;
};

#endif