cmake_minimum_required(VERSION 3.12...3.28)

project(DexprOS VERSION 1.0 LANGUAGES C ASM)


set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS OFF)


message(NOTICE "Notice: The only supported compiler to build DexprOS is MinGW for now.")


set(DEXPROS_KERNEL_SOURCES src/Boot/x86_64-efi/UefiBoot.c
                            src/Kernel/BuiltInBitmapFont.c
                            src/Kernel/ErrorDisplay.c
                            src/Kernel/KernelPanic.c
                            src/Kernel/KeyboardInput.c
                            src/Kernel/x86_64/InlineAsm.c
                            src/Kernel/x86_64/KernelPanicHalt.s
                            src/Kernel/x86_64/TaskStateSegment.c
                            src/Kernel/x86_64/GdtCreator.c
                            src/Kernel/x86_64/GdtLoader.s
                            src/Kernel/x86_64/GdtSetup.c
                            src/Kernel/x86_64/Interrupts.s
                            src/Kernel/x86_64/InterruptHandlers.c
                            src/Kernel/x86_64/IdtCreator.c
                            src/Kernel/x86_64/SyscallHandler.s
                            src/Drivers/Graphics/GraphicsDriver.c
                            src/Drivers/Graphics/CpuGraphicsDriver.c
                            src/Drivers/PICDriver.c
                            src/Drivers/PS2ControllerDriver.c
                            src/Drivers/Keyboard/KeyToUnicodeTable.c
                            src/Drivers/Keyboard/USKeyboardLayout.c
                            src/Drivers/Keyboard/PS2KeyboardDriver.c
                            src/Shell.c)

set(DEXPROS_KERNEL_HEADERS include/DexprOS/DexprOSCommon.h
                           include/DexprOS/Kernel/BuiltInBitmapFont.h
                           include/DexprOS/Kernel/ErrorDisplay.h
                           include/DexprOS/Kernel/KernelPanic.h
                           include/DexprOS/Kernel/KeyboardInput.h
                           include/DexprOS/Kernel/x86_64/InlineAsm.h
                           include/DexprOS/Kernel/x86_64/GdtCreator.h
                           include/DexprOS/Kernel/x86_64/GdtLoader.h
                           include/DexprOS/Kernel/x86_64/GdtSetup.h
                           include/DexprOS/Kernel/x86_64/IdtCreator.h
                           include/DexprOS/Kernel/x86_64/InterruptHandlers.h
                           include/DexprOS/Kernel/x86_64/Interrupts.h
                           include/DexprOS/Kernel/x86_64/KernelPanicHalt.h
                           include/DexprOS/Kernel/x86_64/SyscallHandler.h
                           include/DexprOS/Kernel/x86_64/TaskStateSegment.h
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

target_compile_options(BOOTX64.EFI PRIVATE -ffreestanding)
target_link_options(BOOTX64.EFI PRIVATE -nostdlib -shared -Wl,-dll -Wl,--subsystem,10 -e efi_main)

target_compile_options(BOOTX64.EFI PRIVATE -Wall -Wextra -Wpedantic)


target_include_directories(BOOTX64.EFI PRIVATE include
                                               third-party/gnu-efi-code/inc)


