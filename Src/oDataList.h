/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oDataList.h
 *  Header for our datalist object
 *
 *  Omnis has a number of list controls including a very poor tree list control. I wanted something with a little more umpf.
 *  This is my attempt at creating a cool data list control.
 *  Note that on purpose I am not subclassing from the list objects in the SDK. They do not give me the level of control I wish
 *
 *  Bastiaan Olij
 */

#ifndef odatalisth
#define odatalisth

#include "oDLNode.h"
#include "omnis.xcomp.framework.h"

enum oDataListPropIDs {
	oDL_columncount = 100,
	oDL_columncalcs = 101,
	oDL_columnwidths = 102,
	oDL_columnaligns = 103,
	oDL_maxrowheight = 104,
	oDL_columnprefix = 105,
	oDL_verticalExtend = 106,
	oDL_evenColor = 107,
	oDL_selectColor = 108,
	oDL_groupcalcs = 120,
	oDL_treeIndent = 121,
	oDL_filtercalc = 122,
	oDL_deselNodeClick = 123,
	oDL_parentCalcs = 124,
};

enum oDataListEventIDs {
	oDL_evClick = 100,
	oDL_evHScrolled = 101,
	oDL_evVScrolled = 102,
	oDL_evColumnResized = 103,
	oDL_evDoubleClick = 104,
};

enum oDataListHittest {
	oDL_none,
	oDL_horzSplitter,
	oDL_treeIcon,
	oDL_node,
	oDL_row
};

typedef struct sDLHitTest {
	oDataListHittest mAbove;
	unsigned int mColNo;
	oDLNode *mNode;
	qlong mLineNo;
} sDLHitTest;

typedef struct sDLGrouping {
	qstring *mGroupCalc; // group calculation
	qstring *mParentCalc; // parent calculation
} sDLGrouping;

class oDataList : public oBaseVisComponent {
private:
	bool mRebuildNodes; // if true we need to rebuild our nodes
	int mDrawRecurseCount; // count to protect us from recursive calls into doPaint
	bool mInWindowScrolled; // are we already handling our window scroll?

	bool mShowSelected; // if true we show selected lines, if false we only show the current line
	qdim mIndent; // Indent for our tree
	qdim mLineSpacing; // Empty space between lines in pixels
	qdim mLastCurrentLineTop; // Top position of current line at the last redraw, if changed we check if this is on screen

	std::vector<sDLGrouping> mGroupCalculations; // Calculations by which we're grouping
	bool mDeselectOnNodeClick; // Deselect lines when the user clicks on a node
	qlong mParentGrouping; // Group that can also be a line
	qstring mParentCalculation; // If this calculation returns true, the line is our parent line

	qulong mColumnCount; // Number of columns we are displaying
	qArray<qstring *> mColumnCalculations; // Calculations for displaying our column data
	qdimArray mColumnWidths; // Our column widths
	qArray<qjst> mColumnAligns; // Our column aligns
	qstring mColumnPrefix; // Column prefix calculation
	bool mColumnExtend[256]; // Flags to indicate which columns extend our row height
	qdim mMaxRowHeight; // Maximum rowheight
	qcol mEvenColor; // Background color for even lines
	qcol mSelectColor; // Background color for selected lines

	qstring mFilter; // Our filter

	oDLNode mRootNode; // Our root node
	bool mCheckedDataName; // check if we've attempted to convert our dataname
	qstring mListName; // Contents of $listname
	EXTqlist *mOmnisList; // List pointed to by $listname (only set during events)

	qlong mLastVisListNo; // Last list line no we actually drew..
	sDLHitTest mMouseHitTest; // Our hittest info when our mouse button was pressed
	qpoint mMouseLast; // Last mouse position to calculate deltas

	void clearGroupCalcs(void); // Clear our group calculations
	void clearColumnCalcs(void); // Clear our column calculations

	void checkColumns(void); // Check if our column data is complete
	qdim drawDividers(qdim pTop, qdim pBottom); // Draw divider lines
	qdim drawNode(EXTCompInfo *pECI, oDLNode &pNodeqdim, qdim pIndent, qdim pTop, qlong &pListLineNo, bool &pIsEven); // Draw this node
	qdim drawRow(EXTCompInfo *pECI, qlong pLineNo, qdim pIndent, qdim pTop, bool pIsEven); // Draw this row

	void clearHitTest(void); // clear our hitttest info
	sDLHitTest doHitTest(qpoint pAt); // find what we are above

public:
	oDataList(void);
	~oDataList(void);

	static oDataList *newObject(void);

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

	// scrolling
	virtual qdim getVertStepSize(void); // get our vertical step size
	virtual void evWindowScrolled(qdim pNewX, qdim pNewY); // window was scrolled

	// mouse
	virtual HCURSOR getCursor(qpoint pAt, qword2 pHitTest); // return the mouse cursor we should show
	virtual bool evMouseLDown(qpoint pDownAt); // mouse left button pressed down (return true if we finished handling this, false if we want Omnis internal logic)
	virtual bool evMouseLUp(qpoint pDownAt); // mouse left button released (return true if we finished handling this, false if we want Omnis internal logic)
	virtual void evMouseMoved(qpoint pMovedTo); // mouse moved to this location while mouse button is not down
	virtual void evClick(qpoint pAt, EXTCompInfo *pECI); // mouse click at this location
	virtual bool evDoubleClick(qpoint pAt, EXTCompInfo *pECI); // mouse left button double clicked (return true if we finished handling this, false if we want Omnis internal logic)
	virtual bool evMouseRDown(qpoint pDownAt, EXTCompInfo *pECI); // mouse right button pressed down (return true if we finished handling this, false if we want Omnis internal logic)

	// keyboard
	virtual bool evKeyPressed(qkey *pKey, bool pDown, EXTCompInfo *pECI); // let us know a key was pressed. Return true if Omnis should not do anything with this keypress

	// drag and drop
	virtual bool canDrag(qpoint pFrom); // Can we drag from this location? Return false if we can't
	virtual qlong evSetDragValue(FLDdragDrop *pDragInfo, EXTCompInfo *pECI); // Set drag value, update the pDragInfo structure with information about what we are dragging, return -1 if we leave it up to Omnis to handle this
	virtual qlong evEndDrag(FLDdragDrop *pDragInfo); // Ended dragging, return -1 if we leave it up to Omnis to handle this
};

#endif
