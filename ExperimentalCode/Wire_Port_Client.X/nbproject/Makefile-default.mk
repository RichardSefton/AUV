#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/Wire_Port_Client.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/Wire_Port_Client.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=client.c CWire.c twi_pins.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/client.o ${OBJECTDIR}/CWire.o ${OBJECTDIR}/twi_pins.o
POSSIBLE_DEPFILES=${OBJECTDIR}/client.o.d ${OBJECTDIR}/CWire.o.d ${OBJECTDIR}/twi_pins.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/client.o ${OBJECTDIR}/CWire.o ${OBJECTDIR}/twi_pins.o

# Source Files
SOURCEFILES=client.c CWire.c twi_pins.c

# Pack Options 
PACK_COMPILER_OPTIONS=-I "${DFP_DIR}/include"
PACK_COMMON_OPTIONS=-B "${DFP_DIR}/gcc/dev/attiny1627"



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/Wire_Port_Client.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=ATtiny1627
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/client.o: client.c  .generated_files/flags/default/9b1d009e5c1dd0bdeabd4cfbb17d3ca166f2768c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/client.o.d 
	@${RM} ${OBJECTDIR}/client.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/client.o.d" -MT "${OBJECTDIR}/client.o.d" -MT ${OBJECTDIR}/client.o  -o ${OBJECTDIR}/client.o client.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/CWire.o: CWire.c  .generated_files/flags/default/f6b6d4364d68cb65ed4d8c66eeca0e1a344b25be .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/CWire.o.d 
	@${RM} ${OBJECTDIR}/CWire.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/CWire.o.d" -MT "${OBJECTDIR}/CWire.o.d" -MT ${OBJECTDIR}/CWire.o  -o ${OBJECTDIR}/CWire.o CWire.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/twi_pins.o: twi_pins.c  .generated_files/flags/default/192d835509390d2cae8f58e60afea50aeafe380b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/twi_pins.o.d 
	@${RM} ${OBJECTDIR}/twi_pins.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/twi_pins.o.d" -MT "${OBJECTDIR}/twi_pins.o.d" -MT ${OBJECTDIR}/twi_pins.o  -o ${OBJECTDIR}/twi_pins.o twi_pins.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
else
${OBJECTDIR}/client.o: client.c  .generated_files/flags/default/2bdfa5e7a2ad793d01927ace9b30f02180dac875 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/client.o.d 
	@${RM} ${OBJECTDIR}/client.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/client.o.d" -MT "${OBJECTDIR}/client.o.d" -MT ${OBJECTDIR}/client.o  -o ${OBJECTDIR}/client.o client.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/CWire.o: CWire.c  .generated_files/flags/default/af9fd937d092f1a1d8422e55812f2e0b890e6b50 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/CWire.o.d 
	@${RM} ${OBJECTDIR}/CWire.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/CWire.o.d" -MT "${OBJECTDIR}/CWire.o.d" -MT ${OBJECTDIR}/CWire.o  -o ${OBJECTDIR}/CWire.o CWire.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/twi_pins.o: twi_pins.c  .generated_files/flags/default/a1a81cfdf5d888ddc589f06c82e836cda95e1f79 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/twi_pins.o.d 
	@${RM} ${OBJECTDIR}/twi_pins.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/twi_pins.o.d" -MT "${OBJECTDIR}/twi_pins.o.d" -MT ${OBJECTDIR}/twi_pins.o  -o ${OBJECTDIR}/twi_pins.o twi_pins.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/Wire_Port_Client.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mmcu=attiny1627 ${PACK_COMMON_OPTIONS}   -gdwarf-2 -D__$(MP_PROCESSOR_OPTION)__  -Wl,-Map="${DISTDIR}\Wire_Port_Client.X.${IMAGE_TYPE}.map"    -o ${DISTDIR}/Wire_Port_Client.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1 -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,--end-group 
	
	
	
	
	
	
else
${DISTDIR}/Wire_Port_Client.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mmcu=attiny1627 ${PACK_COMMON_OPTIONS}  -D__$(MP_PROCESSOR_OPTION)__  -Wl,-Map="${DISTDIR}\Wire_Port_Client.X.${IMAGE_TYPE}.map"    -o ${DISTDIR}/Wire_Port_Client.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION) -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,--end-group 
	${MP_CC_DIR}\\avr-objcopy -O ihex "${DISTDIR}/Wire_Port_Client.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "${DISTDIR}/Wire_Port_Client.X.${IMAGE_TYPE}.hex"
	
	
	
	
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(wildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
