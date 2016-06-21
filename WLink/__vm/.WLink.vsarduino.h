/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino Due (Native USB Port), Platform=sam, Package=arduino
*/

#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
#define printf iprintf
#define F_CPU 84000000L
#define ARDUINO 10609
#define ARDUINO_SAM_DUE
#define ARDUINO_ARCH_SAM
#define __SAM3X8E__
#define USB_VID 0x2341
#define USB_PID 0x003e
#define USBCON
#define __cplusplus 201103L
#define __ARM__
#define __arm__
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __volatile__
#define _HAVE_STDC
#define __ASM
#define __INLINE
#define __builtin_va_list void

#define __attribute__(noinline)

typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}

#include "sam3xa.h"
#include <arduino.h>
#include <pins_arduino.h> 
#include <variant.h> 
#undef cli
#define cli()
#include <WLink.ino>
#include <BadgeReader.cpp>
#include <BadgeReader.h>
#include <BadgeReaderManager.cpp>
#include <BadgeReaderManager.h>
#include <CommEvent.h>
#include <Debug.cpp>
#include <Debug.h>
#include <FlatPanel.cpp>
#include <FlatPanel.h>
#include <Hardware.h>
#include <Indicator.cpp>
#include <Indicator.h>
#include <IndicatorInterface.cpp>
#include <IndicatorInterface.h>
#include <IndicatorManager.cpp>
#include <IndicatorManager.h>
#include <LD5218.cpp>
#include <LD5218.h>
#include <LcdDisplay.cpp>
#include <LcdDisplay.h>
#include <TCPServer.cpp>
#include <TCPServer.h>
#include <TCPServerManager.cpp>
#include <TCPServerManager.h>
#include <UDPServer.cpp>
#include <UDPServer.h>
#include <UDPServerManager.cpp>
#include <UDPServerManager.h>
#include <WCommand.cpp>
#include <WCommand.h>
#include <WCommandInterpreter.cpp>
#include <WCommandInterpreter.h>
#include <WCommandMedium.cpp>
#include <WCommandMedium.h>
#include <WLink.h>
#endif
