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
	cv::equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	m_FaceCascadeClassifier.detectMultiScale(frame_gray, faces, 1.2, 5, 0 | cv::CASCADE_SCALE_IMAGE | cv::CASCADE_DO_CANNY_PRUNING, cv::Size(30, 30));
	for (size_t i = 0; i < faces.size(); i++)
	{
		cv::Rect leftSide = faces[i];
		leftSide -= cv::Size(faces[i].width * 0.5f, 0);
		cv::Rect rightSide = leftSide;
		rightSide.x += leftSide.width;


		cv::rectangle(frame, leftSide, cv::Scalar(255, 255, 0));
		cv::rectangle(frame, rightSide, cv::Scalar(255, 0, 255));
		//cv::rectangle(frame, faces[i], cv::Scalar(255, 0, 255));

		//-- In each face, detect eyes
		//m_EyesCascadeClassifier.detectMultiScale(frame_gray(faces[i]), eyes, 1.2, 6, 0 | cv::CASCADE_SCALE_IMAGE | cv::CASCADE_DO_CANNY_PRUNING, 
		//	cv::Size(30, 30));

		m_LeftEyeCascadeClassifier.detectMultiScale(frame_gray(leftSide), leftEye, 1.1, 3, 0 | cv::CASCADE_SCALE_IMAGE | cv::CASCADE_DO_CANNY_PRUNING, 
			cv::Size(30, 30));
		m_RightEyeCascadeClassifier.detectMultiScale(frame_gray(rightSide), rightEye, 1.1, 3, 0 | cv::CASCADE_SCALE_IMAGE | cv::CASCADE_DO_CANNY_PRUNING, 
			cv::Size(30, 30));
		for (size_t j = 0; j < leftEye.size(); j++)
		{
			cv::Rect eye = cv::Rect(leftSide.x + leftEye[j].x, leftSide.y + leftEye[j].y, leftEye[j].width, leftEye[j].width);
			cv::rectangle(frame, eye, cv::Scalar(255, 0, 0));
		}
		for (size_t j = 0; j < rightEye.size(); j++)
		{
			cv::Rect eye = cv::Rect(rightSide.x + rightEye[j].x, rightSide.y + rightEye[j].y, rightEye[j].width, rightEye[j].width);
			cv::rectangle(frame, eye, cv::Scalar(255, 0, 0));
		}
	}
	//-- Show what you got
	cv::imshow("result", frame);
}

void FaceDetection::pupilDetection(cv::Mat& frame, cv::Mat& roi)
{
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
