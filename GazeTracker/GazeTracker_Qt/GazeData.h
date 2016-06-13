#pragma once

namespace gt
{
	//see http://docs.opencv.org/3.1.0/dd/d74/tutorial_file_input_output_with_xml_yml.html#gsc.tab=0
	class GazeData
	{
	public:
		GazeData();
		GazeData(const GazeData&);
		explicit GazeData(double time, cv::Point gazePos, std::string frameFileName);
		GazeData operator= (const GazeData&) const;
		void write(cv::FileStorage& fs) const; //Write serialization for this class
		void read(const cv::FileNode& node); //Read serialization for this class

		//These write and read functions must be defined for the serialization in FileStorage to work
		static void write(cv::FileStorage& fs, const std::string&, const GazeData& x);
		static void read(const cv::FileNode& node, GazeData& x, const GazeData& default_value = GazeData());
	public: // Data Members
		double time; //use cpu clock time
		cv::Point gazePosition;
		std::string frameFileName;
	private:
		const std::string t = "time";
		const std::string pos = "gazeScreenPos";
		const std::string file = "frameFile";
	};

	// This function will print our custom class to the console
	static std::ostream& operator<<(std::ostream& out, const GazeData& m);
}

