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
FINAL_IMAGE=${DISTDIR}/Wire_Port_Host.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/Wire_Port_Host.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
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
SOURCEFILES_QUOTED_IF_SPACED=twi_pins.c host.c CWire.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/twi_pins.o ${OBJECTDIR}/host.o ${OBJECTDIR}/CWire.o
POSSIBLE_DEPFILES=${OBJECTDIR}/twi_pins.o.d ${OBJECTDIR}/host.o.d ${OBJECTDIR}/CWire.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/twi_pins.o ${OBJECTDIR}/host.o ${OBJECTDIR}/CWire.o

# Source Files
SOURCEFILES=twi_pins.c host.c CWire.c

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
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/Wire_Port_Host.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

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
${OBJECTDIR}/twi_pins.o: twi_pins.c  .generated_files/flags/default/723e154b658ad26e1e4e9f419e9204f6e1a7aad5 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/twi_pins.o.d 
	@${RM} ${OBJECTDIR}/twi_pins.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/twi_pins.o.d" -MT "${OBJECTDIR}/twi_pins.o.d" -MT ${OBJECTDIR}/twi_pins.o  -o ${OBJECTDIR}/twi_pins.o twi_pins.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/host.o: host.c  .generated_files/flags/default/20b6796e4036fb7a1c8b80d7bdc3e4577b693870 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/host.o.d 
	@${RM} ${OBJECTDIR}/host.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/host.o.d" -MT "${OBJECTDIR}/host.o.d" -MT ${OBJECTDIR}/host.o  -o ${OBJECTDIR}/host.o host.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/CWire.o: CWire.c  .generated_files/flags/default/2a4005e092b7e9ecedbb939dbf451a90962bfd4c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/CWire.o.d 
	@${RM} ${OBJECTDIR}/CWire.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/CWire.o.d" -MT "${OBJECTDIR}/CWire.o.d" -MT ${OBJECTDIR}/CWire.o  -o ${OBJECTDIR}/CWire.o CWire.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
else
${OBJECTDIR}/twi_pins.o: twi_pins.c  .generated_files/flags/default/627c894836213f5477d64af7e89a89bf04bcaa4c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/twi_pins.o.d 
	@${RM} ${OBJECTDIR}/twi_pins.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/twi_pins.o.d" -MT "${OBJECTDIR}/twi_pins.o.d" -MT ${OBJECTDIR}/twi_pins.o  -o ${OBJECTDIR}/twi_pins.o twi_pins.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/host.o: host.c  .generated_files/flags/default/bf673a839a18a2ae2c0124ac78a24b4ed19cf1e3 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/host.o.d 
	@${RM} ${OBJECTDIR}/host.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/host.o.d" -MT "${OBJECTDIR}/host.o.d" -MT ${OBJECTDIR}/host.o  -o ${OBJECTDIR}/host.o host.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/CWire.o: CWire.c  .generated_files/flags/default/3fc63b9a2d235659f07c2b4977a0e173293dc1d0 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/CWire.o.d 
	@${RM} ${OBJECTDIR}/CWire.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/CWire.o.d" -MT "${OBJECTDIR}/CWire.o.d" -MT ${OBJECTDIR}/CWire.o  -o ${OBJECTDIR}/CWire.o CWire.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/Wire_Port_Host.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mmcu=attiny1627 ${PACK_COMMON_OPTIONS}   -gdwarf-2 -D__$(MP_PROCESSOR_OPTION)__  -Wl,-Map="${DISTDIR}\Wire_Port_Host.X.${IMAGE_TYPE}.map"    -o ${DISTDIR}/Wire_Port_Host.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1 -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,--end-group 
	
	
	
	
	
	
else
${DISTDIR}/Wire_Port_Host.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mmcu=attiny1627 ${PACK_COMMON_OPTIONS}  -D__$(MP_PROCESSOR_OPTION)__  -Wl,-Map="${DISTDIR}\Wire_Port_Host.X.${IMAGE_TYPE}.map"    -o ${DISTDIR}/Wire_Port_Host.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION) -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,--end-group 
	${MP_CC_DIR}\\avr-objcopy -O ihex "${DISTDIR}/Wire_Port_Host.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "${DISTDIR}/Wire_Port_Host.X.${IMAGE_TYPE}.hex"
	
	
	
	
	
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
