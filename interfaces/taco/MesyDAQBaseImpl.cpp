// Interface to the QMesyDAQ software
// Copyright (C) 2009-2015 Jens Krüger

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

#include "MesyDAQBase.h"

#include <Admin.h>
#include <iostream>

// TACODEVEL CODEGEN INCLUDES END

#include <TACOStringConverters.h>

#include "QMesydaqDetectorInterface.h"

#include "libgen.h"

void MesyDAQ::Base::deviceInit(void) throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::Base::deviceInit()" << ENDLOG;
	// Please implement this for the startup
	try
	{
		Server::deviceUpdate("lastlistfile");
		Server::deviceUpdate("lasthistfile");
		Server::deviceUpdate("writelistmode");
		Server::deviceUpdate("writehistogram");
//		Server::deviceUpdate("histogram");
//		Server::deviceUpdate("lastbinnedfile");
//		Server::deviceUpdate("runid");
//		Server::deviceUpdate("histogram");
//		Server::deviceUpdate("configfile");
		if (!m_interface)
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
//		m_interface->init();
	}
	catch (const ::TACO::Exception &e)
	{
		throw;
	}
}

void MesyDAQ::Base::deviceUpdate(void) throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::Base::deviceUpdate()" << ENDLOG;

	if (resourceUpdateRequest("runid"))
		try
		{
			m_listFilename = queryResource<DevULong>("runid");
			if (!m_interface)
				throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
			m_interface->setRunID(m_runid);
			INFO_STREAM << "RUN ID " << m_runid << ENDLOG;
		}
		catch (::TACO::Exception &e)
		{
			throw_exception(e, "could not update 'runid' ");
		}
	if (resourceUpdateRequest("configfile"))
		try
		{
			std::string tmp = queryResource<std::string>("configfile");
			if (tmp.empty())
				throw ::TACO::Exception(::TACO::Error::INVALID_VALUE, "Not empty config file name allowed");
			if (!m_interface)
				throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
			m_interface->loadConfigurationFile(tmp.c_str());
			INFO_STREAM << "CONFIG FILE " << tmp << ENDLOG;
		}
		catch (::TACO::Exception &e)
		{
			throw_exception(e, "could not update 'configfile' ");
		}
	if (resourceUpdateRequest("lastlistfile"))
		try
		{
			m_listFilename = queryResource<std::string>("lastlistfile");
			ERROR_STREAM << deviceName() << " " << m_listFilename << ENDLOG;
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
	if (resourceUpdateRequest("lasthistfile"))
		try
		{
			m_histFilename = queryResource<std::string>("lasthistfile");
			ERROR_STREAM << deviceName() << " " << m_histFilename << ENDLOG;
			if (m_histFilename == "")
				m_histFilename = "tacohistfile00000.mtxt";
			if (!m_interface)
				throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "Control interface not initialized");
			m_interface->setHistogramFileName(m_histFilename.c_str());
			m_binnedFilename = m_histFilename;
			updateResource<std::string>("lastbinnedfile", m_binnedFilename);
			INFO_STREAM << "HISTOGRAM FILE " << m_histFilename << ENDLOG;
		}
		catch (::TACO::Exception &e)
		{
			throw_exception(e, "could not update 'lasthistfile' ");
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
			INFO_STREAM << "BINNED FILE " << m_histFilename << ENDLOG;
		}
		catch (::TACO::Exception &e)
		{
			throw_exception(e, "could not update 'lasthistfile' ");
		}
	if (resourceUpdateRequest("histogram"))
		try
		{
			std::string tmp = queryResource<std::string>("histogram");
			if (tmp == "mapped")
				m_histo = 2;
			else if (tmp == "amplitude")
				m_histo = 1;
			else
				m_histo = 0;
		}
		catch (::TACO::Exception &e)
		{
			throw_exception(e, "could not update 'histogram' ");
		}
	if (resourceUpdateRequest("writelistmode"))
		try
		{
			std::string tmp = queryResource<std::string>("writelistmode");
			if (!tmp.empty())
				m_writeListmode = queryResource<bool>("writelistmode");
		}
		catch (::TACO::Exception &e)
		{
			throw_exception(e, "could not update 'writelistmode' ");
		}
	if (resourceUpdateRequest("writehistogram"))
		try
		{
			std::string tmp = queryResource<std::string>("writehistogram");
			if (!tmp.empty())
				m_writeHistogram = queryResource<bool>("writehistogram");
		}
		catch (::TACO::Exception &e)
		{
			throw_exception(e, "could not update 'writehistogram' ");
		}

	::TACO::Server::deviceUpdate();
}

