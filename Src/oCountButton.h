/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oCountButton.h
 *  Header for our counter button object
 *
 *  This is a nice little control that shows an icon and a message count
 *
 *  Bastiaan Olij
 */

#ifndef ocountbuttonh
#define ocountbuttonh

#include "omnis.xcomp.framework.h"

enum oCountButtonPropIDs {
	oCB_counter			= 100,
	oCB_radius			= 101,
	oCB_spacing			= 102,
	oCB_countColour		= 103,
};

enum oCountBUttonEventIDs {
	oCB_evClick			= 100,
};

class oCountButton : public oBaseVisComponent {
private:
	qulong						mIconID;
	int							mCounter;
	int							mRadius;
	int							mSpacing;
	qcol						mCountColour;
	
public:
	oCountButton(void);
	~oCountButton(void);
	
	static oCountButton *		newObject(void);
	
	virtual void				doPaint(EXTCompInfo* pECI);											// Do our drawing in here
	
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
};

#endif
