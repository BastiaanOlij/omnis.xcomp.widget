/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oImgLayer.h
 *  Header for our image layer objects
 *
 *  This object represents a single image layer for our image system
 *  The baseclass image layer defines:
 *  - offset information for this layer
 *  - sizing information for our layer
 *  - an alpha value to apply to the image (or multiplied with the alpha in the image)
 *
 *  We have a bitmap subclass that defines a layer based on an uncompressed bitmap contained within
 *
 *  Bastiaan Olij
 */

#ifndef oimglayerh
#define oimglayerh

#include "omnis.xcomp.framework.h"

class oImgLayer {
private:
protected:
	bool		mEnabled;
	qlong		mLeft;
	qlong		mTop;
	qlong		mWidth;
	qlong		mHeight;
	qbyte		mAlpha;

	sPixel		mixPixel(sPixel pBase, sPixel pAdd, qbyte pAlpha);
	
public:
	oImgLayer(void);
	virtual ~oImgLayer(void);
	
	virtual	const char *	layerType();							// type of layer this is, just for feedback to the developer
	
	virtual		bool		enabled();
	virtual		void		setEnabled(bool pEnabled);
	virtual		qlong		left();
	virtual		void		setLeft(qlong pNewValue);
	virtual		qlong		top();
	virtual		void		setTop(qlong pNewValue);
	virtual		qlong		width();
	virtual		void		setWidth(qlong pNewValue);
	virtual		qlong		height();
	virtual		void		setHeight(qlong pNewValue);	
	
	virtual		void		getContents(EXTfldval &pContents);
	virtual		void		setContents(EXTfldval &pContents);
	
	virtual		void		drawLayer(HPIXMAP pOnto, bool pMix);	// draw our layer onto the pixmap..
};

class oImgBitmap : public oImgLayer {
private:
	qlong		mImgWidth;
	qlong		mImgHeight;
	HPIXMAP		mPixmap;
	
	sPixel		interpolatePixel(sPixel pA, sPixel pB, float pFract);
	sPixel		getPixel(sPixel * pData, float pX, float pY);		// get interpolated pixel

protected:
public:
	oImgBitmap(qlong pWidth, qlong pHeight, qcol pColor);			// init as empty bitmap
	virtual ~oImgBitmap(void);										// and cleanup!

	virtual	const char *	layerType();							// type of layer this is, just for feedback to the developer

	virtual		qlong		width();
	virtual		void		setWidth(qlong pNewValue);
	virtual		qlong		height();
	virtual		void		setHeight(qlong pNewValue);

	virtual		void		getContents(EXTfldval &pContents);
	virtual		void		setContents(EXTfldval &pContents);

	virtual		void		drawLayer(HPIXMAP pOnto, bool pMix);	// draw our layer onto the pixmap..
};


#endif
