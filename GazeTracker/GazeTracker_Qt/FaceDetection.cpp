#include "stdafx.h"
#include "FaceDetection.h"

cv::String eyeLeftTemplateName = "template_matching/eye_left.png";
cv::String eyeRightTemplateName = "template_matching/eye_right.png";

FaceDetection::FaceDetection() : FaceDetection(m_face_cascade_name_default, m_left_eye_cascade_name_default, m_right_eye_cascade_name_default)
{}

FaceDetection::FaceDetection(const cv::String& faceCascadeName, const cv::String& leftEyeCascadeName, const cv::String& rightEyeCascadeName)
{
	initClassifiers(faceCascadeName, leftEyeCascadeName, rightEyeCascadeName);
	m_EyeLeftTemplate = cv::imread(eyeLeftTemplateName, cv::IMREAD_GRAYSCALE);
	m_EyeRightTemplate = cv::imread(eyeRightTemplateName, cv::IMREAD_GRAYSCALE);

	m_LastDetectedEyesROIFromCascadeClassifier.left.x = m_LastDetectedEyesROIFromCascadeClassifier.right.x = m_LastDetectedEyesROIFromCascadeClassifier.left.y = m_LastDetectedEyesROIFromCascadeClassifier.right.y = 0;
}

FaceDetection::~FaceDetection()
{
}

//flips frame automatically
void FaceDetection::CheckForFaceROIsWithCascadeClassifier(const cv::Mat& flippedFrameGray, std::vector<FaceROI>& out)
{
	if (flippedFrameGray.empty()) return;
	std::vector<cv::Rect> faces;
	cv::Mat frameHist = flippedFrameGray.clone();
	cv::equalizeHist(frameHist, frameHist);
	m_FaceCascadeClassifier.detectMultiScale(frameHist, faces, 1.2, 5, 0 | cv::CASCADE_SCALE_IMAGE | cv::CASCADE_DO_CANNY_PRUNING | cv::CASCADE_FIND_BIGGEST_OBJECT, cv::Size(30, 30));
	if (faces.size() > 0)
	{
		for(auto face : faces)
		{
			FaceROI faceROI;
			faceROI.leftSideROI = face;
			faceROI.leftSideROI -= cv::Size(face.width * 0.5f, 0);
			faceROI.rightSideROI = faceROI.leftSideROI;
			faceROI.rightSideROI.x += faceROI.leftSideROI.width;
			out.push_back(faceROI);
		}
	}
}

void FaceDetection::CheckForEyesROIWithCascadeClassifier(const cv::Mat& flippedFrameGray, const FaceROI& face, EyesROI& out)
{
	if (flippedFrameGray.empty()) return;
	cv::Mat frameHist = flippedFrameGray.clone();
	cv::equalizeHist(frameHist, frameHist); // -> really needed for better face detection?
	cv::Mat leftSide = frameHist.clone()(face.leftSideROI);
	cv::Mat rightSide = frameHist.clone()(face.rightSideROI);
	std::vector<cv::Rect> eyesLeft, eyesRight;

	m_LeftEyeCascadeClassifier.detectMultiScale(leftSide, eyesLeft, 1.1, 3, 0 | cv::CASCADE_SCALE_IMAGE | cv::CASCADE_DO_CANNY_PRUNING, cv::Size(20, 20));
	m_RightEyeCascadeClassifier.detectMultiScale(rightSide, eyesRight, 1.1, 3, 0 | cv::CASCADE_SCALE_IMAGE | cv::CASCADE_DO_CANNY_PRUNING, cv::Size(20, 20));

	adjustEyeROI(face.leftSideROI, eyesLeft, out.left);
	adjustEyeROI(face.rightSideROI, eyesRight, out.right);

	if(out.left.x != 0 || out.left.y != 0)
		m_LastDetectedEyesROIFromCascadeClassifier.left = out.left;	
	if (out.right.x != 0 || out.right.y != 0)
		m_LastDetectedEyesROIFromCascadeClassifier.right = out.right;
}

void FaceDetection::adjustEyeROI(const cv::Rect& faceRegion, const std::vector<cv::Rect>& eye, cv::Rect& out)
{
	if (eye.size() == 0) return;
	for (size_t j = 0; j < eye.size(); j++)
	{
		//float offsetX = eye[j].width / 3.;
		//float yDivid = 3.;
		//float offsetY = eye[j].height / yDivid;
		out = cv::Rect(faceRegion.x + eye[j].x /*+ offsetX / 2*/, faceRegion.y + eye[j].y /*+ offsetY*(yDivid / 2)*/, eye[j].width /*- offsetX*/, eye[j].height /*- offsetY * 2*/);
	}
}


