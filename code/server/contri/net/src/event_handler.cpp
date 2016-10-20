

#include "event_handler.h"
#include <assert.h>


// class Event_Handler
Event_Handler::Event_Handler()
{
	m_reactor = NULL;
}

Event_Handler::~Event_Handler()
{
}

Reactor* Event_Handler::reactor()
{
	return m_reactor;
}

void Event_Handler::reactor(Reactor* reactor)
{
	m_reactor = reactor;
}

