#include "stdafx.h"
#include "FaceDetection.h"


FaceDetection::FaceDetection(std::string faceCascadeName, std::string eyesCascadeName) : m_LeftEyeCascadeClassifier(), m_RightEyeCascadeClassifier()
{
	initClassifier(faceCascadeName, eyesCascadeName);
}

FaceDetection::FaceDetection(std::string faceCascadeName, std::string leftEyeCascadeName, std::string rightEyeCascadeName) : m_EyesCascadeClassifier()
{
	initClassifier(faceCascadeName, leftEyeCascadeName, rightEyeCascadeName);
}

FaceDetection::~FaceDetection()
{
}

void FaceDetection::test_detectAndDraw_v1(cv::Mat& frame, double scale, bool tryflip)
{
	double t = 0;
	std::vector<cv::Rect> faces, faces2;
	const static cv::Scalar colors[] =
	{
		cv::Scalar(255,0,0),
		cv::Scalar(255,128,0),
		cv::Scalar(255,255,0),
		cv::Scalar(0,255,0),
		cv::Scalar(0,128,255),
		cv::Scalar(0,255,255),
		cv::Scalar(0,0,255),
		cv::Scalar(255,0,255)
	};
	cv::Mat gray, smallImg;

	cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
	double fx = 1 / scale;
	resize(gray, smallImg, cv::Size(), fx, fx, cv::INTER_LINEAR);
	equalizeHist(smallImg, smallImg);

	t = (double)cvGetTickCount();
	m_FaceCascadeClassifier.detectMultiScale(smallImg, faces,
		1.1, 2, 0
		//|CASCADE_FIND_BIGGEST_OBJECT
		//|CASCADE_DO_ROUGH_SEARCH
		| cv::CASCADE_SCALE_IMAGE,
	                         cv::Size(30, 30));
	if (tryflip)
	{
		flip(smallImg, smallImg, 1);
		m_FaceCascadeClassifier.detectMultiScale(smallImg, faces2,
			1.1, 2, 0
			//|CASCADE_FIND_BIGGEST_OBJECT
			//|CASCADE_DO_ROUGH_SEARCH
			| cv::CASCADE_SCALE_IMAGE,
		                         cv::Size(30, 30));
		for (std::vector<cv::Rect>::const_iterator r = faces2.begin(); r != faces2.end(); r++)
		{
			faces.push_back(cv::Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
		}
	}
	t = (double)cvGetTickCount() - t;
	printf("detection time = %g ms\n", t / ((double)cvGetTickFrequency()*1000.));
	for (size_t i = 0; i < faces.size(); i++)
	{
		//face
		cv::Rect r = faces[i];
		cv::Mat smallImgROI;
		std::vector<cv::Rect> nestedObjects;
		cv::Point center;
		cv::Scalar color = colors[i % 8];
		int radius;

		double aspect_ratio = (double)r.width / r.height;
		if (0.75 < aspect_ratio && aspect_ratio < 1.3)
		{
			center.x = cvRound((r.x + r.width*0.5)*scale);
			center.y = cvRound((r.y + r.height*0.5)*scale);
			radius = cvRound((r.width + r.height)*0.25*scale);
			circle(frame, center, radius, color, 3, 8, 0);
		}
		else
			rectangle(frame, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)),
				cvPoint(cvRound((r.x + r.width - 1)*scale), cvRound((r.y + r.height - 1)*scale)),
				color, 3, 8, 0);
		

		//eyes
		smallImgROI = smallImg(r);
		m_EyesCascadeClassifier.detectMultiScale(smallImgROI, nestedObjects,
			1.1, 2, 0
			//|CASCADE_FIND_BIGGEST_OBJECT
			//|CASCADE_DO_ROUGH_SEARCH
			//|CASCADE_DO_CANNY_PRUNING
			| cv::CASCADE_SCALE_IMAGE,
		                               cv::Size(30, 30));
		for (size_t j = 0; j < nestedObjects.size(); j++)
		{
			cv::Rect nr = nestedObjects[j];
			center.x = cvRound((r.x + nr.x + nr.width*0.5)*scale);
			center.y = cvRound((r.y + nr.y + nr.height*0.5)*scale);
			radius = cvRound((nr.width + nr.height)*0.25*scale);
			circle(frame, center, radius, color, 3, 8, 0);
		}
	}
	imshow("result", frame);
}

