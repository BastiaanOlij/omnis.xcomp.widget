/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oImage.h
 *  Source for our Non-visual image object
 *
 *  Bastiaan Olij
 */


#include "oImage.h"

oImage::oImage(void) {
	// by default we add an empty bitmap layer
	oImgBitmap * bitmap = new oImgBitmap(640,480,GDI_COLOR_QBLUE);
	mLayers.push_back(bitmap);
	mCurrentLayer	= 1;
	mCachedPixmap	= 0;
	mCachedWidth	= 0;
	mCachedHeight	= 0;
	mImageWidth		= 0;
	mImageHeight	= 0;
};

oImage::~oImage(void) {
	clearCachedImg();
	
	while (mLayers.size()>0) {
		oImgLayer *layer = mLayers.back();
		mLayers.pop_back();
		
		delete layer;
	};
};

oImage *	oImage::newObject(void) {
	oImage *lvNewObject = new oImage();
	
	return lvNewObject;
};

// create a copy of pCopy, this MUST be implemented in a subclass
void	oImage::copyObject(oBaseNVComponent *pCopy) {
	// we MUST implement this by deep copying our layers!
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// cached image
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// clear our cached image
void	oImage::clearCachedImg() {
	if (mCachedPixmap!=0) {
		GDIdeleteHPIXMAP(mCachedPixmap);
		mCachedPixmap	= 0;
		mCachedWidth	= 0;
		mCachedHeight	= 0;
	};
};

// update our cached image
void	oImage::updateCachedImg(qdim pWidth, qdim pHeight) {
	if (pWidth == 0) {
		pWidth = mLayers[0]->left() + mLayers[0]->width();
	};
	if (pHeight == 0) {
		pHeight = mLayers[0]->top() + mLayers[0]->height();
	};
	
	// check if our width and height have changed, if so then rebuild our image..
	if ((mCachedWidth!=pWidth) || (mCachedHeight!=pHeight)) {
		// JIC
		clearCachedImg();
		
		if ((pWidth!=0) && (pHeight!=0)) {
			bool	mix = false;
			
			addToTraceLog("Creating cached image: %li, %li", pWidth, pHeight);
			
			mCachedWidth = pWidth;
			mCachedHeight = pHeight;
			mCachedPixmap = GDIcreateHPIXMAP(pWidth, pHeight, 32, false);
			
			sPixel * buffer = (sPixel *) GDIlockHPIXMAP(mCachedPixmap);
			if (buffer != NULL) {
				// fill our map with a default colour
				sPixel col;
				col.mR = 255;
				col.mG = 255;
				col.mB = 255;
				col.mA = 255;
				
				for (qlong offset = 0; offset < pWidth*pHeight; offset++) {
					buffer[offset] = col;
				};
				
				GDIunlockHPIXMAP(mCachedPixmap);
				
				// and apply our layers...
				for	(qlong layer = 0;layer < mLayers.size(); layer++) {
					if (mLayers[layer]->enabled()) {
						mLayers[layer]->drawLayer(mCachedPixmap, mix);
						mix = true;
					};
				};
			} else {
				addToTraceLog("Couldn't lock pixel buffer");
				clearCachedImg();
			};
		};
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ECOproperty oImageProperties[] = { 
	//	ID						ResID	Type			Flags					ExFlags	EnumStart	EnumEnd
	oIM_layerCount,				4300,	fftInteger,		EXTD_FLAG_PROPDATA
														+EXTD_FLAG_RUNTIMEONLY,	0,		0,			0,
	oIM_image,					4309,	fftPicture,		EXTD_FLAG_PROPDATA
														+EXTD_FLAG_RUNTIMEONLY,	0,		0,			0,
	oIM_imageWidth,				4310,	fftInteger,		EXTD_FLAG_PROPDATA
														+EXTD_FLAG_RUNTIMEONLY,	0,		0,			0,
	oIM_imageHeight,			4311,	fftInteger,		EXTD_FLAG_PROPDATA
														+EXTD_FLAG_RUNTIMEONLY,	0,		0,			0,
	oIM_currentlayer,			4301,	fftInteger,		EXTD_FLAG_PROPCUSTOM
														+EXTD_FLAG_RUNTIMEONLY,	0,		0,			0,
	oIM_layerenabled,			4308,	fftBoolean,		EXTD_FLAG_PROPCUSTOM
														+EXTD_FLAG_RUNTIMEONLY,	0,		0,			0,
	oIM_layertype,				4302,	fftCharacter,	EXTD_FLAG_PROPCUSTOM
														+EXTD_FLAG_RUNTIMEONLY,	0,		0,			0,
	oIM_layerleft,				4303,	fftInteger,		EXTD_FLAG_PROPCUSTOM
														+EXTD_FLAG_RUNTIMEONLY,	0,		0,			0,
	oIM_layertop,				4304,	fftInteger,		EXTD_FLAG_PROPCUSTOM
														+EXTD_FLAG_RUNTIMEONLY,	0,		0,			0,
	oIM_layerwidth,				4305,	fftInteger,		EXTD_FLAG_PROPCUSTOM
														+EXTD_FLAG_RUNTIMEONLY,	0,		0,			0,
	oIM_layerheight,			4306,	fftInteger,		EXTD_FLAG_PROPCUSTOM
														+EXTD_FLAG_RUNTIMEONLY,	0,		0,			0,
	oIM_layercontents,			4307,	fftBinary,		EXTD_FLAG_PROPCUSTOM
														+EXTD_FLAG_RUNTIMEONLY,	0,		0,			0,
};	


// return an array of property data
qProperties *	oImage::properties(void) {
	qProperties *	lvProperties = oBaseNVComponent::properties();
	
	// Add the property definition for our object here...
	lvProperties->addElements(oImageProperties, sizeof(oImageProperties) / sizeof(ECOproperty));
	
	return lvProperties;
};

// return true/false if a property can be written too
qbool	oImage::canAssign(qlong pPropID) {
	switch (pPropID) {
		case oIM_layerCount:
			return qfalse;
			break;
		case oIM_image:
			return qfalse;
			break;
		case oIM_layertype:
			// read only for now..
			return qfalse;
			break;
		default:
			return oBaseNVComponent::canAssign(pPropID);
			break;
	};	
};

// set the value of a property
qbool	oImage::setProperty(qlong pPropID,EXTfldval &pNewValue,EXTCompInfo* pECI) {
	switch (pPropID) {
		case oIM_layerCount: {
			// read only
			return qfalse;
		}; break;
		case oIM_image: {
			// read only
			return qfalse;
		}; break;
		case oIM_imageWidth: {
			mImageWidth = pNewValue.getLong();
			clearCachedImg();
			return true;						
		}; break;
		case oIM_imageHeight: {
			mImageHeight = pNewValue.getLong();
			clearCachedImg();
			return true;						
		}; break;
		case oIM_currentlayer: {
			qlong	newlayer = pNewValue.getLong();
			if ((newlayer > 0) && (newlayer <= mLayers.size())) {
				mCurrentLayer = newlayer;
			};
			return true;
		}; break;
		case oIM_layertype: {
			// for now not implemented, we'll do something about this later on...
			return false;
		}; break;
		case oIM_layerenabled: {
			oImgLayer	*layer = mLayers[mCurrentLayer-1];
			layer->setEnabled(pNewValue.getBool()==2);
			clearCachedImg();
			return true;			
		}; break;
		case oIM_layerleft: {
			oImgLayer	*layer = mLayers[mCurrentLayer-1];
			layer->setLeft(pNewValue.getLong());
			clearCachedImg();
			return true;
		}; break;
		case oIM_layertop: {
			oImgLayer	*layer = mLayers[mCurrentLayer-1];
			layer->setTop(pNewValue.getLong());
			clearCachedImg();
			return true;
		}; break;
		case oIM_layerwidth: {
			oImgLayer	*layer = mLayers[mCurrentLayer-1];
			layer->setWidth(pNewValue.getLong());
			clearCachedImg();
			return true;
		}; break;
		case oIM_layerheight: {
			oImgLayer	*layer = mLayers[mCurrentLayer-1];
			layer->setHeight(pNewValue.getLong());
			clearCachedImg();
			return true;
		}; break;
		case oIM_layercontents: {
			oImgLayer	*layer = mLayers[mCurrentLayer-1];
			layer->setContents(pNewValue);
			clearCachedImg();
			return true;			
		}; break;
		default:
			return oBaseNVComponent::setProperty(pPropID, pNewValue, pECI);
			break;
	};
			
};

// get the value of a property
void	oImage::getProperty(qlong pPropID,EXTfldval &pGetValue,EXTCompInfo* pECI) {
	switch (pPropID) {
		case oIM_layerCount: {
			pGetValue.setLong(mLayers.size());
		}; break;
		case oIM_image: {
			// create our image based on the size of our first layer
			updateCachedImg(mImageWidth, mImageHeight);
			
			if (mCachedPixmap == 0) {
				pGetValue.setEmpty(fftPicture, 0);
			} else {
				GDIbitmapToColorShared(mCachedPixmap, pGetValue);
			};
		}; break;
		case oIM_imageWidth: {
			pGetValue.setLong(mImageWidth);
		}; break;
		case oIM_imageHeight: {
			pGetValue.setLong(mImageHeight);
		}; break;
		case oIM_currentlayer: {
			pGetValue.setLong(mCurrentLayer);
		}; break;
		case oIM_layertype: {
			oImgLayer	*layer = mLayers[mCurrentLayer-1];
			qstring		layertype (layer->layerType());
			pGetValue.setChar((qchar *)layertype.cString(), layertype.length());								
		}; break;
		case oIM_layerenabled: {
			oImgLayer	*layer = mLayers[mCurrentLayer-1];
			pGetValue.setBool(layer->enabled() ? 2 : 1);
		}; break;			
		case oIM_layerleft: {
			oImgLayer	*layer = mLayers[mCurrentLayer-1];
			pGetValue.setLong(layer->left());
		}; break;
		case oIM_layertop: {
			oImgLayer	*layer = mLayers[mCurrentLayer-1];
			pGetValue.setLong(layer->top());
		}; break;
		case oIM_layerwidth: {
			oImgLayer	*layer = mLayers[mCurrentLayer-1];
			pGetValue.setLong(layer->width());
		}; break;
		case oIM_layerheight: {
			oImgLayer	*layer = mLayers[mCurrentLayer-1];
			pGetValue.setLong(layer->height());
		}; break;
		case oIM_layercontents: {
			oImgLayer	*layer = mLayers[mCurrentLayer-1];
			layer->getContents(pGetValue);
		}; break;
		default:
			return oBaseNVComponent::getProperty(pPropID, pGetValue, pECI);
			break;
	};	
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// parameters for our callback function
ECOparam remLayerParams[] = {
	//	Resource	Type			Flags				ExFlags
	7100,			fftInteger,		0,					0,		// layer
};

// This is our array of methods we support
ECOmethodEvent oImageMethods[] = {
	//	ID				Resource	Return type		Paramcount		Params					Flags		ExFlags
	oIM_addlayer,		8100,		fftNone,		0,				NULL,					0,			0,			// $addLayer	
	oIM_remlayer,		8101,		fftNone,		1,				remLayerParams,			0,			0,			// $remLayer	
};

// return array of method meta data
qMethods *	oImage::methods(void) {
	qMethods * lvMethods = oBaseNVComponent::methods();
	
	lvMethods->addElements(oImageMethods, sizeof(oImageMethods) / sizeof(ECOmethodEvent));		
	
	return lvMethods;	
};

// invoke a method
int	oImage::invokeMethod(qlong pMethodId, EXTCompInfo* pECI) {
	switch (pMethodId) {
		case oIM_addlayer: { // add layer
			// For now we add an empty bitmap layer, that is the only thing we support. 
			// Eventually we will be able to change the layer type which simply means we destroy this object and replace it with a new only
			// At that time we'll enhance this method to optionally specialy the correct type to insert right away.
			
			oImgBitmap * bitmap = new oImgBitmap(640,480,GDI_COLOR_QWHITE);
			mLayers.push_back(bitmap);
			mCurrentLayer = mLayers.size();
			
			clearCachedImg();

			return 1L;							
		}; break;
		case oIM_remlayer: { // remove layer
			EXTParamInfo*		param1 = ECOfindParamNum( pECI, 1 );
			EXTfldval			tmpData((qfldval) param1->mData);
			int					tmpLayerNo = tmpData.getLong();
			
			// need to implement this and prevent deleting the last layer...
			
			return 1L;							
		}; break;
		default: {
			return oBaseNVComponent::invokeMethod(pMethodId, pECI);
		}; break;
	}	
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// events
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// return an array of events meta data
qEvents *	oImage::events(void) {
	qEvents *	lvEvents = oBaseNVComponent::events();
	
	// nothing to add yet, I'm not even sure these work for non-visual components
	
	return lvEvents;
};