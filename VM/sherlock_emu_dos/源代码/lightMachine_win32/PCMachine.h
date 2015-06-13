#ifndef _PCMACHINE_H
#define _PCMACHINE_H
#include "CPU/CentreProcessorUnit.h"
#include "motherboard.h"
#include "Device/VGACard.h"
#include "Device/Screen.h"
#include "Device/MagneticDisk.h"
#include "Device/KeyBoard.h"
#include "Device/KBInterface.h"
#include "Device/RealTimeClock.h"
#include "Device/MachineDetect.h"
class PCMachine
{
public:
	PCMachine();
	void run();
	void refreshScreen();
	void repaintScreen();
	void updateTime();

public:
	KBInterface kbFace;
private:
	CentreProcessorUnit cpu;
	MotherBoard moBoard;
	MagneticDisk disk;
	VGACard vga;
	PCKeyBoard keyBoard;
	Screen screen;
	RealTimeClock realClock;
	MachineDetective detective;
	
};


#endif
