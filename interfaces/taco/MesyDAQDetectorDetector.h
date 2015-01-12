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

#ifndef MESY_D_A_Q_DETECTOR_DETECTOR_H
#define MESY_D_A_Q_DETECTOR_DETECTOR_H

// TACODEVEL CODEGEN INCLUDES BEGIN
// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif // HAVE_CONFIG_H

#include <DetectorCommon.h>
#include <TACOServer.h>

// TACODEVEL CODEGEN INCLUDES END

#include "MesyDAQBase.h"

namespace MesyDAQ {
	namespace Detector {
		class Detector;
	}
}

	// TACODEVEL CODEGEN BASE CLASSES BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

class MesyDAQ::Detector::Detector
	: public Base

	// TACODEVEL CODEGEN BASE CLASSES END

	// TACODEVEL CODEGEN METHOD DECLARATIONS BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.
{
public:
	Detector(const std::string& name, DevLong& error) throw (::TACO::Exception);

	~Detector() throw ();

protected:

	virtual DevVoid start() throw (::TACO::Exception);

	virtual DevVoid stop() throw (::TACO::Exception);

	virtual DevVoid setPreselection(const DevDouble input) throw (::TACO::Exception);

	virtual DevVoid resume() throw (::TACO::Exception);

	virtual DevVoid clear() throw (::TACO::Exception);

	virtual DevDouble preselection() throw (::TACO::Exception);

	virtual const std::vector<DevULong> &read() throw (::TACO::Exception);

	virtual const char *GetClassName() {return "MesyDAQ::Detector::Detector";}

	virtual const char *GetDevType() {return "Detector::Detector";}

	// TACODEVEL CODEGEN METHOD DECLARATIONS END

	DevShort deviceState(void) throw (::TACO::Exception);

	void deviceUpdate(void) throw (::TACO::Exception);

	void deviceQueryResource(void) throw (::TACO::Exception);

private:
	// TACODEVEL CODEGEN TACO METHOD DECLARATIONS BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

	static void tacoStart(::TACO::Server* server, DevArgument argin, DevArgument argout) throw (::TACO::Exception);

	static void tacoStop(::TACO::Server* server, DevArgument argin, DevArgument argout) throw (::TACO::Exception);

	static void tacoSetPreselection(::TACO::Server* server, DevArgument argin, DevArgument argout) throw (::TACO::Exception);

	static void tacoResume(::TACO::Server* server, DevArgument argin, DevArgument argout) throw (::TACO::Exception);

	static void tacoClear(::TACO::Server* server, DevArgument argin, DevArgument argout) throw (::TACO::Exception);

	static void tacoPreselection(::TACO::Server* server, DevArgument argin, DevArgument argout) throw (::TACO::Exception);

	static void tacoRead(::TACO::Server* server, DevArgument argin, DevArgument argout) throw (::TACO::Exception);

	void deviceInit(void) throw (::TACO::Exception);

	// TACODEVEL CODEGEN TACO METHOD DECLARATIONS END
};

#endif // MESY_D_A_Q_DETECTOR_DETECTOR_H
