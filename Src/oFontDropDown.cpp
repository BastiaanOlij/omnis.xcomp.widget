/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oFontDropDown.cpp
 *  Source for our font dropdown list object
 *
 *  Drop down list showing each line in the font specified
 *
 *  Bastiaan Olij
 */

#include "oFontDropDown.h"

// Constructor to initialize object
oFontDropDown::oFontDropDown(void) {
	mObjType			= cObjType_DropList;
    mFontColumn         = 1;
};

// Destructor to clean up
oFontDropDown::~oFontDropDown(void) {
	
};

// instantiate a new object
oFontDropDown * oFontDropDown::newObject(void) {
	oFontDropDown *lvNewDropDown = new oFontDropDown();
	
	return lvNewDropDown;
};

// Do our list content drawing here (what we see when the list is collapsed, for cObjType_DropList only)
bool	oFontDropDown::drawListContents(EXTListLineInfo *pInfo, EXTCompInfo* pECI) {
	// Draw our text
    if (pInfo->mLine > 0) {
        EXTfldval *	calcFld;
        EXTfldval fval;
			
        ECOgetProperty(mHWnd,anumListCalc,fval);
        qstring	calculation(fval);
			
        calcFld = newCalculation(calculation, pECI);
		
        if (calcFld != NULL) {
            EXTfldval	result;
            calcFld->evalCalculation(result, pECI->mLocLocp, NULL, qfalse);
            qstring		text(result);
				
            GDItextSpecStruct   textSpec = mCanvas->textSpec();
            str255              fontName;
            qrect               where = pInfo->mLineRect;
            qpoint              leftTop(where.left+10, where.top+3);
        
            // get our font name
            EXTfldval fontNameFld;
            str255  fontNameStr;
            pInfo->mListPtr->getColVal(pInfo->mLine, mFontColumn, fftCharacter, 0, fontNameFld);
            fontNameFld.getChar(fontNameStr);
            GDIsetFontName(&textSpec.mFnt, fontNameStr.cString(), fontNameStr.length());

            // and draw our text
            mCanvas->drawText(text.cString(), leftTop, textSpec);
		
            delete calcFld;
        };
    };

	return true;
};

// Do our list line drawing here (for cObjType_List or cObjType_DropList)
bool	oFontDropDown::drawListLine(EXTListLineInfo *pInfo, EXTCompInfo* pECI) {
	// draw our text
	qstring *	text = newStringFromParam(1, pECI); // first parameter contains our calculated text :)
	if (text!=NULL) {
        GDItextSpecStruct   textSpec = mCanvas->textSpec();
        str255              fontName;
		qrect               where = pInfo->mLineRect;
        qpoint              leftTop(where.left+20, where.top+3);
        
        // get our font name
        EXTfldval fontNameFld;
        str255  fontNameStr;
        pInfo->mListPtr->getColVal(pInfo->mLine, mFontColumn, fftCharacter, 0, fontNameFld);
        fontNameFld.getChar(fontNameStr);
        GDIsetFontName(&textSpec.mFnt, fontNameStr.cString(), fontNameStr.length());

        // and draw our text
		mCanvas->drawText(text->cString(), leftTop, textSpec);
		
		delete text;
	};
	
	return true; // we have drawn this...
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ECOproperty oFontDropDownProperties[] = {
	//	ID						ResID	Type			Flags					ExFlags	EnumStart	EnumEnd
	oFDD_fontColumn,			4201,	fftInteger,     EXTD_FLAG_PROPGENERAL,	0,		0,			0,		// $fontColumn
};

qProperties * oFontDropDown::properties(void) {
	qProperties *	lvProperties = oBaseComponent::properties();	// we skip the properties in oBaseVisComponent, Omnis implements those in the background for drop down controls
	
	// Add the property definition for our visual component here...
	lvProperties->addElements(oFontDropDownProperties, sizeof(oFontDropDownProperties) / sizeof(ECOproperty));
	
	return lvProperties;
};


// set the value of a property
qbool oFontDropDown::setProperty(qlong pPropID,EXTfldval &pNewValue,EXTCompInfo* pECI) {
	// most anum properties are managed by Omnis but some we need to do ourselves, no idea why...
	
	switch (pPropID) {
		case oFDD_fontColumn: {
			mFontColumn = pNewValue.getLong();
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		default:
			return oBaseVisComponent::setProperty(pPropID, pNewValue, pECI);
			break;
	};
};

// get the value of a property
void oFontDropDown::getProperty(qlong pPropID,EXTfldval &pGetValue,EXTCompInfo* pECI) {
	// most anum properties are managed by Omnis but some we need to do ourselves...
	
	switch (pPropID) {
		case oFDD_fontColumn: {
			pGetValue.setLong(mFontColumn);
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
// ECOmethodEvent oFontDropDownMethods[] = {
//	ID				Resource	Return type		Paramcount		Params					Flags		ExFlags
//	1,					8000,		fftCharacter,	0,				NULL,					0,			0,			// $testMethod	
// };

// return an array of method meta data
qMethods * oFontDropDown::methods(void) {
	qMethods * lvMethods = oBaseVisComponent::methods();
	
	//	lvMethods->addElements(oDropDownMethods, sizeof(oDropDownMethods) / sizeof(ECOmethodEvent));		
	
	return lvMethods;
};

// invoke a method
int	oFontDropDown::invokeMethod(qlong pMethodId, EXTCompInfo* pECI) {
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

ECOmethodEvent oFontDropDownEvents[] = {
	//	ID					Resource	Return type		Paramcount		Params					Flags		ExFlags
	oFDD_evClick,			5000,		0,				0,				0,						0,			0,
};	

// return an array of events meta data
qEvents *	oFontDropDown::events(void) {
	qEvents *	lvEvents = oBaseNVComponent::events();
	
	// Add the event definition for our visual component here...
	lvEvents->addElements(oFontDropDownEvents, sizeof(oFontDropDownEvents) / sizeof(ECOmethodEvent));
	
	return lvEvents;
};

void	oFontDropDown::evClick(qpoint pAt, EXTCompInfo* pECI) {
	// need to find out if Omnis has an internal ID for its standard evClick
	ECOsendEvent(mHWnd, oFDD_evClick, 0, 0, EEN_EXEC_IMMEDIATE);
};	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mouse
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// mouse left button pressed down (return true if we finished handling this, false if we want Omnis internal logic)
bool	oFontDropDown::evMouseLDown(qpoint pDownAt) {
	return false; // let omnis do its thing
};

// mouse left button released (return true if we finished handling this, false if we want Omnis internal logic)
bool	oFontDropDown::evMouseLUp(qpoint pUpAt) {
	return false; // let omnis do its thing
};

