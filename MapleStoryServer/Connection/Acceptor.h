#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "Selector.h"
#include "AbstractPlayer.h"

class Acceptor: public Selector::SelectHandler {
public:

	Acceptor (int port, Selector* selector, AbstractPlayerFactory* apf);
	virtual void handle (Selector* selector, int socket);

protected:
	AbstractPlayerFactory* abstractPlayerFactory;
};

#endif