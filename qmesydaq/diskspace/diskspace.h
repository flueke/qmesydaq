/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2012 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _DISKSPACE_H
#define _DISKSPACE_H

#include <QtCore>

/*!
    \class DiskSpace

    \short This class handles displaying and handling of the presets

    \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class DiskSpace
{
public:
	/**
 	 * constructor
	 *
	 * \param p path to the directory
	 */
	DiskSpace(const QString &p)
		: m_path(QDir(p))
	{
	}

	//! \returns the current path
	std::string path(void)
	{
		return m_path.path().toStdString();
	}

	/**
	 * sets the path
	 *
	 * \param p new path
	 */
	void setPath(const QString &p)
	{
		m_path = QDir(p);
	}

	//! returns the number of free bytes
	unsigned long long freeBytes(void);

	//! returns the number of free space in kB
	unsigned long long freeKB(void)
	{
		return freeBytes() / 1024;
	}

        //! returns the number of free space in MB
	unsigned long long freeMB(void)
	{
		return freeKB() / 1024;
	}

	//! returns the number of free space in GB
	unsigned long long freeGB(void)
	{
		return freeMB() / 1024;
	}

	//! returns the number of available bytes
	unsigned long long availableBytes(void);

	//! returns the number of available space in kB
	unsigned long long availableKB(void)
	{
		return availableBytes() / 1024;
	}

	//! returns the number of available space in MB
	unsigned long long availableMB(void)
	{
		return availableKB() / 1024;
	}

	//! returns the number of available space in MB
	unsigned long long availableGB(void)
	{
		return availableMB() /  1024;
	}

private:
	QDir m_path;
};

#endif //_DISKSPACE_H
