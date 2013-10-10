// Interface to the QMesyDAQ software
// Copyright (C) 2012 Jens Krüger

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

// TACODEVEL CODEGEN INCLUDES BEGIN
// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif // HAVE_CONFIG_H

#include <TACOConverters.h>
#include <TACOCommands.h>

#include "MesyDAQIOCounter.h"

#include <Admin.h>
#include <iostream>

// TACODEVEL CODEGEN INCLUDES END

#include <TACOStringConverters.h>

#include "QMesydaqDetectorInterface.h"

DevVoid MesyDAQ::IO::Counter::start() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Counter::start()" << log4cpp::eol;
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Counter::start()");
	}
}

DevVoid MesyDAQ::IO::Counter::stop() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Counter::stop()" << log4cpp::eol;
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Counter::stop()");
	}
}

DevVoid MesyDAQ::IO::Counter::resume() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Counter::resume()" << log4cpp::eol;
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Counter::resume()");
	}
}

DevVoid MesyDAQ::IO::Counter::clear() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Counter::clear()" << log4cpp::eol;
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Counter::clear()");
	}
}

DevVoid MesyDAQ::IO::Counter::setMode(const DevLong input) throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Counter::setMode(" << input << ")" << log4cpp::eol;
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Counter::setMode()");
	}
}

DevLong MesyDAQ::IO::Counter::mode() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Counter::mode()" << log4cpp::eol;
	DevLong tmp(::IO::MODE_NORMAL);
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Counter::mode()");
	}
	return tmp;
}

DevDouble MesyDAQ::IO::Counter::timeBase() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Counter::timeBase()" << log4cpp::eol;
	DevDouble tmp(::IO::TIME_BASE_NONE);
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Counter::timeBase()");
	}
	return tmp;
}

DevVoid MesyDAQ::IO::Counter::setTimeBase(const DevDouble input) throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Counter::setTimeBase(" << input << ")" << log4cpp::eol;
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		if (input != ::IO::TIME_BASE_NONE)
			throw TACO::Exception( TACO::Error::RUNTIME_ERROR, "invalid time base");
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Counter::setTimeBase()");
	}
}

DevVoid MesyDAQ::IO::Counter::enableMaster(const bool input) throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Counter::enableMaster(" << input << ")" << log4cpp::eol;
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		m_interface->selectCounter(m_channel, input, preselection());
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Counter::enableMaster()");
	}
}

bool MesyDAQ::IO::Counter::isMaster() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Counter::isMaster()" << log4cpp::eol;
	bool tmp(false);
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		tmp = m_interface->counterSelected(m_channel);
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Counter::isMaster()");
	}
	return tmp;
}

DevULong MesyDAQ::IO::Counter::read() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Counter::read()" << log4cpp::eol;
	DevULong tmp(0);
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		tmp = m_interface->readCounter(m_channel);
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Counter::read()");
	}
	return tmp;
}

DevULong MesyDAQ::IO::Counter::preselection() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Counter::preselection()" << log4cpp::eol;
	DevULong tmp(0.0);
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		tmp = m_interface->preSelection(m_channel);
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Counter::preselection()");
	}
	return tmp;
}

DevVoid MesyDAQ::IO::Counter::setPreselection(const DevULong input) throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Counter::setPreselection(" << input << ")" << log4cpp::eol;
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		m_interface->setPreSelection(m_channel, input);
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Counter::setPreselection()");
	}
}

void MesyDAQ::IO::Counter::deviceInit(void) throw (::TACO::Exception)
{
// Please implement this for the startup 
	try
	{
		::TACO::Server::deviceUpdate("channel");
	}
	catch (const ::TACO::Exception &e)
	{
		setDeviceState(::TACO::State::FAULT);
		throw;
	}
	setDeviceState(::TACO::State::DEVICE_NORMAL);
#if 0
	::TACO::Server::deviceInit();
#endif
}

void MesyDAQ::IO::Counter::deviceReset(void) throw (::TACO::Exception)
{
	::TACO::Server::deviceReset();
}

DevShort MesyDAQ::IO::Counter::deviceState(void) throw (::TACO::Exception)
{
	if(!m_interface)
		return ::TACO::State::FAULT;
	switch (m_interface->status())
	{
		case 1 :
			return ::TACO::State::COUNTING;
		default:
		case 0 :
			return ::TACO::Server::deviceState();
	}
}

void MesyDAQ::IO::Counter::deviceUpdate(void) throw (::TACO::Exception)
{
        if (resourceUpdateRequest("channel"))
                try
                {
                        DevULong tmp = queryResource<DevULong>("channel");
			if (tmp > 4)
				throw ::TACO::Exception(::TACO::Error::INVALID_VALUE, "channel must be [0..3] for monitor/choppers or 4 for events");
                        m_channel = tmp;
                }
                catch (::TACO::Exception &e)
                {
                        throw "could not update 'channel' " >> e;
                }
	::TACO::Server::deviceUpdate();
}

void MesyDAQ::IO::Counter::deviceQueryResource(void) throw (::TACO::Exception)
{
        if (resourceQueryRequest("channel"))
                try
                {
                        updateResource<DevULong>("channel", m_channel);
                }
                catch (TACO::Exception &e)
                {
                        throw "Could not query resource 'channel' " >> e;
                }

	::TACO::Server::deviceQueryResource();
}
