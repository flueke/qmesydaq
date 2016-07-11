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

#ifndef MESY_D_A_Q_BASE_H
#define MESY_D_A_Q_BASE_H

// TACODEVEL CODEGEN INCLUDES BEGIN
// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif // HAVE_CONFIG_H

#include <DetectorCommon.h>
#include <TACOServer.h>

// TACODEVEL CODEGEN INCLUDES END

#if SIZEOF_UNSIGNED_LONG == 4
typedef unsigned long DevULong;
#else
typedef unsigned int DevULong;
#endif

class QMesyDAQDetectorInterface;

namespace MesyDAQ {
	class Base;
}

	// TACODEVEL CODEGEN BASE CLASSES BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

class MesyDAQ::Base
	: public ::TACO::Server

	// TACODEVEL CODEGEN BASE CLASSES END

	/* , MyFirstBaseClass, MySecondBaseClass, ... */
	// TACODEVEL CODEGEN METHOD DECLARATIONS BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.
{
public:
	Base(const std::string& name, DevLong& error) throw (::TACO::Exception);

	~Base() throw ();

protected:
	virtual const char *GetClassName() {return "MesyDAQ::Base";}

	virtual const char *GetDevType() {return "Base";}

	// TACODEVEL CODEGEN METHOD DECLARATIONS END

	virtual void deviceUpdate(void) throw (::TACO::Exception);

	virtual void deviceQueryResource(void) throw (::TACO::Exception);

	virtual void deviceInit(void) throw (::TACO::Exception);

	virtual void deviceReset(void) throw (::TACO::Exception);

private:
	// TACODEVEL CODEGEN TACO METHOD DECLARATIONS BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

	// TACODEVEL CODEGEN TACO METHOD DECLARATIONS END

protected:
	std::string incNumber(const std::string &);

	std::string runNumber(const std::string &val);

protected:
	QMesyDAQDetectorInterface	*m_interface;

	std::string			m_listFilename;

	std::string			m_histFilename;

	std::string			m_binnedFilename;

	DevULong			m_runid;

	DevULong			m_histo;

	bool				m_writeListmode;

	bool				m_writeHistogram;
};

#endif // MESY_D_A_Q_BASE_H
