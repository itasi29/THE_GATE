#pragma once
#include "Input.h"
#include "CommandList.h"

class CursorUtility
{
public:
	static bool CursorUp(int& index, int max, const char* command = Command::BTN_UP, bool isRepeat = false, int repeatIntreval = 5)
	{
		auto& input = Input::GetInstance();

		RepeatKind kind;
		if (input.IsRepeat(command, kind, repeatIntreval))
		{
			if (kind == RepeatKind::TRIGGER)
			{
				index = (index - 1 + max) % max;
				return true;
			}
			else if (isRepeat)
			{
				--index;
				if (index < 0)
				{
					index = 0;
					return false;
				}
				return true;
			}
		}

		return false;
	}
	static bool CursorDown(int& index, int max, const char* command = Command::BTN_DOWN, bool isRepeat = false, int repeatIntreval = 5)
	{
		auto& input = Input::GetInstance();

		RepeatKind kind;
		if (input.IsRepeat(command, kind, repeatIntreval))
		{
			if (kind == RepeatKind::TRIGGER)
			{
				index = (index + 1) % max;
				return true;
			}
			else if (isRepeat)
			{
				++index;
				if (index > max - 1)
				{
					index = max - 1;
					return false;
				}
				return true;
			}
		}
		
		return false;
	}
	template<class T>
	static bool CursorUp(int& index, T max, const char* command = Command::BTN_UP, bool isRepeat = false, int repeatIntreval = 5)
	{
		return CursorUp(index, static_cast<int>(max), command, isRepeat, repeatIntreval);
	}
	template<class T>
	static bool CursorDown(int& index, T max, const char* command = Command::BTN_DOWN, bool isRepeat = false, int repeatIntreval = 5)
	{
		return CursorDown(index, static_cast<int>(max), command, isRepeat, repeatIntreval);
	}
};