bool FaceDetection::GetEyes(const cv::Mat& flippedFrameGray, cv::Mat& leftEye, cv::Mat& rightEye, double resizeFactor, bool equalizeHist)
{
	if (flippedFrameGray.empty()) return false;
	std::vector<FaceROI> faces;
	EyesROI eyes;
	CheckForFaceROIsWithCascadeClassifier(flippedFrameGray, faces);
	if (faces.size() > 0)
	{
		for (auto face : faces)
		{
			CheckForEyesROIWithCascadeClassifier(flippedFrameGray, face, eyes);
		}
		leftEye = flippedFrameGray.clone()(eyes.left);
		rightEye = flippedFrameGray.clone()(eyes.right);
		if(resizeFactor > 0)
		{
			if(!leftEye.empty())
				cv::resize(leftEye, leftEye, cv::Size(), resizeFactor, resizeFactor);
			if(!rightEye.empty())
				cv::resize(rightEye, rightEye, cv::Size(), resizeFactor, resizeFactor);
		}
		if(equalizeHist)
		{
			if (!leftEye.empty())
				cv::equalizeHist(leftEye, leftEye);
			if (!rightEye.empty())
				cv::equalizeHist(rightEye, rightEye);
		}
		return true;
	}
	return false;
}

void FaceDetection::GetIrisesCenterPositions(const cv::Mat& flippedFrameGray, const FaceROI& faceROI, cv::Point& leftIris, cv::Point& rightIris)
{
	if (flippedFrameGray.empty()) return;
	EyesROI eyesROI;
	CheckForEyesROIWithCascadeClassifier(flippedFrameGray, faceROI, eyesROI); //check if new eye pos found

	//because of eyeCascadeClassifier isn't very reliable we dont use the received ROI
	//instead of using the eye roi, we are gonna using the roi of the face and create our own eye roi
	createEyeROIFomFaceROI(faceROI.leftSideROI, m_LastDetectedEyesROIFromCascadeClassifier.left, eyesROI.left);
	createEyeROIFomFaceROI(faceROI.rightSideROI, m_LastDetectedEyesROIFromCascadeClassifier.right, eyesROI.right);

	/*------------------DEBUG------------------*/
	//cv::Mat left, right;
	//left = flippedFrameGray(eyesROI.left);
	//cv::resize(left, left, cv::Size(), 15, 15);
	//right = flippedFrameGray(eyesROI.right);
	//cv::resize(right, right, cv::Size(), 15, 15);
	//cv::imshow("left test", left);
	//cv::imshow("right test", right);
	/*------------------DEBUG------------------*/

	int radius = 3;
	if (eyesROI.left.x != 0 || eyesROI.left.y != 0)
	{
		getIrisCenterPosition(flippedFrameGray, eyesROI.left, m_EyeLeftTemplate, leftIris);
	}
	if (eyesROI.right.x != 0 || eyesROI.right.y != 0)
	{
		getIrisCenterPosition(flippedFrameGray, eyesROI.right, m_EyeRightTemplate, rightIris);
	}
}

void FaceDetection::detectFaceEyeIrisAndDraw(cv::Mat& flippedFrameGray)
{
	std::vector<FaceROI> out;
	CheckForFaceROIsWithCascadeClassifier(flippedFrameGray, out);
	if(out.size() > 0)
	{
		for(auto roi : out)
		{
			cv::rectangle(flippedFrameGray, roi.leftSideROI, cv::Scalar(255, 255, 0));
			cv::rectangle(flippedFrameGray, roi.rightSideROI, cv::Scalar(255, 0, 255));

			detectEyeAndDraw(flippedFrameGray, roi);
			detectIrisesAndDraw(flippedFrameGray, roi);
		}
	}

	cv::imshow("result", flippedFrameGray);
}

