#include "stdafx.h"
#include "GazeData.h"

gt::GazeData::GazeData(): GazeData(0,cv::Point(0,0), "")
{
}

gt::GazeData::GazeData(double time, cv::Point gazePos, std::string frameFileName): /*id(id),*/ time(time), gazePosition(gazePos), frameFileName(frameFileName)
{
}

gt::GazeData::GazeData(const GazeData& d) : GazeData(d.time, d.gazePosition, d.frameFileName)
{
}

gt::GazeData gt::GazeData::operator=(const GazeData& d) const
{
	return GazeData(d);
}

void gt::GazeData::write(cv::FileStorage& fs) const
{
	fs << "{" << t.c_str() << time << pos.c_str() << gazePosition << file.c_str() << frameFileName << "}";
}

void gt::GazeData::read(const cv::FileNode& node)
{
	time = static_cast<double>(node[t.c_str()]);
	node[pos.c_str()] >> gazePosition;
	frameFileName = static_cast<std::string>(node[file.c_str()]);
}



void gt::GazeData::write(cv::FileStorage& fs, const std::string&, const GazeData& x)
{
	x.write(fs);
}

void gt::GazeData::read(const cv::FileNode& node, GazeData& x, const GazeData& default_value)
{
	if (node.empty())
		x = default_value;
	else
		x.read(node);
}

std::ostream & gt::operator<<(std::ostream & out, const GazeData & m)
{
	out << "{" << m.t << " = " << m.time << ", ";
	out << m.pos << " = " << m.gazePosition << ", ";
	out << m.file << " = " << m.frameFileName << "}";
	return out;
}
