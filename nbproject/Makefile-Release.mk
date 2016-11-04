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
	${OBJECTDIR}/ConcreteMdp.o \
	${OBJECTDIR}/Hmdp.o \
	${OBJECTDIR}/Mdp.o \
	${OBJECTDIR}/Mmdp.o \
	${OBJECTDIR}/MmdpManager.o \
	${OBJECTDIR}/NestedLoop.o \
	${OBJECTDIR}/PriorityQueue.o \
	${OBJECTDIR}/VariableSet.o \
	${OBJECTDIR}/Wait.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libpomdp.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libpomdp.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libpomdp.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libpomdp.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libpomdp.a

${OBJECTDIR}/ConcreteMdp.o: ConcreteMdp.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../devel/src/eigen -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ConcreteMdp.o ConcreteMdp.cpp

${OBJECTDIR}/Hmdp.o: Hmdp.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../devel/src/eigen -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Hmdp.o Hmdp.cpp

${OBJECTDIR}/Mdp.o: Mdp.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../devel/src/eigen -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Mdp.o Mdp.cpp

${OBJECTDIR}/Mmdp.o: Mmdp.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../devel/src/eigen -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Mmdp.o Mmdp.cpp

${OBJECTDIR}/MmdpManager.o: MmdpManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../devel/src/eigen -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MmdpManager.o MmdpManager.cpp

${OBJECTDIR}/NestedLoop.o: NestedLoop.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../devel/src/eigen -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NestedLoop.o NestedLoop.cpp

${OBJECTDIR}/PriorityQueue.o: PriorityQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../devel/src/eigen -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PriorityQueue.o PriorityQueue.cpp

${OBJECTDIR}/VariableSet.o: VariableSet.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../devel/src/eigen -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/VariableSet.o VariableSet.cpp

${OBJECTDIR}/Wait.o: Wait.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../devel/src/eigen -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Wait.o Wait.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libpomdp.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
