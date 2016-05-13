#pragma once



class FaceDetection 
{
public:
	FaceDetection(std::string faceCascadeName, std::string eyesCascadeName);
	FaceDetection(std::string faceCascadeName, std::string leftEyeCascadeName, std::string rightEyeCascadeName);
	~FaceDetection();

	enum Eye
	{
		LEFT, RIGHT
	};


	void test_detectAndDraw_v1(cv::Mat& frame, double scale, bool tryflip);
	void test_detectAndDraw_v4(cv::Mat& frame);
	void detectAndDraw(cv::Mat& frame);
	void eyeDetection(cv::Mat& frame, cv::Mat& frame_gray, cv::Rect& faceRegion, enum Eye eyeSide);
	void pupilDetection(cv::Mat& frame, cv::Mat& roi, enum Eye eyeSide, cv::Rect roiRect);

private:
	bool initClassifier(std::string faceCascadeName, std::string eyesCascadeName);
	bool initClassifier(std::string faceCascadeName, std::string leftEyeCascadeName, std::string rightEyeCascadeName);
private:
	cv::CascadeClassifier m_FaceCascadeClassifier;
	cv::CascadeClassifier m_EyesCascadeClassifier;
	cv::CascadeClassifier m_LeftEyeCascadeClassifier;
	cv::CascadeClassifier m_RightEyeCascadeClassifier;

	bool m_IsFaceSplit = false;
};

inline const std::string ToString(enum FaceDetection::Eye v)
{
	switch (v)
	{
	case FaceDetection::LEFT:   return "LEFT";
	case FaceDetection::RIGHT:   return "RIGHT";
	}
}