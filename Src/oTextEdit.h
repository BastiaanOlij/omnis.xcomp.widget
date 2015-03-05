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

#ifndef otextedith
#define otextedith

#include "omnis.xcomp.framework.h"

enum oTextEditPropIDs {
	oTE_lefticon			= 100,
	oTE_righticon			= 101,
};

enum oTextEditEventIDs {
	oTE_evClick             = 100,
};

class oTextEdit : public oBaseVisComponent {
private:
	qulong						mLeftIconID;
	qulong						mRightIconID;
    qstring                     mContents;
	
public:
	oTextEdit(void);
	~oTextEdit(void);
	
	static oTextEdit *          newObject(void);
	
	virtual void				doPaint(EXTCompInfo* pECI);											// Do our drawing in here
	
	// properties
	static  qProperties *		properties(void);													// return an array of property data
	virtual qbool				setProperty(qlong pPropID,EXTfldval &pNewValue,EXTCompInfo* pECI);	// set the value of a property
	virtual qbool				getProperty(qlong pPropID,EXTfldval &pGetValue,EXTCompInfo* pECI);	// get the value of a property

	virtual qbool				setPrimaryData(EXTfldval &pNewValue);								// Changes our primary data
	virtual qbool				getPrimaryData(EXTfldval &pGetValue);								// Retrieves our primary data, return false if we do not manage a copy
	virtual qlong				cmpPrimaryData(EXTfldval &pWithValue);								// Compare with our primary data, return DATA_CMPDATA_SAME if same, DATA_CMPDATA_DIFFER if different, 0 if not implemented
	virtual qlong				getPrimaryDataLen();												// Get our primary data size, return negative if not supported
	
	// methods
	static  qMethods *			methods(void);														// return array of method meta data
	virtual int					invokeMethod(qlong pMethodId, EXTCompInfo* pECI);					// invoke a method
	
	// events
	static	qEvents *			events(void);														// return an array of events meta data
	virtual void				evClick(qpoint pAt, EXTCompInfo* pECI);								// mouse click at this location
};

#endif
