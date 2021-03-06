//#include "win32.c"
#include "logger.h"
#include "frame.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lightwindows.h"

FrameRecord i_CurrentFrame;
FrameRecord i_PastFrame;
extern f32 g_TimeFactor;
extern bool g_Running;

static inline f64 computeMS(s64 start, s64 end)
{
	f32 timeFactor = g_TimeFactor;
	f32 subtraction = (f32)(end - start);
	f32 result = subtraction * timeFactor;
	return result;
}

static inline void logFrametimeInfo(float* currentFrameMiliseconds)
{
	static u64 frameCount = 0;
	char** ppPrintBuffer = getPointerToPrintBuffer();
	(*ppPrintBuffer) += sprintf((*ppPrintBuffer), "[[FRAME %llu]] : [TOTAL] %.02f ms. [CPU] %.02f ms (%.02f%%). [GPU] %.02f ms (%.02f%%).\n",
		++frameCount,
		currentFrameMiliseconds[TIME_FRAME_TOTAL],
		currentFrameMiliseconds[TIME_FRAME_CPU],
		(currentFrameMiliseconds[TIME_FRAME_CPU] / currentFrameMiliseconds[TIME_FRAME_TOTAL]) * 100.0f,
		currentFrameMiliseconds[TIME_FRAME_GPU],
		(currentFrameMiliseconds[TIME_FRAME_GPU] / currentFrameMiliseconds[TIME_FRAME_TOTAL]) * 100.0f
	);
}

static inline void printFrameStringBuffer(LOG_OUPUT_TYPE outputType)
{
	char** pBuffer = getPointerToPrintBuffer();
	const char* printBuffer = getPrintBuffer();
	u64 bytesToWrite = (*pBuffer) - printBuffer;
	switch (outputType)
	{
		case (LOG_STDOUT):
		{
			u64 bytesWritten = fwrite(printBuffer, 1, bytesToWrite, stdout);
			assert(bytesWritten == bytesToWrite);
		} break;
		default:
			break;
	}

}

static inline void resetFrameStringBuffer(void)
{
	*getPointerToPrintBuffer() = (char*) getPrintBuffer();
}

void consumePrintBuffer(LOG_OUPUT_TYPE type)
{
	if (g_Running)
	{
		f32 currentFrameMiliseconds[TIME_FRAME_ELEMENT_COUNT];
		// TODO: VECTORIZE
		for (s32 i = 0; i < TIME_FRAME_ELEMENT_COUNT; i++)
		{
			s64 start = i_CurrentFrame.record[i].start;
			s64 end = i_CurrentFrame.record[i].end;
			currentFrameMiliseconds[i] = computeMS(start, end);
		}
		currentFrameMiliseconds[TIME_FRAME_CPU] = currentFrameMiliseconds[TIME_FRAME_TOTAL] - currentFrameMiliseconds[TIME_FRAME_GPU];

		logFrametimeInfo(currentFrameMiliseconds);
	}

	printFrameStringBuffer(type);
	resetFrameStringBuffer();
}
