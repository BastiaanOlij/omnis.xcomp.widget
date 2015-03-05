/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oTextEdit.h
 *  Header for our text edit control
 *
 *  My first attempt at creating my own text edit control
 *
 *  Bastiaan Olij
 */

#include "oTextEdit.h"

// Constructor to initialize object
oTextEdit::oTextEdit(void) {
	mLeftIconID		= 0;
	mRightIconID	= 0;
};

// Destructor to clean up
oTextEdit::~oTextEdit(void) {

};

// instantiate a new object
oTextEdit * oTextEdit::newObject(void) {
	oTextEdit *lvNewTextEdit = new oTextEdit();
	
	return lvNewTextEdit;
};

// Do our drawing in here
void oTextEdit::doPaint(EXTCompInfo* pECI) {
	// call base class to draw background
	oBaseVisComponent::doPaint(pECI);
    
    qrect drawRect = mClientRect;
	
	if (mLeftIconID!=0) {
		drawRect.left = mCanvas->drawIcon(mLeftIconID, drawRect, jstLeft, jstCenter);
	};

	if (mRightIconID!=0) {
		drawRect.right = drawRect.right - mCanvas->drawIcon(mRightIconID, drawRect, jstRight, jstCenter);
	};
    
    // need to add justification..
    mCanvas->drawText(mContents.cString(), drawRect, mTextColor);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ECOproperty oTextEditProperties[] = { 
	//	ID						ResID	Type			Flags					ExFlags	EnumStart	EnumEnd
	anumFieldname,				0,		fftCharacter,	EXTD_FLAG_PRIMEDATA
														+EXTD_FLAG_FAR_SRCH
														+EXTD_FLAG_PROPGENERAL,	0,		0,			0,		// $dataname
	oTE_lefticon,				4400,	fftInteger,		EXTD_FLAG_PROPAPP
                                                        +EXTD_FLAG_PWINDICON,	0,		0,			0,		// $lefticonid
	oTE_righticon,				4401,	fftInteger,		EXTD_FLAG_PROPAPP
                                                        +EXTD_FLAG_PWINDICON,	0,		0,			0,		// $righticonid
};

qProperties * oTextEdit::properties(void) {
	qProperties *	lvProperties = oBaseVisComponent::properties();
	
	// Add the property definition for our visual component here...
	lvProperties->addElements(oTextEditProperties, sizeof(oTextEditProperties) / sizeof(ECOproperty));
	
	return lvProperties;
};


// set the value of a property
qbool oTextEdit::setProperty(qlong pPropID,EXTfldval &pNewValue,EXTCompInfo* pECI) {
	// most anum properties are managed by Omnis but some we need to do ourselves, no idea why...
	
	switch (pPropID) {
		case oTE_lefticon: {
			mLeftIconID = pNewValue.getLong();
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oTE_righticon: {
			mRightIconID = pNewValue.getLong();
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		default:
			return oBaseVisComponent::setProperty(pPropID, pNewValue, pECI);
			break;
	};
};

// get the value of a property
qbool oTextEdit::getProperty(qlong pPropID,EXTfldval &pGetValue,EXTCompInfo* pECI) {
	// most anum properties are managed by Omnis but some we need to do ourselves...
	
	switch (pPropID) {
		case oTE_lefticon: {
			pGetValue.setLong(mLeftIconID);
            return true;
		}; break;
		case oTE_righticon: {
			pGetValue.setLong(mRightIconID);
            return true;
		}; break;
		default:
			return oBaseVisComponent::getProperty(pPropID, pGetValue, pECI);
			
			break;
	};
};	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// primary data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Changes our primary data
qbool oTextEdit::setPrimaryData(EXTfldval &pNewValue) {
    mContents = pNewValue;
    return true;
};

// Retrieves our primary data, return false if we do not manage a copy
qbool oTextEdit::getPrimaryData(EXTfldval &pGetValue) {
    pGetValue.setChar((qchar *) mContents.cString(), mContents.length());
    return true;
};

// Compare with our primary data, return DATA_CMPDATA_SAME if same, DATA_CMPDATA_DIFFER if different, 0 if not implemented
qlong oTextEdit::cmpPrimaryData(EXTfldval &pWithValue) {
    qstring with(pWithValue);
    
    if (mContents == with) {
        return DATA_CMPDATA_SAME;
    } else {
        return DATA_CMPDATA_DIFFER;
    };
};

// Get our primary data size, return negative if not supported
qlong oTextEdit::getPrimaryDataLen() {
    return mContents.length();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This is our array of methods we support
// ECOmethodEvent oTextEditMethods[] = {
	//	ID				Resource	Return type		Paramcount		Params					Flags		ExFlags
//	1,					8000,		fftCharacter,	0,				NULL,					0,			0,			// $testMethod	
// };

// return an array of method meta data
qMethods * oTextEdit::methods(void) {
	qMethods * lvMethods = oBaseVisComponent::methods();
	
//	lvMethods->addElements(oTextEditMethods, sizeof(oTextEditMethods) / sizeof(ECOmethodEvent));		
	
	return lvMethods;
};

// invoke a method
int	oTextEdit::invokeMethod(qlong pMethodId, EXTCompInfo* pECI) {
/*	switch (pMethodId) {
		case 1: {
			EXTfldval	lvResult;
			str255		lvString(QTEXT("Hello world from our visual object!"));
			
			lvResult.setChar(lvString);
			
			ECOaddParam(pECI, &lvResult);
			return 1L;							
		}; break;
		default: {
			return oBaseVisComponent::invokeMethod(pMethodId, pECI);
		}; break;
	}*/
	return 1L;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// events
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ECOmethodEvent oTextEditEvents[] = {
	//	ID					Resource	Return type		Paramcount		Params					Flags		ExFlags
	oTE_evClick,			5000,		0,				0,				0,						0,			0,
};	
	
	// return an array of events meta data
qEvents *	oTextEdit::events(void) {
	qEvents *	lvEvents = oBaseNVComponent::events();
	
	// Add the event definition for our visual component here...
	lvEvents->addElements(oTextEditEvents, sizeof(oTextEditEvents) / sizeof(ECOmethodEvent));
	
	return lvEvents;
};

void	oTextEdit::evClick(qpoint pAt, EXTCompInfo* pECI) {
	/*
	 Even if our field is disabled we still get these events. 
	 We may move this check into our framework however I can imagine there will be situations where we still want
	 to handle this to some extent even when the field is disabled 
	 */
	if (isEnabled() && isActive() && ECOisOMNISinTrueRuntime(mHWnd)) {
        // need to find out if Omnis has an internal ID for its standard evClick
        ECOsendEvent(mHWnd, oTE_evClick, 0, 0, EEN_EXEC_IMMEDIATE);
    };
};	

