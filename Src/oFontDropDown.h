/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oFontDropDown.h
 *  Header for our font dropdown list object
 *
 *  Drop down list showing each line in the font specified
 *
 *  Bastiaan Olij
 */

#ifndef ofontdropdownh
#define ofontdropdownh

#include "omnis.xcomp.framework.h"


enum oFontDropDownPropIDs {
	oFDD_fontColumn         = 100,
};

enum oFontDropDownEventIDs {
	oFDD_evClick			= 100,
};


class oFontDropDown : public oBaseVisComponent {
private:
    qlong                       mFontColumn;                                                        // column in our list that holds our font name

public:
	oFontDropDown(void);
	~oFontDropDown(void);
	
	static oFontDropDown *		newObject(void);
	
	virtual bool				drawListContents(EXTListLineInfo *pInfo, EXTCompInfo* pECI);		// Do our list content drawing here (what we see when the list is collapsed, for cObjType_DropList only)
	virtual bool				drawListLine(EXTListLineInfo *pInfo, EXTCompInfo* pECI);			// Do our list line drawing here (for cObjType_List or cObjType_DropList)
	
	// properties
	static  qProperties *		properties(void);													// return an array of property data
	virtual qbool				setProperty(qlong pPropID,EXTfldval &pNewValue,EXTCompInfo* pECI);	// set the value of a property
	virtual void				getProperty(qlong pPropID,EXTfldval &pGetValue,EXTCompInfo* pECI);	// get the value of a property

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
