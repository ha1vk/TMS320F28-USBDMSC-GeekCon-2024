################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
syscfg/%.obj: ../syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"/Applications/ti/ccs1230/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="/Users/mac/workspace_v12/geekcon" --include_path="/Users/mac/workspace_v12/geekcon/usblib" --include_path="/Users/mac/workspace_v12/geekcon/device/driverlib" --include_path="/Users/mac/workspace_v12/geekcon/device" --include_path="/Users/mac/workspace_v12/geekcon/syscfg" --include_path="/Users/mac/workspace_v12/geekcon/usblib/host" --include_path="/Users/mac/workspace_v12/geekcon/flash_api/include" --include_path="/Users/mac/workspace_v12/geekcon/flash_dev" --include_path="/Users/mac/workspace_v12/geekcon/device_support" --include_path="/Applications/ti/ccs1230/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


