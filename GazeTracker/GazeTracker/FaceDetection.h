#pragma once
class FaceDetection 
{
public:
	FaceDetection(std::string faceCascadeName, std::string eyesCascadeName);
	FaceDetection(std::string faceCascadeName, std::string leftEyeCascadeName, std::string rightEyeCascadeName);
	~FaceDetection();


	void test_detectAndDraw_v1(cv::Mat& frame, double scale, bool tryflip);
	void test_detectAndDraw_v2(cv::Mat& frame);
	void test_detectAndDraw_v3(cv::Mat& frame);
	void test_detectAndDraw_v4(cv::Mat& frame);
	void detectAndDrawWithLeftAndRightEye(cv::Mat& frame);
	void pupilDetection(cv::Mat& frame, cv::Mat& roi);
private:
	bool initClassifier(std::string faceCascadeName, std::string eyesCascadeName);
	bool initClassifier(std::string faceCascadeName, std::string leftEyeCascadeName, std::string rightEyeCascadeName);
private:
	cv::CascadeClassifier m_FaceCascadeClassifier;
	cv::CascadeClassifier m_EyesCascadeClassifier;
	cv::CascadeClassifier m_LeftEyeCascadeClassifier;
	cv::CascadeClassifier m_RightEyeCascadeClassifier;
};

