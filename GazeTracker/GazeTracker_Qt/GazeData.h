#pragma once

//see http://docs.opencv.org/3.1.0/dd/d74/tutorial_file_input_output_with_xml_yml.html#gsc.tab=0
class GazeData
{
public:
	GazeData() : id(), time(0), gazePosition(), frameFileName()
	{}
	explicit GazeData(std::string id, double time, cv::Vec2f gazePos, std::string frameFileName) : id(id), time(time), gazePosition(gazePos), frameFileName(frameFileName) // explicit to avoid implicit conversion
	{}
	void write(cv::FileStorage& fs) const //Write serialization for this class
	{
		fs << "{" << "id" << id << "time" << time << "gazePosition" << gazePosition << "frameFileName" << frameFileName  <<"}";
	}
	void read(const cv::FileNode& node) //Read serialization for this class
	{
		id = static_cast<std::string>(node["id"]);
		time = static_cast<double>(node["time"]);
		gazePosition = static_cast<cv::Vec2f>(node["gazePosition"]);
		id = static_cast<std::string>(node["frameFileName"]);
	}
public:   // Data Members
	std::string id; //really needed?
	double time; //use cpu clock time
	cv::Vec2f gazePosition;
	std::string frameFileName;
};

//These write and read functions must be defined for the serialization in FileStorage to work
static void write(cv::FileStorage& fs, const std::string&, const GazeData& x)
{
	x.write(fs);
}

static void read(const cv::FileNode& node, GazeData& x, const GazeData& default_value = GazeData()) {
	if (node.empty())
		x = default_value;
	else
		x.read(node);
}

// This function will print our custom class to the console
static std::ostream& operator<<(std::ostream& out, const GazeData& m)
{
	out << "{ id = " << m.id << ", ";
	out << "time = " << m.time << ", ";
	out << "gazePosition = " << m.gazePosition << ", ";
	out << "frameFileName = " << m.frameFileName << "}";
	return out;
}
