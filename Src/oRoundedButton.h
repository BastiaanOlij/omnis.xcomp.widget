/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oRoundedButton.h
 *  Header for our rounded button object
 *
 *  A rounded border button control
 *
 *  Bastiaan Olij
 */

#ifndef oroundedbuttonh
#define oroundedbuttonh

#include "omnis.xcomp.framework.h"

enum oRoundedButtonPropIDs {
	oRB_radius = 100,
	oRB_fillColor = 101,
	oRB_iconPos = 102,
};

enum oRoundedButtonEventIDs {
	oRB_evClick = 100,
};

class oRoundedButton : public oBaseVisComponent {
private:
	qulong mIconID;
	qjst mIconPos;
	int mRadius;
	qcol mButtonColor;
	qstring mText;
	qlong mButtonMode;

public:
	oRoundedButton(void);
	~oRoundedButton(void);

	static oRoundedButton *newObject(void);

	virtual void doPaint(EXTCompInfo *pECI); // Do our drawing in here

	// properties
	static qProperties *properties(void); // return an array of property data
	virtual qbool setProperty(qlong pPropID, EXTfldval &pNewValue, EXTCompInfo *pECI); // set the value of a property
	virtual qbool getProperty(qlong pPropID, EXTfldval &pGetValue, EXTCompInfo *pECI); // get the value of a property

	// methods
	static qMethods *methods(void); // return array of method meta data
	virtual int invokeMethod(qlong pMethodId, EXTCompInfo *pECI); // invoke a method

	// events
	static qEvents *events(void); // return an array of events meta data
	virtual void evClick(qpoint pAt, EXTCompInfo *pECI); // mouse click at this location

	// mouse related
	virtual void evMouseEnter(); // mouse moved over our object
	virtual void evMouseLeave(); // mouse moved away from our object
};

#endif
