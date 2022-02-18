/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oCountButton.cpp
 *  Implementation of our counter button object
 *
 *  Bastiaan Olij
 */

#include "oCountButton.h"

// Constructor to initialize object
oCountButton::oCountButton(void) {
	mBKTheme = WND_BK_PARENT; /* now that we use our build in properties properly we need to find out how to properly default this */
	mCounter = 0;
	mIconID = 0;
	mRadius = 5;
	mSpacing = 0;
	mCountColour = GDI_COLOR_QBLUE;
};

// Destructor to clean up
oCountButton::~oCountButton(void){

};

// instantiate a new object
oCountButton *oCountButton::newObject(void) {
	oCountButton *lvNewCountButton = new oCountButton();

	return lvNewCountButton;
};

// Do our drawing in here
void oCountButton::doPaint(EXTCompInfo *pECI) {
	// call base class to draw background
	oBaseVisComponent::doPaint(pECI);

	if (mIconID != 0) {
		mCanvas->drawIcon(mIconID, mClientRect);
	};

	if (mCounter > 0) {
		qstring tmpCounter;

		if (mCounter >= 1000) {
			tmpCounter = QTEXT("***");
		} else {
			tmpCounter.setFormattedString("%li", mCounter);
		};

		int tmpLeft = mClientRect.right - (mRadius * 2);
		qrect tmpRect = qrect(tmpLeft - mSpacing, mClientRect.top, mClientRect.right, mClientRect.top + (mRadius * 2));
		mCanvas->drawEllipse(tmpRect, mCanvas->mixColors(GDI_COLOR_QWHITE, mCountColour), mCountColour, GDI_COLOR_QWHITE, mSpacing);

		tmpRect.left += 2;
		tmpRect.top += 2;
		tmpRect.bottom = mClientRect.bottom;
		mCanvas->drawText(tmpCounter.cString(), tmpRect, GDI_COLOR_QWHITE, jstCenter, false, false);
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ECOproperty oCountButtonProperties[] = {
	//	ID						ResID	Type			Flags					ExFlags	EnumStart	EnumEnd
	anumIconID, 0, fftInteger, EXTD_FLAG_PROPAPP, 0, 0, 0, // $iconid
	oCB_counter, 4100, fftInteger, EXTD_FLAG_PROPDATA, 0, 0, 0, // $counter
	oCB_radius, 4101, fftInteger, EXTD_FLAG_PROPDATA, 0, 0, 0, // $radius
	oCB_spacing, 4102, fftInteger, EXTD_FLAG_PROPDATA, 0, 0, 0, // $spacing
	oCB_countColour, 4103, fftInteger, EXTD_FLAG_PROPAPP + EXTD_FLAG_PWINDCOL, 0, 0, 0, // $countColour
};

qProperties *oCountButton::properties(void) {
	qProperties *lvProperties = oBaseVisComponent::properties();

	// Add the property definition for our visual component here...
	lvProperties->addElements(oCountButtonProperties, sizeof(oCountButtonProperties) / sizeof(ECOproperty));

	return lvProperties;
};

// set the value of a property
qbool oCountButton::setProperty(qlong pPropID, EXTfldval &pNewValue, EXTCompInfo *pECI) {
	// most anum properties are managed by Omnis but some we need to do ourselves...

	switch (pPropID) {
		case anumIconID: {
			mIconID = pNewValue.getLong();
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oCB_counter: {
			mCounter = pNewValue.getLong();
			if (mCounter < 0) {
				mCounter = 0;
				mSpacing = 0;
			} else if (mCounter < 10) {
				mSpacing = 0;
			} else if (mCounter < 100) {
				mSpacing = 10;
			} else {
				mSpacing = 20;
			};

			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oCB_radius: {
			mRadius = pNewValue.getLong();
			if (mRadius < 2) {
				mRadius = 2;
			};
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oCB_spacing: {
			mSpacing = pNewValue.getLong();
			if (mSpacing < 0) {
				mSpacing = 0;
			};
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oCB_countColour: {
			mCountColour = pNewValue.getLong();
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		default:
			return oBaseVisComponent::setProperty(pPropID, pNewValue, pECI);
			break;
	};
};

// get the value of a property
qbool oCountButton::getProperty(qlong pPropID, EXTfldval &pGetValue, EXTCompInfo *pECI) {
	// most anum properties are managed by Omnis but some we need to do ourselves...

	switch (pPropID) {
		case anumIconID: {
			pGetValue.setLong(mIconID);
			return true;
		}; break;
		case oCB_counter: {
			pGetValue.setLong(mCounter);
			return true;
		}; break;
		case oCB_radius: {
			pGetValue.setLong(mRadius);
			return true;
		}; break;
		case oCB_spacing: {
			pGetValue.setLong(mSpacing);
			return true;
		}; break;
		case oCB_countColour: {
			pGetValue.setLong(mCountColour);
			return true;
		}; break;
		default:
			return oBaseVisComponent::getProperty(pPropID, pGetValue, pECI);

			break;
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This is our array of methods we support
// ECOmethodEvent oCountButtonMethods[] = {
//	ID				Resource	Return type		Paramcount		Params					Flags		ExFlags
//	1,					8000,		fftCharacter,	0,				NULL,					0,			0,			// $testMethod
// };

// return an array of method meta data
qMethods *oCountButton::methods(void) {
	qMethods *lvMethods = oBaseVisComponent::methods();

	//	lvMethods->addElements(oCountButtonMethods, sizeof(oCountButtonMethods) / sizeof(ECOmethodEvent));

	return lvMethods;
};

// invoke a method
int oCountButton::invokeMethod(qlong pMethodId, EXTCompInfo *pECI) {
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

ECOmethodEvent oCountButtonEvents[] = {
	//	ID					Resource	Return type		Paramcount		Params					Flags		ExFlags
	oCB_evClick,
	5000,
	0,
	0,
	0,
	0,
	0,
};

// return an array of events meta data
qEvents *oCountButton::events(void) {
	qEvents *lvEvents = oBaseNVComponent::events();

	// Add the event definition for our visual component here...
	lvEvents->addElements(oCountButtonEvents, sizeof(oCountButtonEvents) / sizeof(ECOmethodEvent));

	return lvEvents;
};

void oCountButton::evClick(qpoint pAt, EXTCompInfo *pECI) {
	/*
	 Even if our field is disabled we still get these events. 
	 We may move this check into our framework however I can imagine there will be situations where we still want
	 to handle this to some extent even when the field is disabled 
	 */
	if (isEnabled() && isActive() && ECOisOMNISinTrueRuntime(mHWnd)) {
		// need to find out if Omnis has an internal ID for its standard evClick
		ECOsendEvent(mHWnd, oCB_evClick, 0, 0, EEN_EXEC_IMMEDIATE);
	};
};
