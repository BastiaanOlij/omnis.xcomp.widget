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
	mTouched = true;
	mExpanded = true;
	mLineNo = 0;
	mValue = QTEXT("");
	mDescription = QTEXT("");
	mTop = 0;
	mBottom = 0;
	mSortOrder = 0;
	mListLineNo = -1;
};

oDLNode::oDLNode(const qstring &pValue, const qstring &pDescription, qlong pLineNo) {
	mTouched = true;
	mExpanded = true;
	mLineNo = pLineNo;
	mValue = pValue;
	mDescription = pDescription;
	mTop = 0;
	mBottom = 0;
	mSortOrder = 0;
	mListLineNo = -1;
};

oDLNode::~oDLNode(void) {
	clearChildNodes();
};

////////////////////////////////////////////////
// properties
////////////////////////////////////////////////

// touched?
bool oDLNode::touched(void) {
	return mTouched;
};

// set value of touched
void oDLNode::setTouched(bool pTouched) {
	mTouched = pTouched;
};

// expanded?
bool oDLNode::expanded(void) {
	return mExpanded;
};

// set expanded
void oDLNode::setExpanded(bool pExpanded) {
	mExpanded = pExpanded;
};

// related line number
qlong oDLNode::lineNo(void) {
	return mLineNo;
};

// update the related line number
void oDLNode::setLineNo(qlong pLineNo) {
	mLineNo = pLineNo;
};

// our sort order
qlong oDLNode::sortOrder(void) {
	return mSortOrder;
};

// set our sort order
void oDLNode::setSortOrder(qlong pSortOrder) {
	mSortOrder = pSortOrder;
};

////////////////////////////////////////////////
// for our drawing
////////////////////////////////////////////////

// drawn as visible line no
qlong oDLNode::listLineNo(void) {
	return mListLineNo;
};

// set visible line no
void oDLNode::setListLineNo(qlong pLineNo) {
	mListLineNo = pLineNo;
};

// top of our node
qdim oDLNode::top(void) {
	return mTop;
};

// set the top of our node
void oDLNode::setTop(qdim pTop) {
	mTop = pTop;
};

// bottom of our node
qdim oDLNode::bottom(void) {
	return mBottom;
};

// set bottom of our node
void oDLNode::setBottom(qdim pBottom) {
	mBottom = pBottom;
};

// our tree icon rectangle
qrect oDLNode::treeIconRect(void) {
	return mTreeIconRect;
};

// set our tree icon rectangle
void oDLNode::setTreeIconRect(qrect pRect) {
	mTreeIconRect = pRect;
};

////////////////////////////////////////////////
// read only info
////////////////////////////////////////////////

// value
const qstring &oDLNode::value(void) {
	return mValue;
};

// description
const qstring &oDLNode::description(void) {
	return mDescription;
};

// is this point within our tree icon?
bool oDLNode::aboveTreeIcon(qpoint pAt) {
	if (mChildNodes.size() == 0) {
		return false;
	} else {
		return ((mTreeIconRect.left <= pAt.h) && (mTreeIconRect.right >= pAt.h) && (mTreeIconRect.top <= pAt.v) && (mTreeIconRect.bottom >= pAt.v));
	};
};

////////////////////////////////////////////////
// methods
////////////////////////////////////////////////

// Clear all the child nodes
void oDLNode::clearChildNodes() {
	// free all the children!
	while (mChildNodes.size() > 0) {
		oDLNode *child = mChildNodes.back(); // get the last on our stack
		mChildNodes.pop_back(); // and remove it from our stack

		delete child;
	};
};

// Add this node
void oDLNode::addNode(oDLNode *pNewNode) {
	mChildNodes.push_back(pNewNode);
};

// Returns the number of child nodes
unsigned long oDLNode::childNodeCount() {
	return mChildNodes.size();
};

// Find a child node by value
oDLNode *oDLNode::findChildByValue(const qstring &pValue) {
	for (unsigned long index = 0; index < mChildNodes.size(); index++) {
		oDLNode *child = mChildNodes[index];

		if (pValue == child->value()) {
			return child;
		};
	};
	return NULL;
};

