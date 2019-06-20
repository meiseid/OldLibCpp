MAKETOP	=	../Make
MKTYPE	=	StaticLibrary
MKLANG	=	C++
TARGET	=	libcpp.a
SRCS	=	\
	cData.cpp \
	cArray.cpp \
	cFile.cpp \
	cSocket.cpp \
	cApplication.cpp
CC_DBG	+=	-g
CC_OPT	+=
CC_INC	+=

include ${MAKETOP}/Makefile
