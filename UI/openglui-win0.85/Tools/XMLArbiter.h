#ifndef __XML_ARBITER_H__
#define __XML_ARBITER_H__

#include "tinyxml.h"
#include "../Math/MathUtils.h"

class XMLArbiter
{
  public:
    static Tuple4f &fillComponents4f(const TiXmlElement *element, Tuple4f &tuple);
    static Tuple4i &fillComponents4i(const TiXmlElement *element, Tuple4i &tuple);
    static Tuple4d &fillComponents4d(const TiXmlElement *element, Tuple4d &tuple);

    static Tuple3f &fillComponents3f(const TiXmlElement *element, Tuple3f &tuple);
    static Tuple3i &fillComponents3i(const TiXmlElement *element, Tuple3i &tuple);
    static Tuple3d &fillComponents3d(const TiXmlElement *element, Tuple3d &tuple);

    static Tuple2f &fillComponents2f(const TiXmlElement *element, Tuple2f &tuple);
    static Tuple2i &fillComponents2i(const TiXmlElement *element, Tuple2i &tuple);
    static Tuple2d &fillComponents2d(const TiXmlElement *element, Tuple2d &tuple);

    static const TiXmlElement *getChildElementByName(const TiXmlElement *element,
                                                     const char         *name);

    static double  fillComponents1d(const TiXmlElement *element,
                                    const char         *attribute, 
                                    double              initialValue);

    static float   fillComponents1f(const TiXmlElement *element,
                                    const char         *attribute, 
                                    float               initialValue);

    static int     fillComponents1i(const TiXmlElement *element,
                                    const char         *attribute, 
                                    int                 initialValue);

    static bool    analyzeBooleanAttr(const TiXmlElement *element,
                                      const char         *attribute, 
                                      bool                initialValue);

    static bool    inspectElementInfo(const TiXmlElement *element,
                                      const char         *desiredType);
};

#endif
