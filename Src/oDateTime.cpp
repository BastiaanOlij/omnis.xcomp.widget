/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oDateTime.cpp
 *  Implementation of our datetime object
 *
 *  Bastiaan Olij
 */


#include "oDateTime.h"

// Constructor to initialize object
oDateTime::oDateTime(void) {
//	addToTraceLog("Constructing oDateTime %li",(uint)this);
	
	// default to our current time
	time(&mTimestamp);
};

// Destructor to clean up
oDateTime::~oDateTime(void) {
//	addToTraceLog("Destructing oDateTime %li",(uint)this);	
};


// instantiate a new object
oDateTime * oDateTime::newObject(void) {
	oDateTime *lvNewObject = new oDateTime();
	
	return lvNewObject;
};

// create a copy of pCopy, this MUST be implemented in a subclass
void oDateTime::copyObject(oBaseNVComponent *pCopy) {
	// no members to copy just yet...
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// support functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void	oDateTime::setTimestamp(timestamp pTimestamp) {
	// range check our timestamp, at this point we do not support negative values and mktime/timegm will return -1 if an incorrect date is given.
	if (pTimestamp<0) {
		mTimestamp = 0;
	} else {
		mTimestamp = pTimestamp;
	};
};

void	oDateTime::OmnisToTimeStruct(datestamptype &pOmnisDate, struct tm &pTimeStamp) {
	if (!pOmnisDate.mDateOk) {
		pTimeStamp.tm_year	= 0;
		pTimeStamp.tm_mon	= 0;
		pTimeStamp.tm_mday	= 1;
		pTimeStamp.tm_yday	= 0;
		pTimeStamp.tm_wday	= 0;
		pTimeStamp.tm_hour	= 0;
		pTimeStamp.tm_min	= 0;
		pTimeStamp.tm_sec	= 0;
		pTimeStamp.tm_isdst	= -1;
	} else {
		pTimeStamp.tm_year	= pOmnisDate.mYear - 1900;
		pTimeStamp.tm_mon	= pOmnisDate.mMonth - 1;
		pTimeStamp.tm_mday	= pOmnisDate.mDay;
		pTimeStamp.tm_yday	= 0;
		pTimeStamp.tm_wday	= 0;
		if (!pOmnisDate.mTimeOk) {
			pTimeStamp.tm_hour	= 0;
			pTimeStamp.tm_min	= 0;
			pTimeStamp.tm_sec	= 0;			
		} else {
			pTimeStamp.tm_hour	= pOmnisDate.mHour;
			pTimeStamp.tm_min	= pOmnisDate.mMin;
			if (!pOmnisDate.mSecOk) {
				pTimeStamp.tm_sec	= 0;
			} else {				
				pTimeStamp.tm_sec	= pOmnisDate.mSec;
			}
		};
		pTimeStamp.tm_isdst	= -1; // we don't know, we may need to implement this properly...		
	};
};

