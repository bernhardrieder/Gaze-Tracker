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

	struct FaceROI
	{
		cv::Rect leftSideROI, rightSideROI;
	};

	struct EyesROI
	{
		cv::Rect left, right;
	};

	void CheckForFaces(cv::Mat& frameGray, std::vector<FaceROI>& out, bool flipFace = true);
	void CheckForEyes(cv::Mat& frameGray, FaceROI& face, EyesROI& out);
	bool GetEyes(cv::Mat& frameGray, cv::Mat& leftEye, cv::Mat& rightEye);
	void detectFaceEyeIrisAndDraw(cv::Mat& frameGray);
	void eyeDetection(cv::Mat& frameGray, FaceROI& face, bool drawEye = true);
	void pupilDetection(cv::Mat& frame, cv::Mat& roi, enum Eye eyeSide, cv::Rect roiRect);
	void pupilTemplateMatching(cv::Mat& frame, cv::Mat& eyeRoi, enum Eye eyeSide, cv::Rect& roiRect);

	int templateMatchingMethod = 0;

private:
	bool initClassifiers(const cv::String& faceCascadeName, const cv::String& leftEyeCascadeName, const cv::String& rightEyeCascadeName);
	static void setEyeROI(cv::Rect& faceRegion, std::vector<cv::Rect>& eye, cv::Rect& out);

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