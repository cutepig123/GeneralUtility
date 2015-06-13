#ifndef _MAGNETIC_DISK_H
#define _MAGNETIC_DISK_H
#include "../device.h"
#include "../common.h"
int const bytePerSector = 512;
int const sectorPerTrack = 18;
int const trackPerHead = 80;
int const numHeads = 2;
class MagneticDisk :
	public Device
{
public:
	MagneticDisk(void);
	~MagneticDisk(void);
	void init();
	vector<int> get_INT_shouldHandle();
	void HandleINT(U1 INT_num);

private:
	void diskReset();
	void readCondition();
	void readDisk();
	void writeDisk();
	void getPara();
	void getTypeOfFloppy();

	void readSector(unsigned int drive, unsigned int magneticHead, unsigned int magneticTrack,
				unsigned int sector, U1* des, unsigned int count);
	void writeSector(unsigned int drive, unsigned int magneticHead, unsigned int magneticTrack,
				unsigned int sector, U1* des, unsigned int count);
	void storeCondition_AH();
	U1 getLastCondition();
	U1* memFloppy;
	//void readSector(unsigned int magneticTrack, unsigned int sector,  );

};



#endif