#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/beeeye.o \
	${OBJECTDIR}/beeeyeserver.o \
	${OBJECTDIR}/beerobot.o \
	${OBJECTDIR}/httpclient.o \
	${OBJECTDIR}/httpserver.o \
	${OBJECTDIR}/imagereceiver.o \
	${OBJECTDIR}/imagesender.o \
	${OBJECTDIR}/mainclient.o \
	${OBJECTDIR}/mainserver.o \
	${OBJECTDIR}/socketcommon.o \
	${OBJECTDIR}/videotype.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread -std=c++11
CXXFLAGS=-pthread -std=c++11

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=`pkg-config --libs opencv` iniparser/libiniparser.a  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/beerobot

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/beerobot: iniparser/libiniparser.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/beerobot: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/beerobot ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/beeeye.o: beeeye.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beeeye.o beeeye.cc

${OBJECTDIR}/beeeyeserver.o: beeeyeserver.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beeeyeserver.o beeeyeserver.cc

${OBJECTDIR}/beerobot.o: beerobot.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beerobot.o beerobot.cc

${OBJECTDIR}/httpclient.o: httpclient.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/httpclient.o httpclient.cc

${OBJECTDIR}/httpserver.o: httpserver.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/httpserver.o httpserver.cc

${OBJECTDIR}/imagereceiver.o: imagereceiver.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/imagereceiver.o imagereceiver.cc

${OBJECTDIR}/imagesender.o: imagesender.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/imagesender.o imagesender.cc

${OBJECTDIR}/mainclient.o: mainclient.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mainclient.o mainclient.cc

${OBJECTDIR}/mainserver.o: mainserver.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mainserver.o mainserver.cc

${OBJECTDIR}/socketcommon.o: socketcommon.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/socketcommon.o socketcommon.cc

${OBJECTDIR}/videotype.o: videotype.cc
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/videotype.o videotype.cc

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
