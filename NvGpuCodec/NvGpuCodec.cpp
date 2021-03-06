// GpuCodec.cpp : 定义控制台应用程序的入口点。
//

#include <boost/filesystem.hpp>
#include <boost/atomic.hpp>
#include <iostream>
#include <unordered_map>
#include "NvCodec.h"
#include "MTGpuFramework.h"
#include "MTPlayGround.h"

/* pipeline simulation or only decoding */
#define PIPELINE_SIMULATE_FRAMEWORK

using namespace std;

void OnBatchData(ISmartFramePtr *batch, unsigned int len, void *invoker)
{
	static int idx = 0;
	//cout << "batch index : " << idx++ << endl;
	for (int i = 0; i < len; i++)
	{
		cout << batch[i]->Tid() << ", frame no = " << batch[i]->FrameNo() << ", time = " << batch[i]->Timestamp() << ", ref count" << batch[i]->GetRef() << endl;
		ISmartFramePtr frame(batch[i]);
	}
}

int main(int argc, char **argv)
{
	BOOST_ASSERT(argc > 1);

#ifdef PIPELINE_SIMULATE_FRAMEWORK

	cout << "start up argument " << argc << endl;

	CUcontext ctx;
	cuInit(0);
	cuCtxCreate(&ctx, 0, 0);

	MtPlayGround mvs(OnBatchData, NULL, ctx, true/*loop*/);
	for (int i = 1; i < argc; i++)
	{
		mvs.AddVideo(argv[i]);
	}
	getchar();

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
	NvCodec::CuFrame frame;
	NvCodec::NvMediaSource media(argv[1], &decoder);

	while (!media.Eof())
	{
		if (!decoder.GetFrame(frame))
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
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
