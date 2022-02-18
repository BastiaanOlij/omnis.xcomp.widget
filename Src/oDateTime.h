/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oDateTime.h
 *  Header for our datetime object
 *
 *  Omnis has some really good date and time functions but lacks when international support is required
 *  This is a support object that allows additional functions to be accessed
 *
 *  For the time being we're using C's internal time_t functions but these are limited in their usage in other ways.
 *  Eventually I hope to replace the inner workings with a more capable implementation.
 *
 *  Bastiaan Olij
 */

#ifndef odatetimeh
#define odatetimeh

#include "omnis.xcomp.framework.h"
#include <time.h>

typedef time_t timestamp; // define our timestamp, this will make it easier to change our code if we move away from time.h

enum oDateTimePropIDs {
	oDT_localtime = 100,
	oDT_utctime = 101,
};

class oDateTime : public oBaseNVComponent {
private:
	timestamp mTimestamp;

	void setTimestamp(timestamp pTimestamp);
	void OmnisToTimeStruct(datestamptype &pOmnisDate, struct tm &pTimeStamp);
	void TimeStampToOmnis(struct tm &pTimestamp, datestamptype &pOmnisDate);

public:
	oDateTime(void);
	~oDateTime(void);

	static oDateTime *newObject(void);

	virtual void copyObject(oBaseNVComponent *pCopy); // create a copy of pCopy, this MUST be implemented in a subclass

	// properties
	static qProperties *properties(void); // return an array of property data
	virtual qbool canAssign(qlong pPropID); // return true/false if a property can be written too
	virtual qbool setProperty(qlong pPropID, EXTfldval &pNewValue, EXTCompInfo *pECI); // set the value of a property
	virtual qbool getProperty(qlong pPropID, EXTfldval &pGetValue, EXTCompInfo *pECI); // get the value of a property

	// methods
	static qMethods *methods(void); // return array of method meta data
	virtual int invokeMethod(qlong pMethodId, EXTCompInfo *pECI); // invoke a method

	// events
	static qEvents *events(void); // return an array of events meta data
};

#endif