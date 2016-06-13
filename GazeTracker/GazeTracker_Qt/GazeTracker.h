#pragma once

class GazeTracker
{
public:
	GazeTracker(int argc, char* argv[]);
	~GazeTracker();

	void StartApplication() const;
	void StopApplication() const;
	bool IsDetecting() const;
	int exec() const;

private:
	QApplication qApplication;
};

