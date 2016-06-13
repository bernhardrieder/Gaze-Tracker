#pragma once

namespace gt
{
	class FaceDetection
	{
	public:
		/*uses default cascades defined in this headerfile*/
		FaceDetection();
		FaceDetection(const cv::String& faceCascadeName, const cv::String& leftEyeCascadeName, const cv::String& rightEyeCascadeName);
		~FaceDetection();

		struct FaceROI
		{
			cv::Rect leftSideROI, rightSideROI;
		};

		struct EyesROI
		{
			cv::Rect left, right;
		};

		void CheckForFaceROIsWithCascadeClassifier(const cv::Mat& flippedFrameGray, std::vector<FaceROI>& out);
		void CheckForEyesROIWithCascadeClassifier(const cv::Mat& flippedFrameGray, const FaceROI& face, EyesROI& out);
		bool GetEyes(const cv::Mat& flippedFrameGray, cv::Mat& leftEye, cv::Mat& rightEye, double resizeFactor = 0.0, bool equalizeHist = false);
		void GetEyesForIrisDetection(const cv::Mat& flippedFrameGray, cv::Mat& leftIris, cv::Mat& rightIris);
		void GetIrisesCenterPositions(const cv::Mat& flippedFrameGray, const FaceROI& faceROI, cv::Point& leftIris, cv::Point& rightIris);
		cv::Size GetAverageHeadSize();
		cv::Point GetHeadCenterPosition() const;
		void DetectFaceEyeIrisAndDraw(cv::Mat& flippedFrameGray, bool drawFace = true, bool drawEye = true, bool drawIris = true);
		void ReloadTemplates();

		int templateMatchingMethod = 0;
		static cv::String eyeLeftTemplateName;
		static cv::String eyeRightTemplateName;
		double eyeTemplateResizeFactor;
	private:
		bool initClassifiers(const cv::String& faceCascadeName, const cv::String& leftEyeCascadeName, const cv::String& rightEyeCascadeName);
		static void setEyeROI(const std::vector<cv::Rect>& eyes, const cv::Rect& faceRegion, cv::Rect& eye, cv::Rect& lastDeteced);
		static void adjustEyeROI(const cv::Rect& faceRegion, const std::vector<cv::Rect>& eye, cv::Rect& out);
		void createEyeROIFomFaceROI(const FaceROI& faceROI, EyesROI& outEyeROI) const;
		static void createEyeROIFomFaceROI(const cv::Rect& faceROI, const cv::Rect& lastDetectedEyeROI, cv::Rect& outEyeROI);
		void getIrisCenterPosition(const cv::Mat& flippedFrameGray, const cv::Rect& eyeROI, const cv::Mat& eyeTemplate, cv::Point& out) const;
		void detectEyeAndDraw(cv::Mat& flippedFrameGray, const FaceROI& face, bool draw = true);
		void detectIrisesAndDraw(cv::Mat& flippedFrameGray, const FaceROI& faceROI, bool draw = true);
		void addHeadSize(cv::Size size);

		cv::CascadeClassifier m_FaceCascadeClassifier;
		cv::CascadeClassifier m_LeftEyeCascadeClassifier;
		cv::CascadeClassifier m_RightEyeCascadeClassifier;
		EyesROI m_LastDetectedEyesROIFromCascadeClassifier;
		std::list<cv::Size> m_LastDetectedSizesOfHeadsWithEyes;
		cv::Point m_LastDetectedCenterPointOfHeadWithEyes;

		cv::Mat m_EyeLeftTemplate;
		cv::Mat m_EyeRightTemplate;

		const cv::String m_face_cascade_name_default = "cascade_classifier/haarcascades/opencv/haarcascade_frontalface_alt.xml";
		const cv::String m_left_eye_cascade_name_default = "cascade_classifier/haarcascades/opencv/haarcascade_lefteye_2splits.xml";
		const cv::String m_right_eye_cascade_name_default = "cascade_classifier/haarcascades/opencv/haarcascade_righteye_2splits.xml";
	};
}

