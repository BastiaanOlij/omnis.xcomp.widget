/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oRoundedButton.cpp
 *  Implementation for our rounded button object
 *
 *  A rounded border button control
 *
 *  Bastiaan Olij
 */

#include "oRoundedButton.h"

// Constructor to initialize object
oRoundedButton::oRoundedButton(void) {
	mShowName = false;
	mBKTheme = WND_BK_PARENT; /* now that we use our build in properties properly we need to find out how to properly default this */
	mIconID = 0;
	mIconPos = jstLeft;
	mRadius = 8;
	mButtonColor = GDI_COLOR_QDKCYAN;
	mBorderColor = GDI_COLOR_QDEFAULT;
	mText = QTEXT("");
	mButtonMode = 0;
};

// Destructor to clean up
oRoundedButton::~oRoundedButton(void){

};

// instantiate a new object
oRoundedButton *oRoundedButton::newObject(void) {
	oRoundedButton *lvNewCountButton = new oRoundedButton();

	return lvNewCountButton;
};

// Do our drawing in here
void oRoundedButton::doPaint(EXTCompInfo *pECI) {
	// call base class to draw background
	oBaseVisComponent::doPaint(pECI);

	// set a rectangle for our contents
	qrect contents = mClientRect;
	contents.left += mRadius;
	contents.right -= mRadius;

	// draw our rectangle
	bool enabled = isEnabled() && isActive();
	if (!enabled) {
		// draw in gray
		mCanvas->drawRoundedRect(mClientRect, mRadius * 2, GDI_COLOR_QGRAY, GDI_COLOR_QDEFAULT);
	} else if (mouseIsOver()) {
		mCanvas->drawRoundedRect(mClientRect, mRadius * 2, mButtonColor, GDIgetDarkerShade(mBorderColor = GDI_COLOR_QDEFAULT ? mButtonColor : mBorderColor));
	} else {
		mCanvas->drawRoundedRect(mClientRect, mRadius * 2, mButtonColor, mBorderColor = GDI_COLOR_QDEFAULT ? mButtonColor : mBorderColor);
	};

	// and our icon...
	if (mIconID != 0) {
		qdim size = mCanvas->drawIcon(mIconID, contents, mIconPos, jstCenter, enabled);
		if (mIconPos == jstRight) {
			contents.right -= size + 2;
		} else {
			contents.left += size + 2;
		};
	};

	contents.top += 2;
	contents.bottom -= 2;
	qstring mWrapped = mCanvas->wrapText(mText.cString(), contents.width()); // save some performance by wrapping it once.
	qdim height = mCanvas->getTextHeight(mWrapped.cString(), contents.width(), true, false);
	contents.top += (contents.height() - height) / 2; // vertical center
	mCanvas->drawText(mWrapped.cString(), contents, enabled ? GDI_COLOR_QDEFAULT : GDI_COLOR_QDKGRAY, jstNone, true, false);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ECOproperty oRoundedButtonProperties[] = {
	//	ID						ResID	Type			Flags					ExFlags	EnumStart	EnumEnd
	anumText, 0, fftCharacter, EXTD_FLAG_PROPGENERAL, 0, 0, 0, // $text
	anumIconID, 0, fftInteger, EXTD_FLAG_PROPAPP, 0, 0, 0, // $iconid
	oRB_iconPos, 4105, fftInteger, EXTD_FLAG_PROPAPP + EXTD_FLAG_INTCONSTANT, 0, preJstLeft, preJstRight, // $iconPos
	oRB_fillColor, 4104, fftInteger, EXTD_FLAG_PROPAPP + EXTD_FLAG_PWINDCOL, 0, 0, 0, // $fillcolor
	oRB_radius, 4101, fftInteger, EXTD_FLAG_PROPAPP, 0, 0, 0, // $radius
	anumButtonmode, 0, fftInteger, EXTD_FLAG_PROPACT + EXTD_FLAG_INTCONSTANT, 0, preButtmodeUser, preButtmodeCancel, // $buttonmode
};

qProperties *oRoundedButton::properties(void) {
	qProperties *lvProperties = oBaseVisComponent::properties();

	// Add the property definition for our visual component here...
	lvProperties->addElements(oRoundedButtonProperties, sizeof(oRoundedButtonProperties) / sizeof(ECOproperty));

	return lvProperties;
};

// set the value of a property
qbool oRoundedButton::setProperty(qlong pPropID, EXTfldval &pNewValue, EXTCompInfo *pECI) {
	// most anum properties are managed by Omnis but some we need to do ourselves, no idea why...

	switch (pPropID) {
		case anumText: {
			mText = pNewValue;
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case anumIconID: {
			mIconID = pNewValue.getLong();
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oRB_iconPos: {
			mIconPos = (qjst)pNewValue.getLong();
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oRB_fillColor: {
			mButtonColor = pNewValue.getLong();
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oRB_radius: {
			mRadius = pNewValue.getLong();
			if (mRadius < 2) {
				mRadius = 2;
			};
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case anumButtonmode: {
			mButtonMode = pNewValue.getLong();
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		default:
			return oBaseVisComponent::setProperty(pPropID, pNewValue, pECI);
			break;
	};
};

// get the value of a property
qbool oRoundedButton::getProperty(qlong pPropID, EXTfldval &pGetValue, EXTCompInfo *pECI) {
	// most anum properties are managed by Omnis but some we need to do ourselves...

	switch (pPropID) {
		case anumText: {
			pGetValue.setChar((qchar *)mText.cString(), mText.length());
			return true;
		}; break;
		case anumIconID: {
			pGetValue.setLong(mIconID);
			return true;
		}; break;
		case oRB_iconPos: {
			pGetValue.setLong(mIconPos);
			return true;
		}; break;
		case oRB_fillColor: {
			pGetValue.setLong(mButtonColor);
			return true;
		}; break;
		case oRB_radius: {
			pGetValue.setLong(mRadius);
			return true;
		}; break;
		case anumButtonmode: {
			pGetValue.setLong(mButtonMode);
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
// ECOmethodEvent oRoundedButtonMethods[] = {
//	ID				Resource	Return type		Paramcount		Params					Flags		ExFlags
//	1,					8000,		fftCharacter,	0,				NULL,					0,			0,			// $testMethod
// };

// return an array of method meta data
qMethods *oRoundedButton::methods(void) {
	qMethods *lvMethods = oBaseVisComponent::methods();

	//	lvMethods->addElements(oRoundedButtonMethods, sizeof(oRoundedButtonMethods) / sizeof(ECOmethodEvent));

	return lvMethods;
};

// invoke a method
int oRoundedButton::invokeMethod(qlong pMethodId, EXTCompInfo *pECI) {
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

ECOmethodEvent oRoundedButtonEvents[] = {
	//	ID					Resource	Return type		Paramcount		Params					Flags		ExFlags
	oRB_evClick,
	5000,
	0,
	0,
	0,
	0,
	0,
};

// return an array of events meta data
qEvents *oRoundedButton::events(void) {
	qEvents *lvEvents = oBaseNVComponent::events();

	// Add the event definition for our visual component here...
	lvEvents->addElements(oRoundedButtonEvents, sizeof(oRoundedButtonEvents) / sizeof(ECOmethodEvent));

	return lvEvents;
};

void oRoundedButton::evClick(qpoint pAt, EXTCompInfo *pECI) {
	/* this won't be called, we're leaving it up to Omnis to do its thing */

	/*
	 Even if our field is disabled we still get these events. 
	 We may move this check into our framework however I can imagine there will be situations where we still want
	 to handle this to some extent even when the field is disabled 
	 */
	if (isEnabled() && isActive() && ECOisOMNISinTrueRuntime(mHWnd)) {
		// need to find out if Omnis has an internal ID for its standard evClick
		if (!ECOsendEvent(mHWnd, oRB_evClick, 0, 0, EEN_EXEC_IMMEDIATE)) {
			// ignore, it was discarded
		} else if (mButtonMode == 1) {
			// need to implement
		} else if (mButtonMode == 2) {
			// need to implement
		};
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mouse
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// mouse moved over our object
void oRoundedButton::evMouseEnter() {
	WNDinvalidateRect(mHWnd, NULL);
};

// mouse moved away from our object
void oRoundedButton::evMouseLeave() {
	WNDinvalidateRect(mHWnd, NULL);
};
