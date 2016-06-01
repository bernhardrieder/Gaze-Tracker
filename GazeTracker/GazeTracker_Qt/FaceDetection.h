#pragma once



class FaceDetection 
{
public:
	FaceDetection(const std::string faceCascadeName, const std::string leftEyeCascadeName, const std::string rightEyeCascadeName);
	~FaceDetection();

	enum Eye
	{
		LEFT, RIGHT
	};

	void detectAndDraw(cv::Mat& frame);
	void eyeDetection(cv::Mat& frame, cv::Mat& frame_gray, cv::Rect& faceRegion, enum Eye eyeSide);
	void pupilDetection(cv::Mat& frame, cv::Mat& roi, enum Eye eyeSide, cv::Rect roiRect);
	void pupilTemplateMatching(cv::Mat& frame, cv::Mat& eyeRoi, enum Eye eyeSide, cv::Rect& roiRect);

	int templateMatchingMethod = 0;

private:
	bool initClassifiers(const std::string& faceCascadeName, const std::string& leftEyeCascadeName, const std::string& rightEyeCascadeName);

	cv::CascadeClassifier m_FaceCascadeClassifier;
	cv::CascadeClassifier m_LeftEyeCascadeClassifier;
	cv::CascadeClassifier m_RightEyeCascadeClassifier;

	cv::Mat m_EyeLeftTemplate;
	cv::Mat m_EyeRightTemplate;
};

inline const std::string ToString(enum FaceDetection::Eye v)
{
	switch (v)
	{
	case FaceDetection::LEFT:   return "LEFT";
	case FaceDetection::RIGHT:   return "RIGHT";
	}
}