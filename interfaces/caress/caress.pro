include( ../../qmesydaqconfig.pri )

TEMPLATE 	= lib
TARGET 		= caressInterface
DEPENDPATH 	+= . .. ../.. ../../qmesydaq ../../lib
INCLUDEPATH 	+= . .. ../.. ../../qmesydaq ../../lib

LIBS		+= -L../../lib $${CARESSLIBS}

contains(CONFIG, bit64) {
target.path	= /usr/local/lib64
}
else {
target.path	= /usr/local/lib
}

DISTFILES       += corbadevice.idl
QMAKE_DISTCLEAN += corbadevice.h corbadeviceSK.cpp

# Input
HEADERS 	+= CARESSLoop.h corbadevice.h
SOURCES 	+= CARESSLoop.cpp corbadeviceSK.cpp

# CORBA interface to C++ compiler
idl1intermediate.target = corbadeviceSK.cpp
idl1intermediate.depends = idl2intermediate
idl2intermediate.target = corbadevice.h
idl2intermediate.commands = omniidl -bcxx -Wbh=.h -Wbs=SK.cpp -Wbd=DynSK.cpp corbadevice.idl

# This variable contains the extra targets that have been added
QMAKE_EXTRA_TARGETS += idl1intermediate idl2intermediate
