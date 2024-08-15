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
SOURCEFILES_QUOTED_IF_SPACED=Wire.cpp twi_pins.c host.c String.cpp Stream.cpp Printable.cpp Print.cpp

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/Wire.o ${OBJECTDIR}/twi_pins.o ${OBJECTDIR}/host.o ${OBJECTDIR}/String.o ${OBJECTDIR}/Stream.o ${OBJECTDIR}/Printable.o ${OBJECTDIR}/Print.o
POSSIBLE_DEPFILES=${OBJECTDIR}/Wire.o.d ${OBJECTDIR}/twi_pins.o.d ${OBJECTDIR}/host.o.d ${OBJECTDIR}/String.o.d ${OBJECTDIR}/Stream.o.d ${OBJECTDIR}/Printable.o.d ${OBJECTDIR}/Print.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/Wire.o ${OBJECTDIR}/twi_pins.o ${OBJECTDIR}/host.o ${OBJECTDIR}/String.o ${OBJECTDIR}/Stream.o ${OBJECTDIR}/Printable.o ${OBJECTDIR}/Print.o

# Source Files
SOURCEFILES=Wire.cpp twi_pins.c host.c String.cpp Stream.cpp Printable.cpp Print.cpp

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
${OBJECTDIR}/twi_pins.o: twi_pins.c  .generated_files/flags/default/ecc74a1631174ab2a1ce8fd43a487d6f70fe8979 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/twi_pins.o.d 
	@${RM} ${OBJECTDIR}/twi_pins.o 
	 ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/twi_pins.o.d" -MT "${OBJECTDIR}/twi_pins.o.d" -MT ${OBJECTDIR}/twi_pins.o  -o ${OBJECTDIR}/twi_pins.o twi_pins.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/host.o: host.c  .generated_files/flags/default/5391fc924707af739d91926eb4d0221eff6e5105 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/host.o.d 
	@${RM} ${OBJECTDIR}/host.o 
	 ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/host.o.d" -MT "${OBJECTDIR}/host.o.d" -MT ${OBJECTDIR}/host.o  -o ${OBJECTDIR}/host.o host.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
else
${OBJECTDIR}/twi_pins.o: twi_pins.c  .generated_files/flags/default/2c02f325901269b468ddc41783c6da0ad46afde6 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/twi_pins.o.d 
	@${RM} ${OBJECTDIR}/twi_pins.o 
	 ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/twi_pins.o.d" -MT "${OBJECTDIR}/twi_pins.o.d" -MT ${OBJECTDIR}/twi_pins.o  -o ${OBJECTDIR}/twi_pins.o twi_pins.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/host.o: host.c  .generated_files/flags/default/7b2b795de087e5c6297f033d6cadcbc65f02f026 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/host.o.d 
	@${RM} ${OBJECTDIR}/host.o 
	 ${MP_CPPC}  $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/host.o.d" -MT "${OBJECTDIR}/host.o.d" -MT ${OBJECTDIR}/host.o  -o ${OBJECTDIR}/host.o host.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/Wire.o: Wire.cpp  .generated_files/flags/default/f4b1e5bc2f1fee0ba5f690803dd91346e343c33d .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Wire.o.d 
	@${RM} ${OBJECTDIR}/Wire.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/Wire.o.d" -MT "${OBJECTDIR}/Wire.o.d" -MT ${OBJECTDIR}/Wire.o  -o ${OBJECTDIR}/Wire.o Wire.cpp  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/String.o: String.cpp  .generated_files/flags/default/667da2ab4058745c171b8886d7b23264f580cb48 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/String.o.d 
	@${RM} ${OBJECTDIR}/String.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/String.o.d" -MT "${OBJECTDIR}/String.o.d" -MT ${OBJECTDIR}/String.o  -o ${OBJECTDIR}/String.o String.cpp  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Stream.o: Stream.cpp  .generated_files/flags/default/cc279e4caf6c33ab4196c698c519cfb5e9f3c71 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Stream.o.d 
	@${RM} ${OBJECTDIR}/Stream.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/Stream.o.d" -MT "${OBJECTDIR}/Stream.o.d" -MT ${OBJECTDIR}/Stream.o  -o ${OBJECTDIR}/Stream.o Stream.cpp  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Printable.o: Printable.cpp  .generated_files/flags/default/293a06f5905be7969ef50989f16bf0592d2199af .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Printable.o.d 
	@${RM} ${OBJECTDIR}/Printable.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/Printable.o.d" -MT "${OBJECTDIR}/Printable.o.d" -MT ${OBJECTDIR}/Printable.o  -o ${OBJECTDIR}/Printable.o Printable.cpp  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Print.o: Print.cpp  .generated_files/flags/default/e853a6cd5046dbb965fc9da2943a5ad18d7d5984 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Print.o.d 
	@${RM} ${OBJECTDIR}/Print.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG  -gdwarf-2  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/Print.o.d" -MT "${OBJECTDIR}/Print.o.d" -MT ${OBJECTDIR}/Print.o  -o ${OBJECTDIR}/Print.o Print.cpp  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
