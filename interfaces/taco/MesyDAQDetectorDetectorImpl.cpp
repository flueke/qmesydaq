// Interface to the QMesyDAQ software
// Copyright (C) 2009-2010 Jens Krüger

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

#include "QMesydaqDetectorInterface.h"

#include "libgen.h"

DevVoid MesyDAQ::Detector::Detector::start() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::Detector::Detector::start()" << log4cpp::eol;

	if (!m_interface)
        	throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");

	m_listFilename = incNumber();
	updateResource<std::string>("lastlistfile", m_listFilename);
    	m_interface->setListFileName(m_listFilename.c_str());
	m_interface->start();
}

DevVoid MesyDAQ::Detector::Detector::stop() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::Detector::Detector::stop()" << log4cpp::eol;

	if (!m_interface)
        	throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
        m_interface->stop();
}

DevVoid MesyDAQ::Detector::Detector::resume() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::Detector::Detector::resume()" << log4cpp::eol;

	if (!m_interface)
        	throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
	m_interface->resume();
}

DevVoid MesyDAQ::Detector::Detector::clear() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::Detector::Detector::clear()" << log4cpp::eol;

	if (!m_interface)
        	throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
        m_interface->clear();
}

DevVoid MesyDAQ::Detector::Detector::setPreselection(const DevDouble input) throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::Detector::Detector::setPrelection(" << input << ")" << log4cpp::eol;

	if (!m_interface)
        	throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
        m_interface->setPreSelection(input);
#warning TODO preselection values
}

DevDouble MesyDAQ::Detector::Detector::preselection() throw (::TACO::Exception)
{
	logStream->infoStream() << "MesyDAQ::Detector::Detector::preselection()" << log4cpp::eol;

	if (!m_interface)
        	throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
        return m_interface->preSelection();
#warning TODO preselection values
}

const std::vector<DevULong> &MesyDAQ::Detector::Detector::read() throw (::TACO::Exception)
{
	static std::vector<DevULong> tmp;
	logStream->infoStream() << "MesyDAQ::Detector::Detector::read()" << log4cpp::eol;

	if (!m_interface)
        	throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
        tmp = m_interface->read();
// 1 1 1 value
	return tmp;
}

void MesyDAQ::Detector::Detector::v_Init(void) throw (::TACO::Exception)
{
	// Please implement this for the startup
	try
	{
		Server::deviceUpdate("lastlistfile");
	}
	catch (const ::TACO::Exception &e)
	{
//		setDeviceState(::TACO::State::FAULT);
//		throw e;
	}
}

DevShort MesyDAQ::Detector::Detector::deviceState(void) throw (::TACO::Exception)
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

void MesyDAQ::Detector::Detector::deviceUpdate(void) throw (::TACO::Exception)
{
        if (resourceUpdateRequest("lastlistfile"))
                try
                {
                        m_listFilename = queryResource<std::string>("lastlistfile");
			if (m_listFilename == "")
				m_listFilename = "tacolistfile00000.mdat";
			if (!m_interface)
        			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
                        m_interface->setListFileName(m_listFilename.c_str());
                }
                catch (::TACO::Exception &e)
                {
                        throw "could not update 'lastlistfile' " >> e;
                }
}

void MesyDAQ::Detector::Detector::deviceQueryResource(void) throw (::TACO::Exception)
{
        if (resourceQueryRequest("lastlistfile"))
                try
                {
			if (m_interface->getListFileName().isEmpty())
				m_interface->setListFileName(m_listFilename.c_str());
                        updateResource<std::string>("lastlistfile", m_interface->getListFileName().toStdString());
                }
                catch (TACO::Exception &e)
                {
                        throw "Could not query resource 'lastlistfile' " >> e;
                }

        TACO::Server::deviceQueryResource();
}

std::string MesyDAQ::Detector::Detector::incNumber()
{
	std::string tmpString = m_listFilename;
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
