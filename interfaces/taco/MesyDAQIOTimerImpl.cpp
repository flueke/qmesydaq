// Interface to the QMesyDAQ software
// Copyright (C) 2012-2014 Jens Krüger

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
	INFO_STREAM << "MesyDAQ::IO::Timer::start()" << ENDLOG;
	try
	{
		if (!m_interface)
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		if (isMaster() && deviceState() != ::TACO::State::COUNTING)
		{
			m_listFilename = incNumber(m_listFilename);
			updateResource<std::string>("lastlistfile", m_listFilename);
			m_interface->setListFileName(m_listFilename.c_str());
			m_interface->start();
		}
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Timer::start()");
	}
}

DevVoid MesyDAQ::IO::Timer::stop() throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::IO::Timer::stop()" << ENDLOG;
	try
	{
		if (!m_interface)
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		if (isMaster() && deviceState() == ::TACO::State::COUNTING)
			m_interface->stop();
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Timer::stop()");
	}
}

DevVoid MesyDAQ::IO::Timer::setPreselection(const DevDouble input) throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::IO::Timer::setPreselection(" << input << ")" << ENDLOG;
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
	INFO_STREAM << "MesyDAQ::IO::Timer::resume()" << ENDLOG;
	try
	{
		if (!m_interface)
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		if (isMaster() && deviceState() != ::TACO::State::COUNTING)
			m_interface->resume();
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Timer::resume()");
	}
}

DevVoid MesyDAQ::IO::Timer::clear() throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::IO::Timer::clear()" << ENDLOG;
	try
	{
		if (!m_interface)
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
		if (isMaster())
			m_interface->clear();
	}
	catch (::TACO::Exception &e)
	{
		throw_exception(e, "MesyDAQ::IO::Timer::clear()");
	}
}

DevDouble MesyDAQ::IO::Timer::preselection() throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::IO::Timer::preselection()" << ENDLOG;
	static DevDouble tmp(0.0);
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
	INFO_STREAM << "MesyDAQ::IO::Timer::setMode(" << input << ")" << ENDLOG;
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
	INFO_STREAM << "MesyDAQ::IO::Timer::mode()" << ENDLOG;
	static DevLong tmp(::IO::MODE_NORMAL);
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
	INFO_STREAM << "MesyDAQ::IO::Timer::enableMaster(" << input << ")" << ENDLOG;
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
	INFO_STREAM << "MesyDAQ::IO::Timer::isMaster()" << ENDLOG;
	static bool tmp(false);
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
	INFO_STREAM << "MesyDAQ::IO::Timer::read()" << ENDLOG;
	static DevDouble tmp(0.0);
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
	INFO_STREAM << "MesyDAQ::IO::Timer::deviceInit()" << ENDLOG;
// Please implement this for the startup
	try
	{
		::TACO::Server::deviceUpdate("lastlistfile");
		setDeviceState(::TACO::State::DEVICE_NORMAL);
	}
	catch (const ::TACO::Exception &e)
	{
		setDeviceState(::TACO::State::FAULT);
		throw;
	}
}

void MesyDAQ::IO::Timer::deviceReset(void) throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::IO::Timer::deviceReset()" << ENDLOG;
	::TACO::Server::deviceReset();
}

DevShort MesyDAQ::IO::Timer::deviceState(void) throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::IO::Timer::deviceState()" << ENDLOG;
	if (!m_interface)
		return ::TACO::State::FAULT;
	switch (m_interface->status())
	{
		case 1 :
			return ::TACO::State::COUNTING;
		case 0 :
		default:
			if (::TACO::Server::deviceState() == ::TACO::State::DEVICE_NORMAL)
				return ::TACO::State::PRESELECTION_REACHED;
			return ::TACO::Server::deviceState();
	}
}

void MesyDAQ::IO::Timer::deviceUpdate(void) throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::IO::Timer::deviceUpdate()" << ENDLOG;
	if (resourceUpdateRequest("lastlistfile"))
		try
		{
			m_listFilename = queryResource<std::string>("lastlistfile");
			if (m_listFilename == "")
				m_listFilename = "tacolistfile00000.mdat";
			if (!m_interface)
				throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
			m_interface->setListFileName(m_listFilename.c_str());
			INFO_STREAM << "LIST FILE " << m_listFilename << ENDLOG;
		}
		catch (::TACO::Exception &e)
		{
			throw_exception(e, "could not update 'lastlistfile' ");
		}
	::TACO::Server::deviceUpdate();
}

void MesyDAQ::IO::Timer::deviceQueryResource(void) throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::IO::Timer::deviceQueryResource()" << ENDLOG;
	::TACO::Server::deviceQueryResource();
	if (!m_interface)
	{
		makeResourceQuerySuccessful();
		return;
	}

	if (resourceQueryRequest("lastlistfile"))
		try
		{
			if (m_interface->getListFileName().isEmpty())
				m_interface->setListFileName(m_listFilename.c_str());
			updateResource<std::string>("lastlistfile", m_interface->getListFileName().toStdString());
		}
		catch (TACO::Exception &e)
		{
			throw_exception(e, "Could not query resource 'lastlistfile' ");
		}
}

std::string MesyDAQ::IO::Timer::incNumber(const std::string &val)
{
	std::string tmpString = val;
	std::string baseName = basename(const_cast<char *>(tmpString.c_str()));
	std::string::size_type pos = baseName.rfind(".");
	std::string ext("");
	if (pos == std::string::npos)
		ext = ".mdat";
	else
	{
		ext = baseName.substr(pos);
		baseName.erase(pos);
	}
	DevLong currIndex(0);
	if (baseName.length() > 5)
	{
		currIndex = strtol(baseName.substr(baseName.length() - 5).c_str(), NULL, 10);
		if (currIndex)
			baseName.erase(baseName.length() - 5);
	}
	std::string tmp = ::TACO::numberToString(++currIndex, 5);
	pos = tmpString.find(baseName);
	pos += baseName.length();
	tmpString.erase(pos);
	for (int i = tmp.length(); i < 5; ++i)
		tmpString += '0';
	tmpString += tmp;
	tmpString += ext;
	return tmpString;
}