else
${OBJECTDIR}/Wire.o: Wire.cpp  .generated_files/flags/default/1ed3177d26d92c84b0cce91bfa8a943e37da7e36 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Wire.o.d 
	@${RM} ${OBJECTDIR}/Wire.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/Wire.o.d" -MT "${OBJECTDIR}/Wire.o.d" -MT ${OBJECTDIR}/Wire.o  -o ${OBJECTDIR}/Wire.o Wire.cpp  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/String.o: String.cpp  .generated_files/flags/default/9157b92a74e2e09774fcf44675fe0e093ec4c52a .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/String.o.d 
	@${RM} ${OBJECTDIR}/String.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/String.o.d" -MT "${OBJECTDIR}/String.o.d" -MT ${OBJECTDIR}/String.o  -o ${OBJECTDIR}/String.o String.cpp  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Stream.o: Stream.cpp  .generated_files/flags/default/1853435bc5becc11eca65050f6738e4576d6ea34 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Stream.o.d 
	@${RM} ${OBJECTDIR}/Stream.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/Stream.o.d" -MT "${OBJECTDIR}/Stream.o.d" -MT ${OBJECTDIR}/Stream.o  -o ${OBJECTDIR}/Stream.o Stream.cpp  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Printable.o: Printable.cpp  .generated_files/flags/default/c5df970dd497f1594970790cc4a2823993f708bb .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Printable.o.d 
	@${RM} ${OBJECTDIR}/Printable.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/Printable.o.d" -MT "${OBJECTDIR}/Printable.o.d" -MT ${OBJECTDIR}/Printable.o  -o ${OBJECTDIR}/Printable.o Printable.cpp  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Print.o: Print.cpp  .generated_files/flags/default/f455bac7567006f706a4cf10be3a50367c4f4387 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Print.o.d 
	@${RM} ${OBJECTDIR}/Print.o 
	 ${MP_CPPC} $(MP_EXTRA_CC_PRE) -mmcu=attiny1627 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c++ -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -MD -MP -MF "${OBJECTDIR}/Print.o.d" -MT "${OBJECTDIR}/Print.o.d" -MT ${OBJECTDIR}/Print.o  -o ${OBJECTDIR}/Print.o Print.cpp  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/Wire_Port_Host.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CPPC} $(MP_EXTRA_LD_PRE) -mmcu=attiny1627 ${PACK_COMMON_OPTIONS}   -gdwarf-2 -D__$(MP_PROCESSOR_OPTION)__  -Wl,-Map="${DISTDIR}\Wire_Port_Host.X.${IMAGE_TYPE}.map"    -o ${DISTDIR}/Wire_Port_Host.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1 -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,--end-group 
	
	
	
	
	
	
else
${DISTDIR}/Wire_Port_Host.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CPPC} $(MP_EXTRA_LD_PRE) -mmcu=attiny1627 ${PACK_COMMON_OPTIONS}  -D__$(MP_PROCESSOR_OPTION)__  -Wl,-Map="${DISTDIR}\Wire_Port_Host.X.${IMAGE_TYPE}.map"    -o ${DISTDIR}/Wire_Port_Host.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION) -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,--end-group 
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