void FaceDetection::test_detectAndDraw_v2(cv::Mat& frame)
{
	std::vector<cv::Rect> faces;
	cv::Mat frame_gray;
	cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	m_FaceCascadeClassifier.detectMultiScale(frame_gray, faces, 1.1, 3, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
	for (size_t i = 0; i < faces.size(); i++)
	{
		cv::Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
		ellipse(frame, center, cv::Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, cv::Scalar(255, 0, 255), 4, 8, 0);
		cv::Mat faceROI = frame_gray(faces[i]);
		std::vector<cv::Rect> eyes;
		//-- In each face, detect eyes
		m_EyesCascadeClassifier.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
		for (size_t j = 0; j < eyes.size(); j++)
		{
			cv::Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
			int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
			circle(frame, eye_center, radius, cv::Scalar(255, 0, 0), 4, 8, 0);
		}
	}
	//-- Show what you got
	cv::imshow("result", frame);
}

void FaceDetection::test_detectAndDraw_v3(cv::Mat& frame)
{
	std::vector<cv::Rect> faces, eyes;
	cv::Mat frame_gray;
	cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	m_FaceCascadeClassifier.detectMultiScale(frame_gray, faces, 1.1, 3, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
	for (size_t i = 0; i < faces.size(); i++)
	{
		//-- In each face, detect eyes
		m_EyesCascadeClassifier.detectMultiScale(frame_gray(faces[i]), eyes, 1.1, 3, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
		for (size_t j = 0; j < eyes.size(); j++)
		{
			cv::Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
			int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
			circle(frame, eye_center, radius, cv::Scalar(255, 0, 0), 2);
		}
	}
	//-- Show what you got
	cv::imshow("result", frame);
}

void FaceDetection::test_detectAndDraw_v4(cv::Mat& frame)
{
	std::vector<cv::Rect> faces, eyes;
	cv::Mat frame_gray;
	cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
	cv::equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	m_FaceCascadeClassifier.detectMultiScale(frame_gray, faces, 1.2, 5, 0 | cv::CASCADE_SCALE_IMAGE | cv::CASCADE_DO_CANNY_PRUNING, cv::Size(30, 30));
	for (size_t i = 0; i < faces.size(); i++)
	{
		/* 
		"Within the face region, 20 % from the top(forehead) and 40 % from the bottom (face below nostrils) can be cropped and rejected from further analysis[65].Inspected region limitation let to gain several milliseconds for every cycle of the method." - Paper: "Single web camera robust interactive eye-gaze tracking method", by "A. WOJCIECHOWSKI and K. FORNALCZYK" 
		-> this causes cv::Mat exceptions?
		faces[i].y += faces[i].y * 0.2f;
		faces[i] += cv::Size(0, -(faces[i].height * 0.5f));
		*/
		cv::rectangle(frame, faces[i], cv::Scalar(255, 0, 255));

		//-- In each face, detect eyes
		m_EyesCascadeClassifier.detectMultiScale(frame_gray(faces[i]), eyes, 1.2, 6, 0 | cv::CASCADE_SCALE_IMAGE | cv::CASCADE_DO_CANNY_PRUNING, cv::Size(30, 30));
		for (size_t j = 0; j < eyes.size(); j++)
		{
			cv::Rect eye = cv::Rect(faces[i].x + eyes[j].x, faces[i].y + eyes[j].y, eyes[j].width, eyes[j].width);
			//cv::Point start(eye.x, eye.y);
			//cv::Point end(eye.x + eye.width, eye.y + eye.height);
			//cv::line(frame,start, end, cv::Scalar(255, 0, 0));
			//end.y -= eye.height;
			//start.y += eye.height;
			//cv::line(frame, end, start, cv::Scalar(255, 0, 0));
			cv::rectangle(frame, eye, cv::Scalar(255, 0, 0));

			cv::imshow("eye " + j, frame_gray(eye));

		}
	}
	//-- Show what you got
	cv::imshow("result", frame);
}

void FaceDetection::detectAndDrawWithLeftAndRightEye(cv::Mat& frame)
{

	cv::flip(frame, frame, 1);
	std::vector<cv::Rect> faces, leftEye, rightEye;
	cv::Mat frame_gray;
	cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
	if (frame_gray.empty()) return;
	cv::equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	m_FaceCascadeClassifier.detectMultiScale(frame_gray, faces, 1.2, 5, 0 | cv::CASCADE_SCALE_IMAGE | cv::CASCADE_DO_CANNY_PRUNING, cv::Size(30, 30));
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

void FaceDetection::eyeDetection(cv::Mat& frame, cv::Mat& frame_gray, cv::Rect& faceRegion, enum Eye eyeSide)
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
	cv::imshow(ToString(eyeSide) + "_normal", roi);
	cv::equalizeHist(roi, roi);
	cv::imshow(ToString(eyeSide) + "_hist", roi);

	//brighten -> from tutorial
	if (!roi.empty())
	{
		int beta = 10;
		for (int y = 0; y < roi.rows; y++) {
			for (int x = 0; x < roi.cols; x++) {
				for (int c = 0; c < 3; c++) {
					roi.at<cv::Vec3b>(y, x)[c] =
						cv::saturate_cast<uchar>((roi.at<cv::Vec3b>(y, x)[c]) + beta);
				}
			}
		}
		cv::imshow(ToString(eyeSide) + "_bright", roi);
	}

	int i = 21;
	//cv::medianBlur(roi, roi, 1);
	cv::Mat clone = roi.clone();


	cv::threshold(clone, clone, 70, 0, cv::THRESH_TRUNC);
	//cv::threshold(clone, clone, 50, 255, cv::THRESH_TOZERO_INV);
	cv::threshold(clone, clone, 50, 255, cv::THRESH_BINARY); // v1
	//cv::threshold(clone, clone, 50, 150, cv::THRESH_BINARY_INV);
	cv::imshow(ToString(eyeSide) + "_thres", clone);

	//cv::bilateralFilter(roi, clone, i, i * 2, i / 2);
	cv::GaussianBlur(clone, clone, cv::Size(i, i),0);
	//cv::blur(roi, clone, cv::Size(i, i), cv::Point(-1, -1));
	cv::imshow(ToString(eyeSide) + "_filter", clone);

	cv::threshold(clone, clone, 150, 255, cv::THRESH_BINARY); // v1
	//cv::threshold(clone, clone, 50, 255, cv::THRESH_BINARY); // v2
	cv::imshow(ToString(eyeSide) + "_thres2", clone);
	cv::GaussianBlur(clone, clone, cv::Size(3, 3), 2, 2);
	cv::imshow(ToString(eyeSide) + "_filter", clone);

	//cv::Canny(clone, clone, 0, 100); // v1
	//cv::imshow(ToString(eyeSide) + "_canny", clone); //v1

	std::vector<cv::Vec3f> circleVector;
	cv::HoughCircles(clone, circleVector, CV_HOUGH_GRADIENT, 3, clone.size().height, 35, 25, 1, 10);


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

	cv::imshow(ToString(eyeSide) + "_circle", roi);


	/*
	
	cvAddS(smoothImage, cvScalar(70,70,70), smoothImage);
	cvThreshold(smoothImage,smoothImage, 120,255,CV_THRESH_BINARY);
	cvSmooth(smoothImage , smoothImage, CV_GAUSSIAN, 3,0);
	
	*/


	//cv::Mat eyeROIgray;

	//cv::cvtColor(eyeROI, eyeROIgray, cv::COLOR_BGR2GRAY);
	//equalizeHist(eyeROIgray, eyeROIgray);


	//cv::GaussianBlur(eyeROIgray, eyeROIgray, cv::Size(9, 9), 2, 2);

	//cv::threshold(eyeROIgray, eyeROIgray, 40, 255, cv::THRESH_BINARY_INV);

	//cv::Canny(eyeROIgray, eyeROIgray, 5, 10, 3);


	//std::vector<cv::Vec3f> circleVector;

	//cv::HoughCircles(eyeROIgray, circleVector, CV_HOUGH_GRADIENT, 2, 20.0, 70, 30, 1, 20);


	//for (size_t k = 0; k < circleVector.size(); ++k) {
	//	cv::Point c(faces[i].x + eyes[j].x + circleVector[k][0],
	//		faces[i].y + eyes[j].y + circleVector[k][1]);
	//	int r = circleVector[k][2];

	//	circle(frame, c, r, cv::Scalar(0, 0, 255), 2);
}

bool FaceDetection::initClassifier(std::string faceCascadeName, std::string eyesCascadeName)
{
	if (!m_FaceCascadeClassifier.load(faceCascadeName))
	{
		std::cerr << "--(!)Error loading face cascade" << std::endl; 
		return false;
	}
	if(!m_EyesCascadeClassifier.load(eyesCascadeName))
	{
		std::cerr << "--(!)Error loading eyes cascade" << std::endl;
		return false;
	}

	return true;
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
