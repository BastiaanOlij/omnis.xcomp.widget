/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oDLNode.h
 *  Header for our datalist node object
 *
 *  oDLNode is a support object for building our tree node
 *
 *  Bastiaan Olij
 */

#ifndef odlnodeh
#define odlnodeh

#include "omnis.xcomp.framework.h"

class oDLNode {
private:
	bool mTouched; // if true this node is still part of our data list
	bool mExpanded; // if true our node is expanded
	qlong mLineNo; // line number in our data list this relates to, 0 if just grouping
	qlong mSortOrder; // numeric field to sort our node by
	qstring mValue; // our value, used to find related records
	qstring mDescription; // our description, only used if mLineNo=0

	// These are only set after we've drawn our tree so we can see where nodes are
	qdim mTop; // top position
	qdim mBottom; // bottom position
	qrect mTreeIconRect; // Rectangle for our tree icon
	qlong mListLineNo; // this is drawn as line number....

	// nodes
	std::vector<oDLNode *> mChildNodes; // child nodes

public:
	oDLNode(void);
	oDLNode(const qstring &pValue, const qstring &pDescription, qlong pLineNo = 0);
	~oDLNode(void);

	// properties
	bool touched(void); // touched?
	void setTouched(bool pTouched); // set value of touched
	bool expanded(void); // expanded?
	void setExpanded(bool pExpanded); // set expanded
	qlong lineNo(void); // related line number
	void setLineNo(qlong pLineNo); // update the related line number
	qlong sortOrder(void); // our sort order
	void setSortOrder(qlong pSortOrder); // set our sort order

	// for our drawing
	qlong listLineNo(void); // drawn as line no
	void setListLineNo(qlong pLineNo); // set draws as line no
	qdim top(void); // top of our node
	void setTop(qdim pTop); // set the top of our node
	qdim bottom(void); // bottom of our node
	void setBottom(qdim pBottom); // set bottom of our node
	qrect treeIconRect(void); // our tree icon rectangle
	void setTreeIconRect(qrect pRect); // set our tree icon rectangle

	// read only
	const qstring &value(void); // value
	const qstring &description(void); // description
	bool aboveTreeIcon(qpoint pAt); // is this point within our tree icon?

	// methods
	void clearChildNodes(); // Clear all the child nodes
	void addNode(oDLNode *pNewNode); // Add this node
	unsigned long childNodeCount(); // Returns the number of child nodes
	oDLNode *findChildByValue(const qstring &pValue); // Find a child node by value
	oDLNode *findChildByDescription(const qstring &pDesc, bool pNoValue = false); // Find a child node by description
	oDLNode *findChildByLineNo(qlong pLineNo, bool pNoValue = false); // Find a child node by line number
	oDLNode *findChildByPoint(qpoint pAt); // Find a child node by screen location
	oDLNode *getChildByIndex(unsigned long pIndex); // Get child at specific index

	qlong findListLineNo(qlong pLineNo); // Get our list line no for a line in our source list
	qdim findTopForLine(qlong pLineNo); // Find top position for specific line (will check child nodes if applicable)

	static bool order(oDLNode *pA, oDLNode *pB); // Static function that returns whether true if the sort order of A is smaller then B
	void sortChildren(void); // Sort our child nodes
	void unTouchChildren(void); // Marks all children as untouched
	void removeUntouched(void); // Removes children that are untouched
};

#endif