void MesyDAQ::Base::deviceQueryResource(void) throw (::TACO::Exception)
{
	INFO_STREAM << "MesyDAQ::Base::deviceQueryResource()" << ENDLOG;

	if (resourceQueryRequest("histogram"))
		try
		{
			switch (m_histo)
			{
				default:
					updateResource<std::string>("histogram", "raw");
					break;
				case 1:
					updateResource<std::string>("histogram", "amplitude");
					break;
				case 2:
					updateResource<std::string>("histogram", "mapped");
					break;
			}
		}
		catch (::TACO::Exception &e)
		{
			throw_exception(e, "could not query resource 'histogram' ");
		}

	if (!m_interface)
	{
		makeResourceQuerySuccessful();
		return;
	}

	if (resourceQueryRequest("runid"))
		try
		{
			updateResource<DevULong>("runid", m_interface->getRunID());
		}
		catch (TACO::Exception &e)
		{
			throw_exception(e, "Could not query resource 'runid' ");
		}
	if (resourceQueryRequest("configfile"))
		try
		{
			updateResource<std::string>("configfile", m_interface->getConfigurationFileName().toStdString());
		}
		catch (TACO::Exception &e)
		{
			throw_exception(e, "Could not query resource 'configfile' ");
		}
	if (resourceQueryRequest("lastlistfile"))
		try
		{
#if 0
			if (m_interface->getListFileName().isEmpty())
				m_interface->setListFileName(m_listFilename.c_str());
			updateResource<std::string>("lastlistfile", m_interface->getListFileName().toStdString());
#endif
			updateResource<std::string>("lastlistfile", m_listFilename);
		}
		catch (TACO::Exception &e)
		{
			throw_exception(e, "Could not query resource 'lastlistfile' ");
		}
	if (resourceQueryRequest("lasthistfile") || resourceQueryRequest("lastbinnedfile"))
		try
		{
#if 0
			if (m_interface->getHistogramFileName().isEmpty())
				m_interface->setHistogramFileName(m_histFilename.c_str());
			updateResource<std::string>("lasthistfile", m_interface->getHistogramFileName().toStdString());
			updateResource<std::string>("lastbinnedfile", m_interface->getHistogramFileName().toStdString());
#endif
			updateResource<std::string>("lasthistfile", m_histFilename);
			updateResource<std::string>("lastbinnedfile", m_histFilename);
		}
		catch (TACO::Exception &e)
		{
			throw_exception(e, "Could not query resource 'lasthistfile' ");
		}
	if (resourceQueryRequest("writelistmode"))
		try
		{
			updateResource<bool>("writelistmode", m_writeListmode);
		}
		catch (TACO::Exception &e)
		{
			throw_exception(e, "Could not query resource 'writelistmode' ");
		}
	if (resourceQueryRequest("writehistogram"))
		try
		{
			updateResource<bool>("writehistogram", m_writeHistogram);
		}
		catch (TACO::Exception &e)
		{
			throw_exception(e, "Could not query resource 'writehistogram' ");
		}

	TACO::Server::deviceQueryResource();
}

std::string MesyDAQ::Base::incNumber(const std::string &val)
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

std::string MesyDAQ::Base::runNumber(const std::string &val)
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
