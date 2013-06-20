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

#include "MesyDAQIOTimer.h"

#include <Admin.h>
#include <iostream>

// TACODEVEL CODEGEN INCLUDES END

#include <TACOStringConverters.h>

#include "QMesydaqDetectorInterface.h"

#include "mdefines.h"

DevVoid MesyDAQ::IO::Timer::start() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Timer::start()" << log4cpp::eol;
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Timer::start()");
	}
}

DevVoid MesyDAQ::IO::Timer::stop() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Timer::stop()" << log4cpp::eol;
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Timer::stop()");
	}
}

DevVoid MesyDAQ::IO::Timer::setPreselection(const DevDouble input) throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Timer::setPreselection(" << input << ")" << log4cpp::eol;
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		m_interface->setPreSelection(TCT, input);
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Timer::setPreselection()");
	}
}

DevVoid MesyDAQ::IO::Timer::resume() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Timer::resume()" << log4cpp::eol;
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Timer::resume()");
	}
}

DevVoid MesyDAQ::IO::Timer::clear() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Timer::clear()" << log4cpp::eol;
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Timer::clear()");
	}
}

DevDouble MesyDAQ::IO::Timer::preselection() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Timer::preselection()" << log4cpp::eol;
	DevDouble tmp(0.0);
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		tmp = m_interface->preSelection(TCT);
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Timer::preselection()");
	}
	return tmp;
}

DevVoid MesyDAQ::IO::Timer::setMode(const DevLong input) throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Timer::setMode(" << input << ")" << log4cpp::eol;
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Timer::setMode()");
	}
}

DevLong MesyDAQ::IO::Timer::mode() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Timer::mode()" << log4cpp::eol;
	DevLong tmp(::IO::MODE_NORMAL);
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Timer::mode()");
	}
	return tmp;
}

DevVoid MesyDAQ::IO::Timer::enableMaster(const bool input) throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Timer::enableMaster(" << input << ")" << log4cpp::eol;
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		m_interface->selectCounter(TCT, input, preselection());
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Timer::enableMaster()");
	}
}

bool MesyDAQ::IO::Timer::isMaster() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Timer::isMaster()" << log4cpp::eol;
	bool tmp(false);
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		tmp = m_interface->counterSelected(TCT);
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Timer::isMaster()");
	}
	return tmp;
}

DevDouble MesyDAQ::IO::Timer::read() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::IO::Timer::read()" << log4cpp::eol;
	DevDouble tmp;
	try
	{
		if (!m_interface)
        		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		tmp = m_interface->readCounter(TCT);
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Timer::read()");
	}
	return tmp;
}

void MesyDAQ::IO::Timer::deviceInit(void) throw (::TACO::Exception)
{
// Please implement this for the startup 
	try
	{
		::TACO::Server::deviceUpdate(std::string());
	}
	catch (const ::TACO::Exception &e)
	{
		setDeviceState(::TACO::State::FAULT);
		throw e;
	}
	setDeviceState(::TACO::State::DEVICE_NORMAL);
#if 0
	::TACO::Server::deviceInit();
#endif
}

void MesyDAQ::IO::Timer::deviceReset(void) throw (::TACO::Exception)
{
	::TACO::Server::deviceReset();
}

DevShort MesyDAQ::IO::Timer::deviceState(void) throw (::TACO::Exception)
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

void MesyDAQ::IO::Timer::deviceUpdate(void) throw (::TACO::Exception)
{
	::TACO::Server::deviceUpdate();
}

void MesyDAQ::IO::Timer::deviceQueryResource(void) throw (::TACO::Exception)
{
	::TACO::Server::deviceQueryResource();
}
