/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oDropDown.cpp
 *  Source for our dropdown list object
 *
 *  Omnis' drop down build in dropdown list now supports styled text but it seems lacking
 *  I've also needed a proper multi select dropdown list control for awhile where we
 *  can show which lines are selected and show more info in the control itself
 *
 *  Bastiaan Olij
 */

#include "oDropDown.h"

// Constructor to initialize object
oDropDown::oDropDown(void) {
	mObjType			= cObjType_DropList;
	mDisplayCalc		= "";
};

// Destructor to clean up
oDropDown::~oDropDown(void) {
	
};

// instantiate a new object
oDropDown * oDropDown::newObject(void) {
	oDropDown *lvNewDropDown = new oDropDown();
	
	return lvNewDropDown;
};

// Do our list content drawing here (what we see when the list is collapsed, for cObjType_DropList only)
bool	oDropDown::drawListContents(EXTListLineInfo *pInfo, EXTCompInfo* pECI) {	
	// Draw our text
	EXTfldval *	calcFld;
		
	if (mDisplayCalc.length()==0) {
		EXTfldval fval;
			
		ECOgetProperty(mHWnd,anumListCalc,fval); 
		qstring	calculation(fval);
			
		calcFld = newCalculation(calculation, pECI);
	} else {
		calcFld = newCalculation(mDisplayCalc, pECI);
	};
		
	if (calcFld != NULL) {
		EXTfldval	result;
		calcFld->evalCalculation(result, pECI->mLocLocp, NULL, qfalse);
		qstring		text(result);
			
//			addToTraceLog("Result: %qs", &text);
		qrect	where = pInfo->mLineRect;
		where.left	+= 10;
		where.top	+= 3;
		
		mCanvas->drawText(text.cString(), where, mTextColor);
		
		delete calcFld;
	};
};

// Do our list line drawing here (for cObjType_List or cObjType_DropList)
bool	oDropDown::drawListLine(EXTListLineInfo *pInfo, EXTCompInfo* pECI) {
	// Omnis seems to already have put a tick infront of our current line. grmbl grmbl grmbl
	// Whenever selecting a line to toggle you'll need to react appropriately
	
	bool	isSelected = pInfo->mListPtr->isRowSelected(pInfo->mLine,qfalse);
	
	if (isSelected && mShowSelected) {
		mCanvas->drawIcon(1655+cBMPicon16x16, qpoint(pInfo->mLineRect.left, pInfo->mLineRect.top));
	};
	
	// draw our text
	qstring *	text = newStringFromParam(1, pECI); // first parameter contains our calculated text :)
	if (text!=NULL) {	
		qrect	where = pInfo->mLineRect;
		where.left	+= 20;
		where.top	+= 3;
		
		mCanvas->drawText(text->cString(), where, mTextColor);
		
		delete text;
	};
	
	return true; // we have drawn this...
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ECOproperty oDropDownProperties[] = { 
	//	ID						ResID	Type			Flags					ExFlags	EnumStart	EnumEnd
	anumShowselected,			0,		fftBoolean,		EXTD_FLAG_PROPACT,		0,		0,			0,		// $multiselect
	oDD_displayCalc,			4200,	fftCharacter,	EXTD_FLAG_PROPGENERAL,	0,		0,			0,		// $displaycalc
};	

qProperties * oDropDown::properties(void) {
	qProperties *	lvProperties = oBaseComponent::properties();	// we skip the properties in oBaseVisComponent, Omnis implements those in the background for drop down controls
	
	// Add the property definition for our visual component here...
	lvProperties->addElements(oDropDownProperties, sizeof(oDropDownProperties) / sizeof(ECOproperty));
	
	return lvProperties;
};


// set the value of a property
qbool oDropDown::setProperty(qlong pPropID,EXTfldval &pNewValue,EXTCompInfo* pECI) {
	// most anum properties are managed by Omnis but some we need to do ourselves, no idea why...
	
	switch (pPropID) {
		case anumShowselected: {
			mShowSelected = pNewValue.getBool()==2;
			WNDinvalidateRect(mHWnd, NULL);			
			return qtrue;			
		}; break;
		case oDD_displayCalc: {
			mDisplayCalc = pNewValue;
			WNDinvalidateRect(mHWnd, NULL);			
			return qtrue;			
		}; break;
		default:
			return oBaseVisComponent::setProperty(pPropID, pNewValue, pECI);
			break;
	};
};

// get the value of a property
void oDropDown::getProperty(qlong pPropID,EXTfldval &pGetValue,EXTCompInfo* pECI) {
	// most anum properties are managed by Omnis but some we need to do ourselves...
	
	switch (pPropID) {
		case anumShowselected: {
			pGetValue.setBool(mShowSelected ? 2 : 1);
		}; break;
		case oDD_displayCalc: {
			pGetValue.setChar((qchar *) mDisplayCalc.cString(), mDisplayCalc.length());
		}; break;
		default:
			oBaseVisComponent::getProperty(pPropID, pGetValue, pECI);
			
			break;
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This is our array of methods we support
// ECOmethodEvent oDropDownMethods[] = {
//	ID				Resource	Return type		Paramcount		Params					Flags		ExFlags
//	1,					8000,		fftCharacter,	0,				NULL,					0,			0,			// $testMethod	
// };

// return an array of method meta data
qMethods * oDropDown::methods(void) {
	qMethods * lvMethods = oBaseVisComponent::methods();
	
	//	lvMethods->addElements(oDropDownMethods, sizeof(oDropDownMethods) / sizeof(ECOmethodEvent));		
	
	return lvMethods;
};

// invoke a method
int	oDropDown::invokeMethod(qlong pMethodId, EXTCompInfo* pECI) {
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

ECOmethodEvent oDropDownEvents[] = {
	//	ID					Resource	Return type		Paramcount		Params					Flags		ExFlags
	oDD_evClick,			5000,		0,				0,				0,						0,			0,
};	

// return an array of events meta data
qEvents *	oDropDown::events(void) {
	qEvents *	lvEvents = oBaseNVComponent::events();
	
	// Add the event definition for our visual component here...
	lvEvents->addElements(oDropDownEvents, sizeof(oDropDownEvents) / sizeof(ECOmethodEvent));
	
	return lvEvents;
};

void	oDropDown::evClick(qpoint pAt, EXTCompInfo* pECI) {
	// need to find out if Omnis has an internal ID for its standard evClick
	ECOsendEvent(mHWnd, oDD_evClick, 0, 0, EEN_EXEC_IMMEDIATE);	
};	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mouse
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// mouse left button pressed down (return true if we finished handling this, false if we want Omnis internal logic)
bool	oDropDown::evMouseLDown(qpoint pDownAt) {
	return false; // let omnis do its thing
};

// mouse left button released (return true if we finished handling this, false if we want Omnis internal logic)
bool	oDropDown::evMouseLUp(qpoint pUpAt) {
	return false; // let omnis do its thing
};

