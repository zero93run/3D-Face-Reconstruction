#ifndef _CASMUTIL_H
#define _CASMUTIL_H
#include <asmfitting.h>
#include <cv.h>
#include <highgui.h>
#include <vector>

using namespace std;

class CASMUTIL
{
public:
	CASMUTIL(char* model_add,char *cascade_add);
	~CASMUTIL();
	bool changeModel(char* model_add);
	bool fit(char* image_add,bool show=false);
	vector<float> getPointList();
	short* getPointIndex();
	int getNumPrim();
	int getTextureWidth();
	int getTextureHeight();
private:
	bool detect_all_faces(asm_shape** Shapes, int& n_shapes, const IplImage* image);
	bool detect_one_face(asm_shape& Shape, const IplImage* image);
	void free_shape_memeory(asm_shape** shapes);
	void drawResult(IplImage* image, const asm_shape& shape);
private:
	const static float ZHeight[68];
private:
	asmfitting m_fit;
	IplImage* img;
	bool flag;
	asm_shape shape, detshape;
	CvHaarClassifierCascade* m_cascade;
	CvMemStorage* m_storage;
	int width;
	int height;
	int numPrim;
};

#endif