// Find a child node by description
oDLNode *oDLNode::findChildByDescription(const qstring &pDesc, bool pNoValue) {
	for (unsigned long index = 0; index < mChildNodes.size(); index++) {
		oDLNode *child = mChildNodes[index];

		if ((pDesc == child->description()) && (!pNoValue || (child->value().length() == 0))) {
			return child;
		};
	};
	return NULL;
};

// Find a child node by description
oDLNode *oDLNode::findChildByLineNo(qlong pLineNo, bool pNoValue) {
	for (unsigned long index = 0; index < mChildNodes.size(); index++) {
		oDLNode *child = mChildNodes[index];

		if ((child->lineNo() == pLineNo) && (!pNoValue || (child->value().length() == 0))) {
			return child;
		};
	};
	return NULL;
};

// Find a child node by screen location
oDLNode *oDLNode::findChildByPoint(qpoint pAt) {
	for (unsigned long index = 0; index < mChildNodes.size(); index++) {
		oDLNode *child = mChildNodes[index];
		if ((child->mTop <= pAt.v) && (child->mBottom > pAt.v)) {
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
oDLNode *oDLNode::getChildByIndex(unsigned long pIndex) {
	if (pIndex < mChildNodes.size()) {
		return mChildNodes[pIndex];
	} else {
		return NULL;
	};
};

// Get our list line no for a line in our source list
qlong oDLNode::findListLineNo(qlong pLineNo) {
	if (mLineNo == pLineNo) {
		return mListLineNo;
	};

	if (mExpanded) {
		// check our child nodes..
		for (unsigned long index = 0; index < mChildNodes.size(); index++) {
			oDLNode *child = mChildNodes[index];
			qlong listLineNo = child->findListLineNo(pLineNo);
			if (listLineNo >= 0) {
				// found it!
				return listLineNo;
			};
		};
	} else {
		// not expanded? then we're not showing anything..
	};

	return -1;
};

// Find top position for specific line (will check child nodes if applicable)
qdim oDLNode::findTopForLine(qlong pLineNo) {
	if (mLineNo == pLineNo) {
		return mTop;
	};

	if (mExpanded) {
		// check our child nodes..
		for (unsigned long index = 0; index < mChildNodes.size(); index++) {
			oDLNode *child = mChildNodes[index];
			qdim top = child->findTopForLine(pLineNo);
			if (top >= 0) {
				// found it!
				return top;
			};
		};
	} else {
		// not expanded? then we're not showing anything..
	};

	return -1; // our positions start at the top..
};

// Static function that returns whether true if the sort order of A is smaller then B
bool oDLNode::order(oDLNode *pA, oDLNode *pB) {
	if (pB == NULL) {
		return false;
	} else if (pA == NULL) {
		return true;
	};

	return pA->mSortOrder < pB->mSortOrder;
};

// Sort our child nodes
void oDLNode::sortChildren(void) {
	// sort our children
	std::sort(mChildNodes.begin(), mChildNodes.end(), oDLNode::order);

	// now tell our children to also sort
	for (unsigned long index = 0; index < mChildNodes.size(); index++) {
		oDLNode *child = mChildNodes[index];

		child->sortChildren();
	};
};

// Marks all children as untouched and remove any line nodes
void oDLNode::unTouchChildren(void) {
	for (unsigned long index = 0; index < mChildNodes.size(); index++) {
		oDLNode *child = mChildNodes[index];
		child->setTouched(false);
		child->unTouchChildren();
	};

	mListLineNo = -1; // reset this

	// if this is a node created by our grouping, clear our related line and sort
	if (mValue.length() != 0 || mDescription.length() != 0) {
		mLineNo = 0;
		mSortOrder = 0;
	};
};

// Removes children that are untouched
void oDLNode::removeUntouched(void) {
	unsigned long index = 0;

	while (index < mChildNodes.size()) {
		oDLNode *child = mChildNodes[index];
		if (child->touched()) {
			child->removeUntouched();
			index++;
		} else {
			mChildNodes.erase(mChildNodes.begin() + index);
			delete child;
		};
	};
};