void FaceDetection::createEyeROIFomFaceROI(const cv::Rect& faceROI, const cv::Rect& lastDetectedEyeROI, cv::Rect& outEyeROI)
{
	outEyeROI = faceROI;
	if (lastDetectedEyeROI.x != 0 || lastDetectedEyeROI.y != 0)
	{
		outEyeROI.height /= 8;
		outEyeROI.y = lastDetectedEyeROI.y + (lastDetectedEyeROI.height / 2) - (outEyeROI.height / 4); // y pos of height/2 of eye
		outEyeROI.width /= 2;
		outEyeROI.x = lastDetectedEyeROI.x + (lastDetectedEyeROI.width / 2) - (outEyeROI.width / 2); // x pos of width/2 of eye
	}
}

void FaceDetection::detectEyeAndDraw(cv::Mat& flippedFrameGray, const FaceROI& face)
{
	if (flippedFrameGray.empty()) return;
	EyesROI eyes;
	CheckForEyesROIWithCascadeClassifier(flippedFrameGray, face, eyes);

	cv::rectangle(flippedFrameGray, eyes.left, cv::Scalar(255, 0, 0));
	cv::rectangle(flippedFrameGray, eyes.right, cv::Scalar(255, 0, 0));
}

void FaceDetection::detectIrisesAndDraw(cv::Mat& flippedFrameGray, const FaceROI& faceROI)
{
	if (flippedFrameGray.empty()) return;
	cv::Point leftIris, rightIris;
	GetIrisesCenterPositions(flippedFrameGray, faceROI, leftIris, rightIris);
	int radius = 3;
	if(leftIris.x != 0 || leftIris.y != 0)
	{
		circle(flippedFrameGray, leftIris, radius, cv::Scalar(255, 255, 255), -1);
	}
	if (rightIris.x != 0 || rightIris.y != 0)
	{
		circle(flippedFrameGray, rightIris, radius, cv::Scalar(255, 255, 255), -1);
	}
}

void FaceDetection::getIrisCenterPosition(const cv::Mat& flippedFrameGray, const cv::Rect& eyeROI, const cv::Mat& eyeTemplate, cv::Point& out) const
{
	if (flippedFrameGray.empty()) return;

	//euqlize hist for better processing and resize frame because of template resize factor (and fit to each other)
	cv::Mat frameHistAndResized, result, templateHist;
	cv::equalizeHist(flippedFrameGray, frameHistAndResized);
	cv::equalizeHist(eyeTemplate, templateHist);
	frameHistAndResized = frameHistAndResized(eyeROI);
	double resizeFactor = 15; //get this variable from the caller!!!
	cv::resize(frameHistAndResized, frameHistAndResized, cv::Size(), resizeFactor, resizeFactor);
	//cv::resize(templateHist, templateHist, cv::Size(), resizeFactor, resizeFactor); // not needed, already scaled!

	//check if template isn't bigger than frame!
	int result_cols = frameHistAndResized.cols - templateHist.cols + 1;
	int result_rows = frameHistAndResized.rows - templateHist.rows + 1;
	if (result_cols < 0 || result_rows < 0) 
		return;

	//template matching
	cv::matchTemplate(frameHistAndResized, templateHist, result, templateMatchingMethod);
	cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
	double minVal, maxVal;
	cv::Point minLoc, maxLoc, matchLoc;
	cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
	if (templateMatchingMethod == cv::TM_SQDIFF || templateMatchingMethod == cv::TM_SQDIFF_NORMED)
		matchLoc = minLoc;
	else
		matchLoc = maxLoc;
	double mul = 1. / resizeFactor; // resize factor 

	//calculate center position
	cv::Rect rect = cv::Rect(matchLoc *mul, cv::Point(matchLoc.x + eyeTemplate.cols, matchLoc.y + eyeTemplate.rows) * mul);
	rect.x += eyeROI.x;
	rect.y += eyeROI.y;
	out = cv::Point(rect.x + (rect.width / 2), rect.y + (rect.height / 2));
}

bool FaceDetection::initClassifiers(const cv::String& faceCascadeName, const cv::String& leftEyeCascadeName, const cv::String& rightEyeCascadeName)
{
	if (!m_FaceCascadeClassifier.load(faceCascadeName))
	{
		std::cerr << "--(!)Error loading face cascade" << std::endl;
		return false;
	}
	if (!m_LeftEyeCascadeClassifier.load(leftEyeCascadeName))
	{
		std::cerr << "--(!)Error loading eyes cascade" << std::endl;
		return false;
	}
	if (!m_RightEyeCascadeClassifier.load(rightEyeCascadeName))
	{
		std::cerr << "--(!)Error loading eyes cascade" << std::endl;
		return false;
	}
	return true;
}


