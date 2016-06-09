#include "stdafx.h"
#include "GazeData.h"

gt::GazeData::GazeData(): /*id(), */time(0), gazePosition(), frameFileName()
{
}

gt::GazeData::GazeData(double time, cv::Point gazePos, std::string frameFileName): /*id(id),*/ time(time), gazePosition(gazePos), frameFileName(frameFileName)
{
}

void gt::GazeData::write(cv::FileStorage& fs) const
{
	fs << "{" << /*"id" << id <<*/ "time" << time << "gazePosition" << gazePosition << "frameFileName" << frameFileName << "}";
}

void gt::GazeData::read(const cv::FileNode& node)
{
	//id = static_cast<std::string>(node["id"]);
	time = static_cast<double>(node["time"]);
	node["gazePosition"] >> gazePosition;
	frameFileName = static_cast<std::string>(node["frameFileName"]);
}

void gt::write(cv::FileStorage& fs, const std::string&, const GazeData& x)
{
	x.write(fs);
}

void gt::read(const cv::FileNode& node, GazeData& x, const GazeData& default_value)
{
	if (node.empty())
		x = default_value;
	else
		x.read(node);
}

std::ostream& gt::operator<<(std::ostream& out, const GazeData& m)
{
	//out << "{ id = " << m.id << ", ";
	out << "{ time = " << m.time << ", ";
	out << "gazePosition = " << m.gazePosition << ", ";
	out << "frameFileName = " << m.frameFileName << "}";
	return out;
}
