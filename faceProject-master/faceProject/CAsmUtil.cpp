#include "CAsmUtil.h"

const float CASMUTIL::ZHeight[68]={
	6,0,0,4,		//0~3
	2,2,0,0,	//4~7
	0,2,2,4,	//8~11
	0,0,6,0,		//12~15
	-2,-2,-4,-2,		//16~19
	-2,0,-2,-2,		//20~23
	-4,-2,-2,4,		//24~27
	0,0,0,-1,		//28~31
	4,0,0,0,		//32~35
	-1,-4,0,0,		//36~39
	0,0,0,0,		//40~43
	0,-2,-6,-6,		//44~47
	2,-1,-2,-2,		//48~51
	-2,-1,2,-1,		//52~55
	-2,-2,-2,-1,		//56~59
	0,0,0,0,		//60~63
	0,0,0,-10			//64~67
};

bool CASMUTIL::detect_all_faces(asm_shape** Shapes, int& n_shapes, const IplImage* image)
{
	IplImage* pWork = cvCreateImage
		(cvSize(image->width/2, image->height/2), image->depth, image->nChannels);
	cvPyrDown(image, pWork, CV_GAUSSIAN_5x5 );

	CvSeq* pFaces = cvHaarDetectObjects(pWork, m_cascade, m_storage,
		1.1, 3, CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30));

	cvReleaseImage(&pWork);

	n_shapes = 0;
	if(0 == pFaces->total)//can't find a face
		return false;

	*Shapes = new asm_shape[pFaces->total];
	n_shapes = pFaces->total;
	for (int iFace = 0; iFace < pFaces->total; iFace++)
	{
		(*Shapes)[iFace].Resize(2);
		CvRect* r = (CvRect*)cvGetSeqElem(pFaces, iFace);

		CvPoint pt1, pt2;
		pt1.x = r->x * 2;
		pt2.x = (r->x + r->width) * 2;
		pt1.y = r->y * 2;
		pt2.y = (r->y + r->height) * 2;

		(*Shapes)[iFace][0].x  = r->x*2.0;
		(*Shapes)[iFace][0].y  = r->y*2.0;
		(*Shapes)[iFace][1].x  = (*Shapes)[iFace][0].x + 2.0*r->width;
		(*Shapes)[iFace][1].y  = (*Shapes)[iFace][0].y + 2.0*r->height;
	}
	return true;
}

void CASMUTIL::free_shape_memeory(asm_shape** shapes)
{
	delete [](*shapes);
}


bool CASMUTIL::detect_one_face(asm_shape& Shape, const IplImage* image)
{
	asm_shape* detShape;
	int nfaces;

	bool flag = detect_all_faces(&detShape, nfaces, image);

	if(!flag) return false;

	// get most central face
	int iSelectedFace = 0;
	double x0 = image->width/2., y0 = image->height/2.;
	double x, y, d, D = 1e307;
	// max abs dist from center of face to center of image
	for (int i = 0; i < nfaces; i++)
	{
		x = (detShape[i][0].x + detShape[i][1].x) / 2.;
		y = (detShape[i][0].y + detShape[i][1].y) / 2.;
		d = sqrt((x-x0)*(x-x0) + (y-y0)*(y-y0)); 
		if (d < D)
		{
			D = d;
			iSelectedFace = i;
		}
	}
	Shape = detShape[iSelectedFace];
	delete []detShape;

	return true;
}


//读入模型和harr检测器
CASMUTIL::CASMUTIL(char* model_add,char *cascade_add){
	if(m_fit.Read(model_add)==false)
		printf("unable to load model\n");
	m_cascade=NULL;
	m_storage=NULL;
	try{
		m_cascade=(CvHaarClassifierCascade*)cvLoad(cascade_add,0,0,0);
		m_storage=cvCreateMemStorage(0);
	}
	catch(int error_code){
		printf("unable to load haar cascade,error_code %d\n",error_code);
	}
	img=0;
	getPointIndex();
}

CASMUTIL::~CASMUTIL(){
	cvReleaseMemStorage(&m_storage);
	cvReleaseHaarClassifierCascade(&m_cascade);
	cvReleaseImage(&img);
}

bool CASMUTIL::changeModel(char* model_add){
	if(m_fit.Read(model_add)==false){
		printf("unable to load model\n");
		return false;
	}
	return true;
}

