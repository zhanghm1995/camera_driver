#pragma once

#include<string>
#include<opencv2/core/core.hpp>

using namespace std;

class ImageRectify
{
public:
	ImageRectify();
	virtual ~ImageRectify();

	bool Init(const string &strSettingsFile, const string &prefix);

	void doRectify(cv::Mat &srcImage, cv::Mat &dstImage);

protected:
	cv::Mat M1, M2;
};

