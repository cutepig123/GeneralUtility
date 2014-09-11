#include "MySerialization.h"

struct gps_position
{
    template<class Archive>
    void serialize(Archive & ar)
    {
		PUHead<Archive> x(ar);

        ar & degrees;
        ar & minutes;
        ar & seconds;
    }
    IMG_UBYTE degrees;
    IMG_UWORD minutes;
    IMG_REAL seconds;
};

struct gps_positions
{
    template<class Archive>
    void serialize(Archive & ar)
    {
		ar & m_vpos;
    }
	std::vector<gps_position> m_vpos;
};

bool operator==(gps_position const& l, gps_position const& r)
{
	return fabs(l.degrees-r.degrees)<0.0001 && fabs(l.minutes-r.minutes)<0.0001 && fabs(l.seconds-r.seconds)<0.0001;
}

bool operator==(gps_positions const& l, gps_positions const& r)
{
	IMG_UWORD uwN =l.m_vpos.size();
	if(uwN != r.m_vpos.size())
		return false;
	for( IMG_UWORD i=0; i<uwN; i++)
	{
		if( !(l.m_vpos[i]==r.m_vpos[i]) )
			return false;
	}
	return true;
}

template <class T>
void TestPU2(T const &t)
{
	// Get length
	MyOArchive o1;
	o1 & (T&)t;		// have to const cast to pass compile, how to fix??

	// Pack gps -> o2 -> v
	std::vector<char>	v(o1.lwLength);
	MyOArchive o2(&v[0]);
	o2 & (T&)t;

	// Unpack v -> gps2
	MyIArchive o3(&v[0]);
	T gps2;
	o3 & gps2;

	SYS_Assert( t==gps2 );
}

void TestPU()
{
	gps_position gps;
	gps.degrees =1;
	gps.minutes =2;
	gps.seconds =3;

	gps_positions gpss;
	gpss.m_vpos.push_back(gps);

	gps.degrees =4;
	gps.minutes =5;
	gps.seconds =6;
	gpss.m_vpos.push_back(gps);

	TestPU2(gpss);
}
