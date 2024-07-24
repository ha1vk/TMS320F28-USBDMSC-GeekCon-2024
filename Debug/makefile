################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /Applications/ti/ccs1230/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./geekcon_main.obj" \
"./usb_hal.obj" \
"./device/F2837xD_CodeStartBranch.obj" \
"./device/F2837xD_GlobalVariableDefs.obj" \
"./device/F2837xD_Gpio.obj" \
"./device/F2837xD_SysCtrl.obj" \
"./device/device.obj" \
"./device/driverlib/dcsm.obj" \
"./device/driverlib/flash.obj" \
"./device/driverlib/gpio.obj" \
"./device/driverlib/interrupt.obj" \
"./device/driverlib/sysctl.obj" \
"./device/driverlib/usb.obj" \
"./device/driverlib/xbar.obj" \
"./flash_api/Fapi_UserDefinedFunctions.obj" \
"./flash_disk/flashdisk.obj" \
"./usbcfg/board.obj" \
"./usbcfg/usb_structs.obj" \
"./usblib/device/usbdcdesc.obj" \
"./usblib/device/usbdconfig.obj" \
"./usblib/device/usbdenum.obj" \
"./usblib/device/usbdhandler.obj" \
"./usblib/device/usbdmsc.obj" \
"./usblib/device/usbdmscglue.obj" \
"./usblib/usbbuffer.obj" \
"./usblib/usbdesc.obj" \
"./usblib/usbmode.obj" \
"./usblib/usbringbuf.obj" \
"./usblib/usbtick.obj" \
"../2837xD_FLASH_lnk_cpu1_USB.cmd" \
"../flash_api/F021_API_F2837xD_FPU32.lib" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include device/subdir_vars.mk
-include device/driverlib/subdir_vars.mk
-include flash_api/subdir_vars.mk
-include flash_disk/subdir_vars.mk
-include usbcfg/subdir_vars.mk
-include usblib/device/subdir_vars.mk
-include usblib/subdir_vars.mk
-include subdir_rules.mk
-include device/subdir_rules.mk
-include device/driverlib/subdir_rules.mk
-include flash_api/subdir_rules.mk
-include flash_disk/subdir_rules.mk
-include usbcfg/subdir_rules.mk
-include usblib/device/subdir_rules.mk
-include usblib/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
geekcon.out 

EXE_OUTPUTS__QUOTED += \
"geekcon.out" 


# All Target
all: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "geekcon.out"

# Tool invocations
geekcon.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C2000 Linker'
	"/Applications/ti/ccs1230/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi -z -m"geekcon.map" --stack_size=0x200 --warn_sections -i"/Applications/ti/ccs1230/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/lib" -i"/Applications/ti/ccs1230/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="geekcon_linkInfo.xml" --rom_model -o "geekcon.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)
	-$(RM) "geekcon_main.obj" "usb_hal.obj" "device/F2837xD_CodeStartBranch.obj" "device/F2837xD_GlobalVariableDefs.obj" "device/F2837xD_Gpio.obj" "device/F2837xD_SysCtrl.obj" "device/device.obj" "device/driverlib/dcsm.obj" "device/driverlib/flash.obj" "device/driverlib/gpio.obj" "device/driverlib/interrupt.obj" "device/driverlib/sysctl.obj" "device/driverlib/usb.obj" "device/driverlib/xbar.obj" "flash_api/Fapi_UserDefinedFunctions.obj" "flash_disk/flashdisk.obj" "usbcfg/board.obj" "usbcfg/usb_structs.obj" "usblib/device/usbdcdesc.obj" "usblib/device/usbdconfig.obj" "usblib/device/usbdenum.obj" "usblib/device/usbdhandler.obj" "usblib/device/usbdmsc.obj" "usblib/device/usbdmscglue.obj" "usblib/usbbuffer.obj" "usblib/usbdesc.obj" "usblib/usbmode.obj" "usblib/usbringbuf.obj" "usblib/usbtick.obj" 
	-$(RM) "geekcon_main.d" "usb_hal.d" "device/F2837xD_GlobalVariableDefs.d" "device/F2837xD_Gpio.d" "device/F2837xD_SysCtrl.d" "device/device.d" "device/driverlib/dcsm.d" "device/driverlib/flash.d" "device/driverlib/gpio.d" "device/driverlib/interrupt.d" "device/driverlib/sysctl.d" "device/driverlib/usb.d" "device/driverlib/xbar.d" "flash_api/Fapi_UserDefinedFunctions.d" "flash_disk/flashdisk.d" "usbcfg/board.d" "usbcfg/usb_structs.d" "usblib/device/usbdcdesc.d" "usblib/device/usbdconfig.d" "usblib/device/usbdenum.d" "usblib/device/usbdhandler.d" "usblib/device/usbdmsc.d" "usblib/device/usbdmscglue.d" "usblib/usbbuffer.d" "usblib/usbdesc.d" "usblib/usbmode.d" "usblib/usbringbuf.d" "usblib/usbtick.d" 
	-$(RM) "device/F2837xD_CodeStartBranch.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