void	oDateTime::TimeStampToOmnis(struct tm &pTimestamp, datestamptype &pOmnisDate) {
	pOmnisDate.mYear		= pTimestamp.tm_year + 1900;
	pOmnisDate.mMonth		= pTimestamp.tm_mon + 1;
	pOmnisDate.mDay			= pTimestamp.tm_mday;
	pOmnisDate.mDateOk		= qtrue;
	pOmnisDate.mHour		= pTimestamp.tm_hour;
	pOmnisDate.mMin			= pTimestamp.tm_min;
	pOmnisDate.mSec			= pTimestamp.tm_sec;
	pOmnisDate.mTimeOk		= qtrue;
	pOmnisDate.mSecOk		= qtrue;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ECOproperty oDateTimeProperties[] = { 
	//	ID						ResID	Type			Flags					ExFlags	EnumStart	EnumEnd
	oDT_localtime,				4000,	fftDate,		EXTD_FLAG_PROPDATA,		0,		0,			0,
	oDT_utctime,				4001,	fftDate,		EXTD_FLAG_PROPDATA,		0,		0,			0,
};	

qProperties * oDateTime::properties(void) {
	qProperties *	lvProperties = oBaseNVComponent::properties();
	
	// Add the property definition for our object here...
	lvProperties->addElements(oDateTimeProperties, sizeof(oDateTimeProperties) / sizeof(ECOproperty));
	
	return lvProperties;
};

qbool	oDateTime::canAssign(qlong pPropID) {
	switch (pPropID) {
		case oDT_localtime:
			return qtrue;
			break;
		default:
			return oBaseNVComponent::canAssign(pPropID);
			break;
	}
};

// set the value of a property
qbool oDateTime::setProperty(qlong pPropID,EXTfldval &pNewValue,EXTCompInfo* pECI) {
	// most anum properties are managed by Omnis but some we need to do ourselves, no idea why...
	
	switch (pPropID) {
		case oDT_localtime: {
			datestamptype omnisstamp;
			qbool success;
			pNewValue.getDate(omnisstamp, dpFdtimeC, &success);
			
			if (success) {
				struct tm timestruct;
				
				OmnisToTimeStruct(omnisstamp, timestruct);
				
				setTimestamp(mktime(&timestruct));
			};
			return qtrue;
		}; break;
		case oDT_utctime: {
			datestamptype omnisstamp;
			qbool success;
			pNewValue.getDate(omnisstamp, dpFdtimeC, &success);
			
			if (success) {
				struct tm timestruct;
				
				OmnisToTimeStruct(omnisstamp, timestruct);
				
#ifdef iswin32
				setTimestamp(_mkgmtime(&timestruct));
#else
				setTimestamp(timegm(&timestruct));
#endif
			};
			return qtrue;
		}; break;
		default:
			return oBaseNVComponent::setProperty(pPropID, pNewValue, pECI);
			break;
	};
};
	
// get the value of a property
void oDateTime::getProperty(qlong pPropID,EXTfldval &pGetValue,EXTCompInfo* pECI) {
	// most anum properties are managed by Omnis but some we need to do ourselves...
		
	switch (pPropID) {
		case oDT_localtime: {
			struct tm *		timestruct = localtime(&mTimestamp);
			datestamptype	omnisstamp;
			
			TimeStampToOmnis(*timestruct,omnisstamp);
			
			pGetValue.setDate(omnisstamp, dpFdtimeC);
		}; break;
		case oDT_utctime: {
			struct tm *		timestruct = gmtime(&mTimestamp);
			datestamptype	omnisstamp;
			
			TimeStampToOmnis(*timestruct,omnisstamp);
			
			pGetValue.setDate(omnisstamp, dpFdtimeC);
		}; break;
		default:
			oBaseNVComponent::getProperty(pPropID, pGetValue, pECI);
				
			break;
	};
};	
				
				
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// parameters for our callback function
ECOparam addSecondsParams[] = {
	//	Resource	Type			Flags				ExFlags
	7000,			fftInteger,		0,					0,		// seconds
};


// This is our array of methods we support
ECOmethodEvent oDateTimeMethods[] = {
	//	ID				Resource	Return type		Paramcount		Params					Flags		ExFlags
	1,					8000,		fftNone,		1,				addSecondsParams,		0,			0,			// $addSeconds	
};

// return an array of method meta data
qMethods * oDateTime::methods(void) {
	qMethods * lvMethods = oBaseNVComponent::methods();
		
	lvMethods->addElements(oDateTimeMethods, sizeof(oDateTimeMethods) / sizeof(ECOmethodEvent));		

	return lvMethods;	
};

// invoke a method
int	oDateTime::invokeMethod(qlong pMethodId, EXTCompInfo* pECI) {
	switch (pMethodId) {
		case 1: { // add seconds
			EXTParamInfo*		param1 = ECOfindParamNum( pECI, 1 );
			EXTfldval			tmpData((qfldval) param1->mData);
			int					tmpSeconds = tmpData.getLong();
			
			setTimestamp(mTimestamp + tmpSeconds);
			
			return 1L;							
		}; break;
		default: {
			return oBaseNVComponent::invokeMethod(pMethodId, pECI);
		}; break;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// events
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// return an array of events meta data
qEvents *	oDateTime::events(void) {
	qEvents *	lvEvents = oBaseNVComponent::events();
	
	// nothing to add yet, I'm not even sure these work for non-visual components
	
	return lvEvents;
};
