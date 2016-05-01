#pragma once
class FaceDetection 
{
public:
	FaceDetection(std::string faceCascadeName, std::string eyesCascadeName);
	~FaceDetection();


	void test_detectAndDraw_v1(cv::Mat& frame, double scale, bool tryflip);
	void test_detectAndDraw_v2(cv::Mat& frame);
	void test_detectAndDraw_v3(cv::Mat& frame);
private:
	bool initClassifier(std::string faceCascadeName, std::string eyesCascadeName);
private:
	cv::CascadeClassifier m_FaceCascadeClassifier;
	cv::CascadeClassifier m_EyesCascadeClassifier;
};

