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
#if 1
	m_listFilename = incNumber(m_listFilename);
#else
	m_listFilename = runNumber(m_listFilename);
#endif
	updateResource<std::string>("lastlistfile", m_listFilename);
    	m_interface->setListFileName(m_listFilename.c_str());
#if 1
	m_histFilename = incNumber(m_histFilename);
#else
	m_histFilename = runNumber(m_histFilename);
#endif
	updateResource<std::string>("lasthistfile", m_histFilename);
    	m_interface->setHistogramFileName(m_histFilename.c_str());
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

	tmp.clear();
	if (!m_interface)
        	throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
        
	QList<quint64> tmpList;
// complete histogram
	{
		quint16 width,
			height;

		m_interface->readHistogramSize(width, height);
		tmpList = m_interface->readHistogram();

		tmp.push_back(width);
		tmp.push_back(height);
		tmp.push_back(1);
	}
// spectrogram
	{
// 1 1 1 value
#if 0
		tmpList = m_interface->readDiffractogram();
        	tmp.push_back(m_values.count());
		tmp.push_back(1);
		tmp.push_back(1);
#endif
	}
	for (QList<quint64>::const_iterator it = tmpList.begin(); it != tmpList.end(); ++it)
		tmp.push_back(quint32(*it));
	return tmp;
}

void MesyDAQ::Detector::Detector::deviceInit(void) throw (::TACO::Exception)
{
	// Please implement this for the startup
	try
	{
		Server::deviceUpdate("lastlistfile");
		Server::deviceUpdate("lasthistfile");
	}
	catch (const ::TACO::Exception &e)
	{
//		setDeviceState(::TACO::State::FAULT);
		throw e;
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
        if (resourceUpdateRequest("runid"))
                try
                {
                        m_listFilename = queryResource<DevULong>("runid");
			if (!m_interface)
        			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
                        m_interface->setRunID(m_runid);
			logStream->errorStream() << "RUN ID " << m_runid << log4cpp::eol;
                }
                catch (::TACO::Exception &e)
                {
                        throw "could not update 'runid' " >> e;
                }
        if (resourceUpdateRequest("lastlistfile"))
                try
                {
                        m_listFilename = queryResource<std::string>("lastlistfile");
			if (m_listFilename == "")
				m_listFilename = "tacolistfile00000.mdat";
			if (!m_interface)
        			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
                        m_interface->setListFileName(m_listFilename.c_str());
			logStream->errorStream() << "LIST FILE " << m_listFilename << log4cpp::eol;
                }
                catch (::TACO::Exception &e)
                {
                        throw "could not update 'lastlistfile' " >> e;
                }
        if (resourceUpdateRequest("lasthistfile")) 
                try
                {
                        m_histFilename = queryResource<std::string>("lasthistfile");
			if (m_histFilename == "")
				m_histFilename = "tacohistfile00000.mtxt";
			if (!m_interface)
        			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
                        m_interface->setHistogramFileName(m_histFilename.c_str());
			m_binnedFilename = m_histFilename;
			updateResource<std::string>("lastbinnedfile", m_binnedFilename);
			logStream->errorStream() << "HISTOGRAM FILE " << m_histFilename << log4cpp::eol;
                }
                catch (::TACO::Exception &e)
                {
                        throw "could not update 'lasthistfile' " >> e;
                }
        if (resourceUpdateRequest("lastbinnedfile")) 
                try
                {
                        m_binnedFilename = queryResource<std::string>("lastbinnedfile");
			if (m_binnedFilename == "")
				m_binnedFilename = "tacohistfile00000.mtxt";
			if (!m_interface)
        			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
                        m_interface->setHistogramFileName(m_binnedFilename.c_str());
			m_histFilename = m_binnedFilename;
			updateResource<std::string>("lasthistfile", m_histFilename);
			logStream->errorStream() << "BINNED FILE " << m_histFilename << log4cpp::eol;
                }
                catch (::TACO::Exception &e)
                {
                        throw "could not update 'lasthistfile' " >> e;
                }
}

void MesyDAQ::Detector::Detector::deviceQueryResource(void) throw (::TACO::Exception)
{
        if (resourceQueryRequest("runid"))
                try
                {
                        updateResource<DevULong>("runid", m_interface->getRunID());
                }
                catch (TACO::Exception &e)
                {
                        throw "Could not query resource 'runid' " >> e;
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
                        throw "Could not query resource 'lastlistfile' " >> e;
                }
        if (resourceQueryRequest("lasthistfile") || resourceQueryRequest("lastbinnedfile"))
                try
                {
			if (m_interface->getHistogramFileName().isEmpty())
				m_interface->setHistogramFileName(m_histFilename.c_str());
                        updateResource<std::string>("lasthistfile", m_interface->getHistogramFileName().toStdString());
                        updateResource<std::string>("lastbinnedfile", m_interface->getHistogramFileName().toStdString());
                }
                catch (TACO::Exception &e)
                {
                        throw "Could not query resource 'lasthistfile' " >> e;
                }

        TACO::Server::deviceQueryResource();
}

std::string MesyDAQ::Detector::Detector::incNumber(const std::string &val)
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

std::string MesyDAQ::Detector::Detector::runNumber(const std::string &val)
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
	if (m_interface)
	{
		currIndex = m_interface->getRunID();
	}
	std::string tmp = ::TACO::numberToString(currIndex, 5);
	pos = tmpString.find(baseName);
	pos += baseName.length();
	tmpString.erase(pos);
	for (int i = tmp.length(); i < 5; ++i)
        	tmpString += '0';
	tmpString += tmp;
	tmpString += ext;
	return tmpString;
}
