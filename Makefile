MAKETOP	=	../Make
MKTYPE	=	StaticLibrary
MKLANG	=	C++
TARGET	=	libcpp.a
SRCS	=	\
	LFile.cpp \
	LSocket.cpp \
	LTool.cpp \
	LApplication.cpp
CC_DBG	+=	-g
CC_OPT	+=
CC_INC	+=

include ${MAKETOP}/Makefile
