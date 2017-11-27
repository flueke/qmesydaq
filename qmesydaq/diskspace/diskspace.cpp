/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2014 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Module Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Module Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU Module Public License      *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "diskspace.h"

#if defined(_MSC_VER)

#include <windows.h>
#include <AtlBase.h>

	//! returns the number of free bytes
	unsigned long long DiskSpace::freeBytes(void)
	{
		while (!this->m_path.isRoot())
			this->m_path.cdUp();

		std::string driveName = this->m_path.dirName().toStdString();

		ULARGE_INTEGER diskspace;
		diskspace.QuadPart = 0;
		PULARGE_INTEGER freeBytesToCaller = &diskspace;
		GetDiskFreeSpaceEx(CA2W(driveName.c_str()), freeBytesToCaller, NULL, NULL);

		return (unsigned long long)freeBytesToCaller->QuadPart;
	}
	//! returns the number of available bytes
	unsigned long long DiskSpace::availableBytes(void)
	{
		return this->freeBytes();
	}

#else //_MSC_VER
# if defined(WIN32)
	unsigned long long DiskSpace::freeBytes(void)
	{
		return 0;
	}
	unsigned long long DiskSpace::availableBytes(void)
	{
		return 0;
	}
#else

#include <boost/filesystem.hpp>

	//! returns the number of free bytes
	unsigned long long DiskSpace::freeBytes(void)
	{
		boost::filesystem::path boost_path = this->m_path.path().toStdString();
		try
		{
			boost::filesystem::space_info spi = boost::filesystem::space(boost_path);
			return spi.free;
		}
#if BOOST_FILESYSTEM_VERSION == 2
		catch (const boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
#else
		catch ( ... )
#endif
		{
		}
		return 0;
	}
	//! returns the number of available bytes
	unsigned long long DiskSpace::availableBytes(void)
	{
		boost::filesystem::path boost_path = this->m_path.path().toStdString();
		try
		{
			boost::filesystem::space_info spi = boost::filesystem::space(boost_path);
			return spi.available;
		}
#if BOOST_FILESYSTEM_VERSION == 2
		catch (const boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
#else
		catch ( ... )
#endif
		{
		}
		return 0;
	}
#endif // WIN32
#endif //_MSC_VER
