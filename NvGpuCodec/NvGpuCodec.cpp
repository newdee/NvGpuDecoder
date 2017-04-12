// GpuCodec.cpp : 定义控制台应用程序的入口点。
//

#include "NvCodec.h"
#include "MTGpuFramework.h"
#include "MTPlayGround.h"
#include <boost/filesystem.hpp>
#include <iostream>

#define MULTITHREAD_FRAMEWORK

using namespace std;

int main(int argc, char **argv)
{
	BOOST_ASSERT(argc > 1);

#ifdef MULTITHREAD_FRAMEWORK
	MtPlayGround mvs(&argv[1], argc - 1);
#else
	if (!argv[1] || !boost::filesystem::exists(boost::filesystem::path(argv[1])))
	{
		cout<<"invalid h264 file parameter ...."<<endl<<"GpuCodec.exe path_file_to_decode.h264"<<endl;
		return 0;
	}

	/**
	 * Description: create media source & decoder
	 */
	NvCodec::NvCodecInit();
	NvCodec::NvDecoder decoder;
	NvCodec::NvDecoder::CuFrame frame;
	NvCodec::NvMediaSource media(argv[1], &decoder);

	while (!media.Eof())
	{
		if (!decoder.GetFrame(frame))
		{
			Sleep(1);
		}
		else
		{
			/**
			 * Description: process the nv12 frame
			 */

			decoder.PutFrame(frame);
		}
	}
#endif

	return 0;
}