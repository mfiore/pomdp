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
	${OBJECTDIR}/AssembleBracket.o \
	${OBJECTDIR}/CleanSurface.o \
	${OBJECTDIR}/GetLaundry.o \
	${OBJECTDIR}/Hmdp.o \
	${OBJECTDIR}/Mdp.o \
	${OBJECTDIR}/Mmdp.o \
	${OBJECTDIR}/NestedLoop.o \
	${OBJECTDIR}/PlaceObject.o \
	${OBJECTDIR}/Pomdp.o \
	${OBJECTDIR}/PriorityQueue.o \
	${OBJECTDIR}/ReorderTable.o \
	${OBJECTDIR}/Saphari.o \
	${OBJECTDIR}/StartWashingMachine.o \
	${OBJECTDIR}/TakeObject.o \
	${OBJECTDIR}/VariableSet.o \
	${OBJECTDIR}/Wait.o \
	${OBJECTDIR}/WashLaundry.o \
	${OBJECTDIR}/WashLaundryFull.o


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

${OBJECTDIR}/AssembleBracket.o: AssembleBracket.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AssembleBracket.o AssembleBracket.cpp

${OBJECTDIR}/CleanSurface.o: CleanSurface.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/CleanSurface.o CleanSurface.cpp

${OBJECTDIR}/GetLaundry.o: GetLaundry.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GetLaundry.o GetLaundry.cpp

${OBJECTDIR}/Hmdp.o: Hmdp.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Hmdp.o Hmdp.cpp

${OBJECTDIR}/Mdp.o: Mdp.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Mdp.o Mdp.cpp

${OBJECTDIR}/Mmdp.o: Mmdp.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Mmdp.o Mmdp.cpp

${OBJECTDIR}/NestedLoop.o: NestedLoop.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NestedLoop.o NestedLoop.cpp

${OBJECTDIR}/PlaceObject.o: PlaceObject.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PlaceObject.o PlaceObject.cpp

${OBJECTDIR}/Pomdp.o: Pomdp.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Pomdp.o Pomdp.cpp

${OBJECTDIR}/PriorityQueue.o: PriorityQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PriorityQueue.o PriorityQueue.cpp

${OBJECTDIR}/ReorderTable.o: ReorderTable.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ReorderTable.o ReorderTable.cpp

${OBJECTDIR}/Saphari.o: Saphari.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Saphari.o Saphari.cpp

${OBJECTDIR}/StartWashingMachine.o: StartWashingMachine.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/StartWashingMachine.o StartWashingMachine.cpp

${OBJECTDIR}/TakeObject.o: TakeObject.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TakeObject.o TakeObject.cpp

${OBJECTDIR}/VariableSet.o: VariableSet.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/VariableSet.o VariableSet.cpp

${OBJECTDIR}/Wait.o: Wait.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Wait.o Wait.cpp

${OBJECTDIR}/WashLaundry.o: WashLaundry.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WashLaundry.o WashLaundry.cpp

${OBJECTDIR}/WashLaundryFull.o: WashLaundryFull.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/WashLaundryFull.o WashLaundryFull.cpp

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
