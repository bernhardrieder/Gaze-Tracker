#pragma once

namespace gt
{
	//see http://docs.opencv.org/3.1.0/dd/d74/tutorial_file_input_output_with_xml_yml.html#gsc.tab=0
	class GazeData
	{
	public:
		GazeData();
		explicit GazeData(/*std::string id,*/ double time, cv::Point gazePos, std::string frameFileName);

		void write(cv::FileStorage& fs) const; //Write serialization for this class
		void read(const cv::FileNode& node); //Read serialization for this class
	public: // Data Members
		//std::string id; //really needed?
		double time; //use cpu clock time
		cv::Point gazePosition;
		std::string frameFileName;
	};

	//These write and read functions must be defined for the serialization in FileStorage to work
	static void write(cv::FileStorage& fs, const std::string&, const GazeData& x);
	static void read(const cv::FileNode& node, GazeData& x, const GazeData& default_value = GazeData());

	// This function will print our custom class to the console
	static std::ostream& operator<<(std::ostream& out, const GazeData& m);
}

