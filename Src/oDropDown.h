/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oDropDown.h
 *  Header for our dropdown list object
 *
 *  Omnis' drop down build in dropdown list now supports styled text but it seems lacking
 *  I've also needed a proper multi select dropdown list control for awhile where we
 *  can show which lines are selected and show more info in the control itself
 *
 *  Bastiaan Olij
 */

#ifndef odropdownh
#define odropdownh

#include "omnis.xcomp.framework.h"


enum oDropDownPropIDs {
	oDD_displayCalc		= 100,
};

enum oDropDownEventIDs {
	oDD_evClick			= 100,
};


class oDropDown : public oBaseVisComponent {
private:
	bool						mShowSelected;														// Show selected lines?
	qstring						mDisplayCalc;														// Display calculation

public:
	oDropDown(void);
	~oDropDown(void);
	
	static oDropDown *			newObject(void);
	
	virtual bool				drawListContents(EXTListLineInfo *pInfo, EXTCompInfo* pECI);		// Do our list content drawing here (what we see when the list is collapsed, for cObjType_DropList only)
	virtual bool				drawListLine(EXTListLineInfo *pInfo, EXTCompInfo* pECI);			// Do our list line drawing here (for cObjType_List or cObjType_DropList)
	
	// properties
	static  qProperties *		properties(void);													// return an array of property data
	virtual qbool				setProperty(qlong pPropID,EXTfldval &pNewValue,EXTCompInfo* pECI);	// set the value of a property
	virtual qbool				getProperty(qlong pPropID,EXTfldval &pGetValue,EXTCompInfo* pECI);	// get the value of a property

	// methods
	static  qMethods *			methods(void);														// return array of method meta data
	virtual int					invokeMethod(qlong pMethodId, EXTCompInfo* pECI);					// invoke a method
	
	// events
	static	qEvents *			events(void);														// return an array of events meta data
	virtual void				evClick(qpoint pAt, EXTCompInfo* pECI);								// mouse click at this location
	
	// mouse
	virtual bool				evMouseLDown(qpoint pDownAt);										// mouse left button pressed down (return true if we finished handling this, false if we want Omnis internal logic)
	virtual bool				evMouseLUp(qpoint pUpAt);											// mouse left button released (return true if we finished handling this, false if we want Omnis internal logic)
};


#endif
