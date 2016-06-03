#pragma once



class FaceDetection 
{
public:
	/*uses default cascades defined in this headerfile*/
	FaceDetection();
	FaceDetection(const cv::String& faceCascadeName, const cv::String& leftEyeCascadeName, const cv::String& rightEyeCascadeName);
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
	bool initClassifiers(const cv::String& faceCascadeName, const cv::String& leftEyeCascadeName, const cv::String& rightEyeCascadeName);

	cv::CascadeClassifier m_FaceCascadeClassifier;
	cv::CascadeClassifier m_LeftEyeCascadeClassifier;
	cv::CascadeClassifier m_RightEyeCascadeClassifier;

	cv::Mat m_EyeLeftTemplate;
	cv::Mat m_EyeRightTemplate;

	const cv::String m_face_cascade_name_default = "cascade_classifier/haarcascades/opencv/haarcascade_frontalface_alt.xml";
	const cv::String m_left_eye_cascade_name_default = "cascade_classifier/haarcascades/opencv/haarcascade_lefteye_2splits.xml";
	const cv::String m_right_eye_cascade_name_default = "cascade_classifier/haarcascades/opencv/haarcascade_righteye_2splits.xml";
};

inline const std::string ToString(enum FaceDetection::Eye v)
{
	switch (v)
	{
	case FaceDetection::LEFT:   return "LEFT";
	case FaceDetection::RIGHT:   return "RIGHT";
	}
}