//检测人脸，人脸对准
bool CASMUTIL::fit(char* image_add,bool show){
	img=cvLoadImage(image_add);
	width=img->width;
	height=img->height;
	if(img==0){
		printf("Can't find image\n");
		return false;
	}
	flag = detect_one_face(detshape, img);	//检测人脸
	if(flag) 
	{	
		InitShapeFromDetBox(shape, detshape, m_fit.GetMappingDetShape(), m_fit.GetMeanFaceWidth());
	}
	m_fit.Fitting2(&shape, 1, img,20);	//人脸对准
	if(show){
		drawResult(img,shape);
		cvShowImage("result",img);
	}
	return true;
}

void CASMUTIL::drawResult(IplImage* image, const asm_shape& shape)
{
	CvFont font;
	cvInitFont( &font, CV_FONT_VECTOR0,0.3, 0.3, 0, 1, 8);
	for(int j = 0; j < shape.NPoints(); j++){
		cvCircle(image, cvPoint(shape[j].x, shape[j].y), 2, CV_RGB(255, 0, 0));
		//char text[10];
		//_itoa(j,text,10);
		//cvPutText(img,text,cvPoint(shape[j].x,shape[j].y),&font, CV_RGB(255, 0, 0));
	}
}

vector<float> CASMUTIL::getPointList(){
	vector<float> ret;
	ret.clear();
	float Z_mul=(shape[14].x-shape[0].x)/60;
	for(int j = 0; j < shape.NPoints(); j++){
		ret.push_back(shape[j].x-width/2);
		ret.push_back(height/2-shape[j].y);
		ret.push_back(ZHeight[j]*Z_mul);
	}
	return ret;
}

short* CASMUTIL::getPointIndex(){
	static short poi[]={
		21,22,26,
		22,23,26,
		26,23,25,
		23,24,25,
		18,17,19,
		17,16,19,
		19,16,20,
		16,15,20,
		0,21,27,
		21,26,27,
		27,26,28,
		26,25,28,
		28,25,29,
		25,24,29,
		24,37,29,
		24,18,37,
		37,18,45,
		23,17,24,
		24,17,18,
		18,34,45,
		18,19,34,
		19,33,34,
		19,20,33,
		20,32,33,
		20,15,32,
		15,14,32,
		27,28,31,
		28,29,31,
		27,31,30,
		31,29,30,
		34,33,36,
		33,32,36,
		34,36,35,
		36,32,35,
		0,27,1,
		1,27,30,
		1,30,38,
		30,29,38,
		29,37,38,
		37,67,38,
		37,45,67,
		45,44,67,
		45,34,44,
		34,35,44,
		44,35,13,
		35,32,13,
		32,14,13,
		1,38,39,
		38,67,39,
		67,44,43,
		44,13,43,
		1,39,2,
		2,39,40,
		39,46,40,
		39,67,46,
		40,46,41,
		46,67,47,
		46,47,41,
		41,47,42,
		67,43,47,
		47,43,42,
		43,12,42,
		43,13,12,
		2,49,48,
		2,40,49,
		40,50,49,
		40,41,50,
		50,41,51,
		41,52,51,
		41,42,52,
		52,42,53,
		42,12,53,
		53,12,54,
		2,48,3,
		48,49,65,
		49,50,65,
		50,64,65,
		50,51,64,
		51,52,64,
		64,52,63,
		52,53,63,
		53,54,63,
		54,12,11,
		48,65,60,
		65,64,66,
		65,66,60,
		60,66,61,
		64,63,66,
		66,63,62,
		66,62,61,
		63,54,62,
		48,60,59,
		59,60,58,
		60,57,58,
		60,61,57,
		61,56,57,
		61,62,56,
		62,55,56,
		62,54,55,
		3,48,4,
		4,48,5,
		5,48,59,
		5,59,6,
		6,59,58,
		6,58,7,
		58,57,7,
		7,57,8,
		57,56,8,
		56,55,8,
		8,55,9,
		55,54,9,
		9,54,10,
		10,54,11
	};
	numPrim=sizeof(poi)/3/sizeof(short);
	return poi;
}

int CASMUTIL::getNumPrim(){
	return numPrim;
}

int CASMUTIL::getTextureWidth(){
	return width;
}

int CASMUTIL::getTextureHeight(){
	return height;
}