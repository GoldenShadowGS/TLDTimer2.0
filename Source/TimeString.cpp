#include "PCH.h"
#include "TimeString.h"

TimeString::TimeString()
{
	FormatArray();
}

BOOL TimeString::Add(char value)
{
	if (rawIndex < maxRawSize && !(value == '0' && rawIndex == 0))
	{
		rawArray[rawIndex] = value;
		rawIndex++;
		FormatArray();
		return TRUE;
	}
	return FALSE;
}

BOOL TimeString::Back()
{
	if (rawIndex > 0)
	{
		rawIndex--;
		rawArray[rawIndex] = 0;
		FormatArray();
		return TRUE;
	}
	return FALSE;
}

BOOL TimeString::Clear()
{
	if (rawIndex != 0)
	{
		for (int i = 0; i < maxRawSize; i++)
		{
			rawArray[i] = 0;
		}
		rawIndex = 0;
		FormatArray();
		return TRUE;
	}
	return FALSE;
}

void TimeString::Set(INT64 ms)
{
	INT64 tenths = ms / 100;
	INT64 Minutes = (ms / 1000) % 60;
	INT64 Hours = (ms / 60000) % 24;
	INT64 Days = ms / 1440000;

	char digits[maxRawSize] =
	{
		((Days / 10000) % 10) + 48,
		((Days / 1000) % 10) + 48,
		((Days / 100) % 10) + 48,
		((Days / 10) % 10) + 48,
		(Days % 10) + 48,
		((Hours / 10) % 10) + 48,
		(Hours % 10) + 48,
		((Minutes / 10) % 10) + 48,
		(Minutes % 10) + 48,
		(tenths % 10) + 48,
	};

	int count = CountLeadingZeroes(digits);
	int j = count;
	for (int i = 0; i < maxRawSize - count; i++)
	{
		rawArray[i] = digits[j++];
	}

	rawIndex = maxRawSize - count;
	FormatArray();
}

INT64 TimeString::GetTime()
{
	auto getDigitTime = [] (int index)
		{
			switch (index)
			{
			case 0:
				return 14400000000LL; // Days Thousands
			case 1:
				return 1440000000LL; // Days Thousands
			case 2:
				return 144000000LL; // Days Hundreds
			case 3:
				return 14400000LL; // Days Tens
			case 4:
				return 1440000LL; // Days Ones
			case 5:
				return 600000LL; // Hours Tens
			case 6:
				return 60000LL; // Hours Ones
			case 7:
				return 10000LL; // Minutes Tens
			case 8:
				return 1000LL; // Minutes Ones
			case 9:
				return 100LL; // Tenths
			default:
				return 0LL;
			}
		};

	INT64 result = 0;
	int startindex = maxRawSize - rawIndex;

	for (int i = 0; i < rawIndex; i++)
	{
		result += (rawArray[i] - 48LL) * getDigitTime(startindex + i);
	}
	return result;
}

int TimeString::CountLeadingZeroes(char* digits)
{
	int count = 0;
	for (int i = 0; i < maxRawSize; i++)
	{
		if (digits[i] == '0')
			count++;
		else
			break;
	}
	return count;
}

void TimeString::FormatArray()
{
	int sourceindex = 0;
	formatIndex = 0;
	if (rawIndex == 0)
	{
		formatArray[0] = '0';
		formatArray[1] = '.';
		formatArray[2] = '0';
		formatIndex = 3;
	}
	while (sourceindex < rawIndex)
	{
		if (rawIndex - sourceindex == 1)
		{
			if (sourceindex == 0)
			{
				formatArray[formatIndex] = '0';
				formatIndex++;
			}
			formatArray[formatIndex] = '.';
			formatArray[formatIndex + 1] = rawArray[sourceindex];
			formatIndex += 2;
			sourceindex++;
		}
		else if ((rawIndex - sourceindex == 3) && sourceindex != 0) // insert colons between hours:minuutes and minutes:seconds
		{
			formatArray[formatIndex] = ':';
			formatArray[formatIndex + 1] = rawArray[sourceindex];
			sourceindex++;
			formatIndex += 2;
		}
		else if ((rawIndex - sourceindex == 5) && sourceindex != 0) // insert space after days
		{
			formatArray[formatIndex] = 'd';
			formatArray[formatIndex + 1] = ' ';
			formatArray[formatIndex + 2] = rawArray[sourceindex];
			sourceindex++;
			formatIndex += 3;
		}
		else
		{
			formatArray[formatIndex] = rawArray[sourceindex];
			sourceindex++;
			formatIndex++;
		}
	}
	formatArray[formatIndex] = 0;
	assert(formatIndex <= maxFormatSize);
}
