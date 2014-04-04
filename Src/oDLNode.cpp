/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oDLNode.cpp
 *  Implementation of our datalist node object
 *
 *  Bastiaan Olij
 */

#include "oDLNode.h"

oDLNode::oDLNode(void) {
	mTouched		= true;
	mExpanded		= true;
	mLineNo			= 0;
	mDescription	= QTEXT("");
	mTop			= 0;
	mBottom			= 0;
};

oDLNode::oDLNode(qstring &pDescription, qlong pLineNo) {
	mTouched		= true;
	mExpanded		= true;
	mLineNo			= pLineNo;
	mDescription	= pDescription;
	mTop			= 0;
	mBottom			= 0;	
};

oDLNode::~oDLNode(void) {
	clearChildNodes();
};

////////////////////////////////////////////////
// properties
////////////////////////////////////////////////

// touched?
bool	oDLNode::touched(void) {
	return mTouched;
};

// set value of touched
void	oDLNode::setTouched(bool pTouched) {
	mTouched = pTouched;
};

// expanded?
bool	oDLNode::expanded(void) {
	return mExpanded;
};

// set expanded
void	oDLNode::setExpanded(bool pExpanded) {
	mExpanded = pExpanded;
};

// related line number
qlong	oDLNode::lineNo(void) {
	return mLineNo;
};

// description
const qchar *	oDLNode::description(void) {
	return mDescription.cString();
};

// is this point within our tree icon?
bool	oDLNode::aboveTreeIcon(qpoint pAt) {
	return ((mTreeIcon.left <= pAt.h) && (mTreeIcon.right >= pAt.h) && (mTreeIcon.top <= pAt.v) && (mTreeIcon.bottom >= pAt.v));
};


////////////////////////////////////////////////
// methods
////////////////////////////////////////////////

// Clear all the child nodes
void	oDLNode::clearChildNodes() {
	// free all the children!
	while (mChildNodes.numberOfElements()>0) {
		oDLNode *child = mChildNodes.pop();
		delete child;
	};	
};

// Add this node
void	oDLNode::addNode(oDLNode * pNewNode) {
	mChildNodes.push(pNewNode);
};

// Returns the number of child nodes
unsigned long	oDLNode::childNodeCount() {
	return mChildNodes.numberOfElements();
};

// Find a child node by description
oDLNode	*	oDLNode::findChildByDescription(qstring & pDescription) {
	for (unsigned long index = 0; index < mChildNodes.numberOfElements(); index++) {
		oDLNode *child = mChildNodes[index];
	
		if (pDescription == child->description()) {
			return child;
		};
	};
	return NULL;
};

// Find a child node by screen location
oDLNode *	oDLNode::findChildByPoint(qpoint pAt) {
	for (unsigned long index = 0; index < mChildNodes.numberOfElements(); index++) {
		oDLNode *child = mChildNodes[index];
		if ((child->mTop <= pAt.v) && (child->mBottom >= pAt.v)) {
			// Our point lies within this child, see if it lies within one of its children
			oDLNode *subchild = child->findChildByPoint(pAt);
			if (subchild != NULL) {
				return subchild;
			};
			
			return child;
		};
	};
	
	return NULL;
};


// Get child at specific index
oDLNode *	oDLNode::getChildByIndex(unsigned long pIndex) {
	if (pIndex < mChildNodes.numberOfElements()) {
		return mChildNodes[pIndex];
	} else {
		return NULL;
	};
};

// Add a row
void	oDLNode::addRow(sDLRow pRow) {
	mRowNodes.push(pRow);
};

// Returns the number of lrows
unsigned long	oDLNode::rowCount() {
	return mRowNodes.numberOfElements();
};

// Get the row at this index
sDLRow	oDLNode::getRowAtIndex(unsigned long pIndex) {
	if (pIndex < mRowNodes.numberOfElements()) {
		return mRowNodes[pIndex];
	} else {
		sDLRow	lvEmpty;
		
		lvEmpty.mLineNo = 0;
		lvEmpty.mTop = 0;
		lvEmpty.mBottom = 0;
		
		return lvEmpty;
	};
};

// Set the row at this index
void	oDLNode::setRowAtIndex(unsigned long pIndex, sDLRow pRow) {
	if (pIndex < mRowNodes.numberOfElements()) {
		mRowNodes.setElementAtIndex(pIndex, pRow);
	};		
};

// Find row by screen location
qlong	oDLNode::findRowAtPoint(qpoint pAt) {
	for (unsigned int i = 0; i < mRowNodes.numberOfElements(); i++) {
		sDLRow row = mRowNodes[i];
		
		if ((row.mTop <= pAt.v) && (row.mBottom >= pAt.v)) {
			return row.mLineNo;
		};
	};
	
	return 0;
};

// Check if this row is part of our node?
bool	oDLNode::hasRow(qlong pLineNo) {
	for (unsigned int i = 0; i < mRowNodes.numberOfElements(); i++) {
		sDLRow row = mRowNodes[i];
		
		if (row.mLineNo == pLineNo) {
			return true;
		};
	};
	
	return false;
};

// Marks all children as untouched and remove any line nodes
void	oDLNode::unTouchChildren(void) {
	for (unsigned long index = 0; index < mChildNodes.numberOfElements(); index++) {
		oDLNode *child = mChildNodes[index];
		child->setTouched(false);
		child->unTouchChildren();
	};
	
	mRowNodes.clear();
};

// Removes children that are untouched
void	oDLNode::removeUntouched(void) {
	unsigned long index = 0;
	
	while (index < mChildNodes.numberOfElements()) {
		oDLNode * child = mChildNodes[index];
		if (child->touched()) {
			child->removeUntouched();
			index++;
		} else {
			mChildNodes.remElementAtIndex(index);
			delete child;
		};
	};
};
