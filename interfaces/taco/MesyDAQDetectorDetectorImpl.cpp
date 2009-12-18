// Interface to the QMesyDAQ software
// Copyright (C) 2009 Jens Krüger

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

#include "MesyDAQDetectorDetector.h"

#include <Admin.h>
#include <iostream>

// TACODEVEL CODEGEN INCLUDES END

#include <TACOStringConverters.h>

#include "tacocontrol.h"

DevVoid MesyDAQ::Detector::Detector::start() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::Detector::Detector::start()" << log4cpp::eol;
	if (!m_pInt)
		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	reinterpret_cast<TACOControl *>(m_pInt)->setListFileName(incNumber("lastlistfile", "tacolistfile") + ".mdat");
	m_pInt->start();
}

DevVoid MesyDAQ::Detector::Detector::stop() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::Detector::Detector::stop()" << log4cpp::eol;
	if (!m_pInt)
		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	m_pInt->stop();
}

DevVoid MesyDAQ::Detector::Detector::resume() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::Detector::Detector::resume()" << log4cpp::eol;
	if (!m_pInt)
		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	m_pInt->cont();
}

DevVoid MesyDAQ::Detector::Detector::clear() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::Detector::Detector::clear()" << log4cpp::eol;
	if (!m_pInt)
		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	m_pInt->clear();
}

#warning TODO preselection values
DevVoid MesyDAQ::Detector::Detector::setPreselection(const DevDouble input) throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::Detector::Detector::setPrelection(" << input << ")" << log4cpp::eol;
	if (!m_pInt)
		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	m_pInt->setTimePreselection(input);
}

DevDouble MesyDAQ::Detector::Detector::preselection() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::Detector::Detector::preselection()" << log4cpp::eol;
	if (!m_pInt)
		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	return m_pInt->timePreselection();
}

const std::vector<DevULong> &MesyDAQ::Detector::Detector::read() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::Detector::Detector::read()" << log4cpp::eol;
	if (!m_pInt)
		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	throw ::TACO::Exception( ::TACO::Error::COMMAND_NOT_IMPLEMENTED);
}

void MesyDAQ::Detector::Detector::v_Init(void) throw (::TACO::Exception)
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
}

DevShort MesyDAQ::Detector::Detector::deviceState(void) throw (::TACO::Exception)
{
	if (!m_pInt)
		return ::TACO::State::FAULT;
	switch (m_pInt->status())
	{
		case 1 :
			return ::TACO::State::COUNTING;
		default:
		case 0 :
			return ::TACO::Server::deviceState();
	}
}

void MesyDAQ::Detector::Detector::setControlInterface(ControlInterface *pInt)
{
	m_pInt = pInt;
}

std::string MesyDAQ::Detector::Detector::incNumber(std::string resource, std::string filename)
{
	std::string tmpString = queryResource<std::string>("lastlistfile");
	if (tmpString.empty())
		tmpString = filename;
	DevLong currIndex = strtol(tmpString.substr(tmpString.length() - 5).c_str(), NULL, 10);
	if (currIndex)
		tmpString.erase(tmpString.length() - 5);
	currIndex += 1;
	std::string tmp = ::TACO::numberToString(currIndex, 5);
	for (int i = tmp.length(); i < 5; ++i)
		tmpString += '0';
	tmpString += tmp;
	updateResource<std::string>("lastlistfile", tmpString);
	return tmpString;
}
