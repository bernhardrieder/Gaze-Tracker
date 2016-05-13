#include "stdafx.h"
#include "FaceDetection.h"

cv::String eyeLeftTemplateName = "template_matching/eye_left.png";
cv::String eyeRightTemplateName = "template_matching/eye_right.png";

FaceDetection::FaceDetection(std::string faceCascadeName, std::string leftEyeCascadeName, std::string rightEyeCascadeName)
{
	initClassifier(faceCascadeName, leftEyeCascadeName, rightEyeCascadeName);
	m_EyeLeftTemplate = cv::imread(eyeLeftTemplateName, cv::IMREAD_GRAYSCALE);
	m_EyeRightTemplate = cv::imread(eyeRightTemplateName, cv::IMREAD_GRAYSCALE);
}

FaceDetection::~FaceDetection()
{
}

void FaceDetection::detectAndDraw(cv::Mat& frame)
{
	cv::flip(frame, frame, 1);
	std::vector<cv::Rect> faces, leftEye, rightEye;
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
		-> this causes cv::Mat exceptions?
		
		*/
		//faces[i].y += faces[i].y * 0.2f;
		//faces[i] += cv::Size(0, -(faces[i].height * 0.6f));

		cv::Rect leftSide = faces[i];
		leftSide -= cv::Size(faces[i].width * 0.5f, 0);
		cv::Rect rightSide = leftSide;
		rightSide.x += leftSide.width;


		cv::rectangle(frame, leftSide, cv::Scalar(255, 255, 0));
		cv::rectangle(frame, rightSide, cv::Scalar(255, 0, 255));


		eyeDetection(frame, frame_gray, leftSide, Eye::LEFT);
		eyeDetection(frame, frame_gray, rightSide, Eye::RIGHT);
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
		float offsetX = eye[j].width / 2.5;
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
	cv::equalizeHist(roi, roi);

	double resize = 10;

	cv::resize(roi, roi, cv::Size(), resize, resize);
	cv::imshow(ToString(eyeSide) + "_thres", roi);
	pupilTemplateMatching(frame, roi, eyeSide, roiRect);

	return;
	if (roi.empty()) return;
	cv::imshow(ToString(eyeSide) + "_normal", roi);

	cv::equalizeHist(roi, roi);
	cv::imshow(ToString(eyeSide) + "_hist", roi);

	//brighten -> from tutorial
	int brightnessAdd = 0;
	for (int y = 0; y < roi.rows; y++) {
		for (int x = 0; x < roi.cols; x++) {
			for (int c = 0; c < 3; c++) {
				roi.at<cv::Vec3b>(y, x)[c] =
					cv::saturate_cast<uchar>((roi.at<cv::Vec3b>(y, x)[c]) + brightnessAdd);
			}
		}
	}
	cv::imshow(ToString(eyeSide) + "_bright", roi);
	
	cv::Mat clone = roi.clone();


	cv::threshold(clone, clone, 70, 0, cv::THRESH_TRUNC);
	cv::threshold(clone, clone, 50, 255, cv::THRESH_BINARY); 
	cv::imshow(ToString(eyeSide) + "_thres", clone);

	int gaussianSize = 21;
	double gaussianSigma = 0;
	//double resize = 5;
	
	cv::resize(clone, clone, cv::Size(), resize, resize);
	cv::GaussianBlur(clone, clone, cv::Size(gaussianSize, gaussianSize), gaussianSigma, gaussianSigma);
	//cv::resize(clone, clone, cv::Size(), 1 / resize, 1 / resize);

	cv::threshold(clone, clone, 150, 255, cv::THRESH_BINARY); // v1
	//cv::threshold(clone, clone, 50, 255, cv::THRESH_BINARY); // v2
	cv::imshow(ToString(eyeSide) + "_thres2", clone);
	gaussianSize = 3; //3
	gaussianSigma = 2;// 2

	//cv::resize(clone, clone, cv::Size(), resize, resize);
	cv::GaussianBlur(clone, clone, cv::Size(gaussianSize, gaussianSize), gaussianSigma, gaussianSigma);
	cv::resize(clone, clone, cv::Size(), 1/resize, 1/resize);
	cv::imshow(ToString(eyeSide) + "_filter", clone);


	std::vector<cv::Vec3f> circleVector;
	cv::HoughCircles(clone, circleVector, CV_HOUGH_GRADIENT, 6, clone.size().height, 35, 25, 3, 10);


	for (size_t k = 0; k < circleVector.size(); ++k) {

		//int r = circleVector[k][2];
		int r = 3;

		//cv::Point c(circleVector[k][0],
		//	 circleVector[k][1]);

		//circle(roi, c, r, cv::Scalar(0, 0, 255), 2);

		cv::Point c(roiRect.x + circleVector[k][0],
		roiRect.y + circleVector[k][1]);

		circle(frame, c, r, cv::Scalar(0, 0, 255), -1);
	}
}

void FaceDetection::pupilTemplateMatching(cv::Mat& frame, cv::Mat& eyeRoi, Eye eyeSide, cv::Rect& roiRect)
{
	//TODO: rescale images -> they need to have the same sizes in relation to each other (eyeRoi and template). i think this will lead to some problems in finding the middle point of the pupil. another issue is facing with the template image -> should it be an qubic image? should be the pupil the middle of the picture? -> due to exact middle point detection!!
	cv::Mat result, templ;
	switch(eyeSide)
	{
	case Eye::RIGHT: templ = m_EyeLeftTemplate;
	case Eye::LEFT: templ = m_EyeLeftTemplate;
	}
	//img.copyTo(img_display);
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

bool FaceDetection::initClassifier(std::string faceCascadeName, std::string leftEyeCascadeName, std::string rightEyeCascadeName)
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
