#include "stdafx.h"
#include"PCMachine.h"
extern bool isReady;
PCMachine::PCMachine()
{
	this->moBoard.boot();
	cpu.linkMotherBoard(&moBoard);
	vga.linkScreen(&this->screen);
	keyBoard.linkKBInterface(&this->kbFace);
	moBoard.pCPU = &cpu;
	moBoard.addDevic(&this->vga);
	moBoard.addDevic(&this->disk);
	moBoard.addDevic(&this->keyBoard);
	moBoard.addDevic(&this->realClock);
	moBoard.addDevic(&this->detective);
	::isReady=true;
	

}
void PCMachine::run()
{
	
	this->cpu.deCodeBlock.deCodeInstruction();
}

void PCMachine::refreshScreen()
{
	this->screen.showAll();//��ǿ���ػ���ˢ��
}
void PCMachine::repaintScreen()
{
	this->screen.repaint();//ǿ���ػ�
}
void PCMachine::updateTime()
{
	this->realClock.updateTime();
}