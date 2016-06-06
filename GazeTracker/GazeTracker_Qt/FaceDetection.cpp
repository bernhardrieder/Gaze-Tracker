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
}

FaceDetection::~FaceDetection()
{
}

void FaceDetection::CheckForFaces(cv::Mat& frame, std::vector<FaceROI>& out)
{
	cv::flip(frame, frame, 1);
	std::vector<cv::Rect> faces;
	cv::Mat frame_gray;
	cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
	if (frame_gray.empty()) return;
	cv::equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	m_FaceCascadeClassifier.detectMultiScale(frame_gray, faces, 1.2, 5, 0 | cv::CASCADE_SCALE_IMAGE | cv::CASCADE_DO_CANNY_PRUNING | cv::CASCADE_FIND_BIGGEST_OBJECT, cv::Size(30, 30));
	for (size_t i = 0; i < faces.size(); i++)
	{
		/*
		"Within the face region, 20 % from the top(forehead) and 40 % from the bottom (face below nostrils) can be cropped and rejected from further analysis[65].Inspected region limitation let to gain several milliseconds for every cycle of the method." - Paper: "Single web camera robust interactive eye-gaze tracking method", by "A. WOJCIECHOWSKI and K. FORNALCZYK"
		*/
		faces[i].y += faces[i].y * 0.2f;
		faces[i] += cv::Size(0, -(faces[i].height * 0.6f));

		FaceROI roi;
		roi.leftSideROI = faces[i];
		roi.leftSideROI -= cv::Size(faces[i].width * 0.5f, 0);
		roi.rightSideROI = roi.leftSideROI;
		roi.rightSideROI.x += roi.leftSideROI.width;
		out.push_back(roi);
	}
}

void FaceDetection::detectAndDraw(cv::Mat& frame)
{
	std::vector<FaceROI> out;
	CheckForFaces(frame, out);
	if(out.size() > 0)
	{
		cv::Mat frame_gray;
		cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
		for(auto roi : out)
		{
			cv::rectangle(frame, roi.leftSideROI, cv::Scalar(255, 255, 0));
			cv::rectangle(frame, roi.rightSideROI, cv::Scalar(255, 0, 255));

			eyeDetection(frame, frame_gray, roi.leftSideROI, Eye::LEFT);
			eyeDetection(frame, frame_gray, roi.rightSideROI, Eye::RIGHT);
		}
	}

	//-- Show what you got
	cv::imshow("result", frame);
}

void FaceDetection::eyeDetection(cv::Mat& frame, cv::Mat& frame_gray, cv::Rect& faceRegion, Eye eyeSide)
{

	cv::Mat roi = frame_gray.clone();
	roi = roi(faceRegion);
	if (roi.empty()) return;
	std::vector<cv::Rect> eye;
	cv::CascadeClassifier* classifier = nullptr;
	switch(eyeSide)
	{
	case Eye::LEFT: classifier = &m_LeftEyeCascadeClassifier;
	case Eye::RIGHT: classifier = &m_RightEyeCascadeClassifier;
	}
	CV_Assert(classifier != nullptr);
	classifier->detectMultiScale(roi, eye, 1.1, 3, 0 | cv::CASCADE_SCALE_IMAGE | cv::CASCADE_DO_CANNY_PRUNING,
		cv::Size(20, 20));

	for (size_t j = 0; j < eye.size(); j++)
	{
		float offsetX = eye[j].width / 3.;
		float yDivid = 3.;
		float offsetY = eye[j].height / yDivid;
		cv::Rect eyeRegion = cv::Rect(faceRegion.x + eye[j].x + offsetX / 2, faceRegion.y + eye[j].y + offsetY*(yDivid / 2), eye[j].width - offsetX, eye[j].height - offsetY * 2);
		cv::rectangle(frame, eyeRegion, cv::Scalar(255, 0, 0));
		cv::Mat roi = frame_gray.clone();
		roi = roi(eyeRegion);
		if (roi.empty()) break;
		pupilDetection(frame, roi, eyeSide, eyeRegion);
	}
}

void FaceDetection::pupilDetection(cv::Mat& frame, cv::Mat& roi, Eye eyeSide, cv::Rect roiRect)
{
	if (roi.empty()) return;
	cv::equalizeHist(roi, roi);

	double resize = 10;

	cv::resize(roi, roi, cv::Size(), resize, resize);
	cv::imshow(ToString(eyeSide) + "_hist", roi); // use this to config the templates
	pupilTemplateMatching(frame, roi, eyeSide, roiRect);
}

void FaceDetection::pupilTemplateMatching(cv::Mat& frame, cv::Mat& eyeRoi, Eye eyeSide, cv::Rect& roiRect)
{
	//TODO: 4.06.2016 - config eye templates -> define roi of each eye and save roi size and eyetemplate size. change size of template if head moves in z axis?
	//TODO: rescale images -> they need to have the same sizes in relation to each other (eyeRoi and template). i think this will lead to some problems in finding the middle point of the pupil. another issue is facing with the template image -> should it be an qubic image? should be the pupil the middle of the picture? -> due to exact middle point detection!!
	cv::Mat result, templ;
	switch(eyeSide)
	{
	case Eye::RIGHT: templ = m_EyeLeftTemplate;
	case Eye::LEFT: templ = m_EyeLeftTemplate;
	}
	//img.copyTo(img_display);
	//cv::Mat newTmplt;
	//float cubicSize = eyeRoi.rows;;
	//cv::resize(templ, newTmplt, cv::Size(cubicSize - (cubicSize / 2), cubicSize - (cubicSize / 2)));
	//templ = newTmplt;
	int result_cols = eyeRoi.cols - templ.cols + 1;
	int result_rows = eyeRoi.rows - templ.rows + 1;
	if (result_cols < 0 || result_rows < 0) return;
	result.create(result_rows, result_cols, CV_32FC1);
	cv::matchTemplate(eyeRoi, templ, result, templateMatchingMethod);
	cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
	double minVal, maxVal;
	cv::Point minLoc, maxLoc, matchLoc;
	cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
	if (templateMatchingMethod == cv::TM_SQDIFF || templateMatchingMethod == cv::TM_SQDIFF_NORMED)
		matchLoc = minLoc;
	else
		matchLoc = maxLoc;
	double mul = 1. / 10.; // resize factor 
	cv::Rect rect = cv::Rect(matchLoc *mul, cv::Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows) * mul);
	rect.x += roiRect.x;
	rect.y += roiRect.y;
	//cv::rectangle(frame, rect, cv::Scalar::all(0), 2, 8, 0);
	cv::Point c(rect.x + (rect.width / 2), rect.y + (rect.height / 2));
	int r = 3;
	circle(frame, c, r, cv::Scalar(255, 255, 255), -1);
	//cv::rectangle(result, rect, cv::Scalar::all(0), 2, 8, 0);
	//cv:imshow("img", img_display);
	//cv::imshow(result_window, result);
	//return &rect;
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
