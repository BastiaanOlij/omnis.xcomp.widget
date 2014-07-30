/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oImage.h
 *  Header for our Non-visual image object
 *
 *  Bastiaan Olij
 */

#ifndef oimageh
#define oimageh

#include "omnis.xcomp.framework.h"
#include "oImgLayer.h"

enum oImagePropIDs {
	oIM_layerCount			= 1,
	oIM_currentlayer		= 2,
	oIM_layertype			= 3,
	oIM_layerleft			= 4,
	oIM_layertop			= 5,
	oIM_layerwidth			= 6,
	oIM_layerheight			= 7,
	oIM_layercontents		= 8,
	oIM_layerenabled		= 9,
	oIM_image				= 10,
	oIM_imageWidth			= 11,
	oIM_imageHeight			= 12,
};

enum oImageMethodIDs {
	oIM_addlayer			= 1,
	oIM_remlayer			= 2,
};

class oImage : public oBaseNVComponent {
private:
	std::vector<oImgLayer *>	mLayers;															// image layers
	qlong						mCurrentLayer;														// current layer we have selected. Note mCurrentLayer = 1 => mLayers[0] !!
	qdim						mImageWidth;														// width of our resulting image
	qdim						mImageHeight;														// height of our resulting image
	
	
	oRGBAImage *				mCachedImage;
	qlong						mCachedWidth;
	qlong						mCachedHeight;
	
	void						clearCachedImg();													// clear our cached image
	void						updateCachedImg(qdim pWidth, qdim pHeight);						// update our cached image
protected:
public:
	oImage(void);
	~oImage(void);
	
	static oImage *				newObject(void);
	
	virtual void				copyObject(oBaseNVComponent *pCopy);								// create a copy of pCopy, this MUST be implemented in a subclass
	
	// properties
	static  qProperties *		properties(void);													// return an array of property data
	virtual qbool				canAssign(qlong pPropID);											// return true/false if a property can be written too
	virtual qbool				setProperty(qlong pPropID,EXTfldval &pNewValue,EXTCompInfo* pECI);	// set the value of a property
	virtual void				getProperty(qlong pPropID,EXTfldval &pGetValue,EXTCompInfo* pECI);	// get the value of a property
	
	// methods
	static  qMethods *			methods(void);														// return array of method meta data
	virtual int					invokeMethod(qlong pMethodId, EXTCompInfo* pECI);					// invoke a method
	
	// events
	static	qEvents *			events(void);														// return an array of events meta data	
};

#endif
