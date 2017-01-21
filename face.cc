/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_face.h"

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <dirent.h>
#include <algorithm>
#include <vector>

using namespace cv;
using namespace std;

/* If you declare any globals in php_face.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(face)
*/

/* True global resources - no need for thread safety here */
static int le_face;

/* {{{ face_functions[]
 *
 * Every user visible function must have an entry in face_functions[].
 */
const zend_function_entry face_functions[] = {
	PHP_FE(confirm_face_compiled,	NULL)		/* For testing, remove later. */
    PHP_FE(getAverageFace,	NULL)
	PHP_FE_END	/* Must be the last line in face_functions[] */
};
/* }}} */

/* {{{ face_module_entry
 */
zend_module_entry face_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"face",
	face_functions,
	PHP_MINIT(face),
	PHP_MSHUTDOWN(face),
	PHP_RINIT(face),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(face),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(face),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_FACE_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_FACE
ZEND_GET_MODULE(face)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("face.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_face_globals, face_globals)
    STD_PHP_INI_ENTRY("face.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_face_globals, face_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_face_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_face_init_globals(zend_face_globals *face_globals)
{
	face_globals->global_value = 0;
	face_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(face)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(face)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(face)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(face)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(face)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "face support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_face_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_face_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "face", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/
int getAverageFace(string imageNameStr, string allPointStr, string resultPath);
PHP_FUNCTION(getAverageFace)
{
    char *imageNames = NULL;
    char *allPoints = NULL;
    char *resultPath = NULL;
    int argc = ZEND_NUM_ARGS();
    int imageNames_len;
    int allPoints_len;
    int resultPath_len;
    
    if (zend_parse_parameters(argc TSRMLS_CC, "sss", &imageNames, &imageNames_len, &allPoints, &allPoints_len, &resultPath, &resultPath_len) == FAILURE)
        return;
    
    getAverageFace(string(imageNames),string(allPoints),string(resultPath));
    // php_error(E_WARNING, "startFunc: not yet implemented");
    RETURN_TRUE;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */


// Compute similarity transform given two pairs of corresponding points.
// OpenCV requires 3 pairs of corresponding points.
// We are faking the third one.
void similarityTransform(std::vector<cv::Point2f>& inPoints, std::vector<cv::Point2f>& outPoints, cv::Mat &tform)
{
    
    double s60 = sin(60 * M_PI / 180.0);
    double c60 = cos(60 * M_PI / 180.0);
    
    vector <Point2f> inPts = inPoints;
    vector <Point2f> outPts = outPoints;
    
    inPts.push_back(cv::Point2f(0,0));
    outPts.push_back(cv::Point2f(0,0));
    
    
    inPts[2].x =  c60 * (inPts[0].x - inPts[1].x) - s60 * (inPts[0].y - inPts[1].y) + inPts[1].x;
    inPts[2].y =  s60 * (inPts[0].x - inPts[1].x) + c60 * (inPts[0].y - inPts[1].y) + inPts[1].y;
    
    outPts[2].x =  c60 * (outPts[0].x - outPts[1].x) - s60 * (outPts[0].y - outPts[1].y) + outPts[1].x;
    outPts[2].y =  s60 * (outPts[0].x - outPts[1].x) + c60 * (outPts[0].y - outPts[1].y) + outPts[1].y;
    
    
    tform = cv::estimateRigidTransform(inPts, outPts, false);
}


// Calculate Delaunay triangles for set of points
// Returns the vector of indices of 3 points for each triangle
static void calculateDelaunayTriangles(Rect rect, vector<Point2f> &points, vector< vector<int> > &delaunayTri){
    
    // Create an instance of Subdiv2D
    Subdiv2D subdiv(rect);
    
    // Insert points into subdiv
    for( vector<Point2f>::iterator it = points.begin(); it != points.end(); it++)
        subdiv.insert(*it);
    
    vector<Vec6f> triangleList;
    subdiv.getTriangleList(triangleList);
    vector<Point2f> pt(3);
    vector<int> ind(3);
    
    for( size_t i = 0; i < triangleList.size(); i++ )
    {
        Vec6f t = triangleList[i];
        pt[0] = Point2f(t[0], t[1]);
        pt[1] = Point2f(t[2], t[3]);
        pt[2] = Point2f(t[4], t[5 ]);
        
        if ( rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2])){
            for(int j = 0; j < 3; j++)
                for(size_t k = 0; k < points.size(); k++)
                    if(abs(pt[j].x - points[k].x) < 1.0 && abs(pt[j].y - points[k].y) < 1)
                        ind[j] = k;
            
            delaunayTri.push_back(ind);
        }
    }
    
}

// Apply affine transform calculated using srcTri and dstTri to src
void applyAffineTransform(Mat &warpImage, Mat &src, vector<Point2f> &srcTri, vector<Point2f> &dstTri)
{
    // Given a pair of triangles, find the affine transform.
    Mat warpMat = getAffineTransform( srcTri, dstTri );
    
    // Apply the Affine Transform just found to the src image
    warpAffine( src, warpImage, warpMat, warpImage.size(), INTER_LINEAR, BORDER_REFLECT_101);
}


// Warps and alpha blends triangular regions from img1 and img2 to img
void warpTriangle(Mat &img1, Mat &img2, vector<Point2f> t1, vector<Point2f> t2)
{
    // Find bounding rectangle for each triangle
    Rect r1 = boundingRect(t1);
    Rect r2 = boundingRect(t2);
    
    // Offset points by left top corner of the respective rectangles
    vector<Point2f> t1Rect, t2Rect;
    vector<Point> t2RectInt;
    for(int i = 0; i < 3; i++)
    {
        //tRect.push_back( Point2f( t[i].x - r.x, t[i].y -  r.y) );
        t2RectInt.push_back( Point((int)(t2[i].x - r2.x), (int)(t2[i].y - r2.y)) ); // for fillConvexPoly
        
        t1Rect.push_back( Point2f( t1[i].x - r1.x, t1[i].y -  r1.y) );
        t2Rect.push_back( Point2f( t2[i].x - r2.x, t2[i].y - r2.y) );
    }
    
    // Get mask by filling triangle
    Mat mask = Mat::zeros(r2.height, r2.width, CV_32FC3);
    fillConvexPoly(mask, t2RectInt, Scalar(1.0, 1.0, 1.0), 16, 0);
    
    // Apply warpImage to small rectangular patches
    Mat img1Rect, img2Rect;
    img1(r1).copyTo(img1Rect);
    
    Mat warpImage = Mat::zeros(r2.height, r2.width, img1Rect.type());
    
    applyAffineTransform(warpImage, img1Rect, t1Rect, t2Rect);
    
    // Copy triangular region of the rectangular patch to the output image
    multiply(warpImage,mask, warpImage);
    multiply(img2(r2), Scalar(1.0,1.0,1.0) - mask, img2(r2));
    img2(r2) = img2(r2) + warpImage;
    
}

// Constrains points to be inside boundary
void constrainPoint(Point2f &p, Size sz)
{
    p.x = min(max( (double)p.x, 0.0), (double)(sz.width - 1));
    p.y = min(max( (double)p.y, 0.0), (double)(sz.height - 1));
    
}


// 分隔字符串
vector<string> splitCustomStr(string& str,const char* c)
{
    char *cstr, *p;
    vector<string> res;
    cstr = new char[str.size()+1];
    strcpy(cstr,str.c_str());
    p = strtok(cstr,c);
    while(p!=NULL)
    {
        res.push_back(p);
        p = strtok(NULL,c);
    }
    delete cstr;
    return res;
}


//将特定格式的字符串转为需要的点数组
void getAllPoints(vector<string> pointsStrArrs, vector<vector<Point2f> > &pointsVec) {
    for(size_t i = 0; i < pointsStrArrs.size(); i++)
    {
        vector<Point2f> points;
        
        vector<string>pointArrs;
        pointArrs = splitCustomStr(pointsStrArrs[i], "_");
        
        for (size_t j = 0;j < pointArrs.size(); j++) {
            vector<string>pointValues;
            pointValues = splitCustomStr(pointArrs[j], ",");
            
            float x = atof(pointValues[0].c_str());
            float y = atof(pointValues[1].c_str());
            points.push_back(Point2f(x, y));
        }
        pointsVec.push_back(points);
    }
}

//开始
int getAverageFace(string imageNameStr, string allPointStr, string resultPath) {
    
    vector<string>imageNames;
    imageNames=splitCustomStr(imageNameStr, ",");
    
    vector<string>pointStrs;
    pointStrs = splitCustomStr(allPointStr, "-");
    vector<vector<Point2f> >allPoints;
    getAllPoints(pointStrs, allPoints);
    
    // Exit program if no images or pts are found or if the number of image files does not match with the number of point files
    if(imageNames.empty() || allPoints.empty() || imageNames.size() != allPoints.size())
        return 0;
    
    // Dimensions of output image
    int w = 600;
    int h = 600;
    // Read images
    vector<Mat> images;
    for(size_t i = 0; i < imageNames.size(); i++)
    {
        Mat img = imread(imageNames[i]);
        
        img.convertTo(img, CV_32FC3, 1/255.0);
        
        if(!img.data)
        {
            cout << "image " << imageNames[i] << " not read properly" << endl;
        }
        else
        {
            images.push_back(img);
        }
    }
    
    if(images.empty())
    {
        cout << "No images found " << endl;
        return 0;
    }
    
    int numImages = images.size();
    
    
    // Eye corners
    vector<Point2f> eyecornerDst, eyecornerSrc;
    eyecornerDst.push_back(Point2f( 0.3*w, h/3));
    eyecornerDst.push_back(Point2f( 0.7*w, h/3));
    
    eyecornerSrc.push_back(Point2f(0,0));
    eyecornerSrc.push_back(Point2f(0,0));
    
    // Space for normalized images and points.
    vector <Mat> imagesNorm;
    vector < vector <Point2f> > pointsNorm;
    
    // Space for average landmark points
    vector <Point2f> pointsAvg(allPoints[0].size());
    
    // 8 Boundary points for Delaunay Triangulation
    vector <Point2f> boundaryPts;
    boundaryPts.push_back(Point2f(0,0));
    boundaryPts.push_back(Point2f(w/2, 0));
    boundaryPts.push_back(Point2f(w-1,0));
    boundaryPts.push_back(Point2f(w-1, h/2));
    boundaryPts.push_back(Point2f(w-1, h-1));
    boundaryPts.push_back(Point2f(w/2, h-1));
    boundaryPts.push_back(Point2f(0, h-1));
    boundaryPts.push_back(Point2f(0, h/2));
    
    // Warp images and trasnform landmarks to output coordinate system,
    // and find average of transformed landmarks.
    
    for(size_t i = 0; i < images.size(); i++)
    {
        
        vector <Point2f> points = allPoints[i];
        
        // The corners of the eyes are the landmarks number 36 and 45
        eyecornerSrc[0] = allPoints[i][0];
        eyecornerSrc[1] = allPoints[i][1];
        
        // Calculate similarity transform
        Mat tform;
        similarityTransform(eyecornerSrc, eyecornerDst, tform);
        
        // Apply similarity transform to input image and landmarks
        Mat img = Mat::zeros(h, w, CV_32FC3);
        warpAffine(images[i], img, tform, img.size());
        transform( points, points, tform);
        
        // Calculate average landmark locations
        for ( size_t j = 0; j < points.size(); j++)
        {
            pointsAvg[j] += points[j] * ( 1.0 / numImages);
        }
        
        // Append boundary points. Will be used in Delaunay Triangulation
        for ( size_t j = 0; j < boundaryPts.size(); j++)
        {
            points.push_back(boundaryPts[j]);
        }
        
        pointsNorm.push_back(points);
        imagesNorm.push_back(img);
        
        
    }
    
    // Append boundary points to average points.
    for ( size_t j = 0; j < boundaryPts.size(); j++)
    {
        pointsAvg.push_back(boundaryPts[j]);
    }
    
    
    
    // Calculate Delaunay triangles
    Rect rect(0, 0, w, h);
    vector< vector<int> > dt;
    calculateDelaunayTriangles(rect, pointsAvg, dt);
    
    // Space for output image
    Mat output = Mat::zeros(h, w, CV_32FC3);
    Size size(w,h);
    
    // Warp input images to average image landmarks
    
    for(size_t i = 0; i < numImages; i++)
    {
        Mat img = Mat::zeros(h, w, CV_32FC3);
        // Transform triangles one by one
        for(size_t j = 0; j < dt.size(); j++)
        {
            // Input and output points corresponding to jth triangle
            vector<Point2f> tin, tout;
            for(int k = 0; k < 3; k++)
            {
                Point2f pIn = pointsNorm[i][dt[j][k]];
                constrainPoint(pIn, size);
                
                Point2f pOut = pointsAvg[dt[j][k]];
                constrainPoint(pOut,size);
                
                tin.push_back(pIn);
                tout.push_back(pOut);
            }
            
            warpTriangle(imagesNorm[i], img, tin, tout);
        }
        
        // Add image intensities for averaging
        output = output + img;
        
    }
    
    // Divide by numImages to get average
    output = output / (double)numImages;
    
    // Display result
    //    imshow("image", output);
    output.convertTo(output, CV_8UC3, 255.0);
    
    imwrite(resultPath, output);
    
    return 1;
}
