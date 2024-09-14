cmake_minimum_required(VERSION 3.12...3.30)

project(DexprOS VERSION 1.0 LANGUAGES C ASM)


set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS OFF)



set(DEXPROS_KERNEL_SOURCES src/Boot/x86_64-efi/UefiBoot.c
                            src/Boot/x86_64-efi/EndBootPhase.c
                            src/Boot/x86_64-efi/InitialPageMapSetup.c
                            src/Boot/x86_64-efi/InitialPageMapSwitch.S
                            src/Kernel/BuiltInBitmapFont.c
                            src/Kernel/ErrorDisplay.c
                            src/Kernel/KernelPanic.c
                            src/Kernel/KernelAssert.c
                            src/Kernel/KeyboardInput.c
                            src/Kernel/Memory/PhysicalMemMap.c
                            src/Kernel/Memory/PhysicalMemTree.c
                            src/Kernel/x86_64/InlineAsm.c
                            src/Kernel/x86_64/CpuidRetrieve.S
                            src/Kernel/x86_64/CpuFeatures.c
                            src/Kernel/x86_64/MemoryProtectionCpuSetup.S
                            src/Kernel/x86_64/FloatingPointInit.S
                            src/Kernel/x86_64/FloatingPointSymbols.c
                            src/Kernel/x86_64/KernelPanicHalt.S
                            src/Kernel/x86_64/TaskStateSegment.c
                            src/Kernel/x86_64/GdtCreator.c
                            src/Kernel/x86_64/GdtLoader.S
                            src/Kernel/x86_64/GdtSetup.c
                            src/Kernel/x86_64/Interrupts.S
                            src/Kernel/x86_64/InterruptHandlers.c
                            src/Kernel/x86_64/IdtCreator.c
                            src/Kernel/x86_64/SyscallHandler.S
                            src/Kernel/x86_64/PageMapSwitching.S
                            src/Kernel/efi/EfiMemMapIteration.c
                            src/Kernel/efi/PhysicalMemMapEfi.c
                            src/Kernel/efi/PhysicalMemTreeGenEfi.c
                            src/Kernel/kstdlib/string.c
                            src/Drivers/Graphics/GraphicsDriver.c
                            src/Drivers/Graphics/CpuGraphicsDriver.c
                            src/Drivers/PICDriver.c
                            src/Drivers/PS2ControllerDriver.c
                            src/Drivers/Keyboard/KeyToUnicodeTable.c
                            src/Drivers/Keyboard/USKeyboardLayout.c
                            src/Drivers/Keyboard/PS2KeyboardDriver.c
                            src/Shell.c)

set(DEXPROS_KERNEL_HEADERS include/DexprOS/Boot/x86_64-efi/EndBootPhase.h
                           include/DexprOS/Boot/x86_64-efi/InitialPageMapSetup.h
                           include/DexprOS/Boot/x86_64-efi/InitialPageMapSwitch.h
                           include/DexprOS/Kernel/BuiltInBitmapFont.h
                           include/DexprOS/Kernel/DexprOSCommon.h
                           include/DexprOS/Kernel/ErrorDisplay.h
                           include/DexprOS/Kernel/KernelPanic.h
                           include/DexprOS/Kernel/KernelAssert.h
                           include/DexprOS/Kernel/KeyboardInput.h
                           include/DexprOS/Kernel/Memory/MemoryDef.h
                           include/DexprOS/Kernel/Memory/Paging.h
                           include/DexprOS/Kernel/Memory/PhysicalMemMap.h
                           include/DexprOS/Kernel/Memory/PhysicalMemTree.h
                           include/DexprOS/Kernel/x86_64/InlineAsm.h
                           include/DexprOS/Kernel/x86_64/CpuidRetrieve.h
                           include/DexprOS/Kernel/x86_64/CpuFeatures.h
                           include/DexprOS/Kernel/x86_64/MemoryProtectionCpuSetup.h
                           include/DexprOS/Kernel/x86_64/FloatingPointInit.h
                           include/DexprOS/Kernel/x86_64/GdtCreator.h
                           include/DexprOS/Kernel/x86_64/GdtLoader.h
                           include/DexprOS/Kernel/x86_64/GdtSetup.h
                           include/DexprOS/Kernel/x86_64/IdtCreator.h
                           include/DexprOS/Kernel/x86_64/InterruptHandlers.h
                           include/DexprOS/Kernel/x86_64/Interrupts.h
                           include/DexprOS/Kernel/x86_64/KernelPanicHalt.h
                           include/DexprOS/Kernel/x86_64/SyscallHandler.h
                           include/DexprOS/Kernel/x86_64/TaskStateSegment.h
                           include/DexprOS/Kernel/x86_64/PageMapSwitching.h
                           include/DexprOS/Kernel/kstdlib/string.h
                           include/DexprOS/Kernel/efi/EfiMemMapIteration.h
                           include/DexprOS/Kernel/efi/PhysicalMemMapEfi.h
                           include/DexprOS/Kernel/efi/PhysicalMemTreeGenEfi.h
                           include/DexprOS/Drivers/Graphics/GraphicsDriver.h
                           include/DexprOS/Drivers/Graphics/CpuGraphicsDriver.h
                           include/DexprOS/Drivers/PICDriver.h
                           include/DexprOS/Drivers/PS2ControllerDriver.h
                           include/DexprOS/Drivers/Keyboard/KeyToUnicodeTable.h
                           include/DexprOS/Drivers/Keyboard/PS2KeyboardDriver.h
                           include/DexprOS/Drivers/Keyboard/PS2KeyboardScanCodes.h
                           include/DexprOS/Drivers/Keyboard/USKeyboardLayout.h
                           include/DexprOS/font256.h
                           include/DexprOS/Shell.h)


add_executable(BOOTX64.EFI ${DEXPROS_KERNEL_SOURCES} ${DEXPROS_KERNEL_HEADERS})


if (CMAKE_C_COMPILER_ID MATCHES GNU) # MinGW

    target_compile_options(BOOTX64.EFI PRIVATE -ffreestanding -mno-red-zone -fshort-wchar -fno-stack-protector)
    target_link_options(BOOTX64.EFI PRIVATE -nostdlib -shared -Wl,-dll -Wl,--subsystem,10 -e efi_main)

elseif (CMAKE_C_COMPILER_ID MATCHES Clang)

    target_compile_options(BOOTX64.EFI PRIVATE -target x86_64-unknown-windows -ffreestanding -mno-red-zone -fshort-wchar -fno-stack-protector)
    target_link_options(BOOTX64.EFI PRIVATE -target x86_64-unknown-windows -fuse-ld=lld-link -nostdlib -Wl,-subsystem:efi_application -Wl,-entry:efi_main)

endif()


target_compile_options(BOOTX64.EFI PRIVATE -Wall -Wextra -Wpedantic)


target_include_directories(BOOTX64.EFI PRIVATE include
                                               third-party/gnu-efi-code/inc)


