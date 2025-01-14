﻿#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <cassert>
#include "Vec3.h"

struct Vec3;

class StringUtility
{
public:
	/// <summary>
	/// マルチバイト文字列からワイド文字列への変換
	/// </summary>
	/// <param name="str">変換元のマルチバイト文字列</param>
	/// <returns>変換後のワイド文字列</returns>
	static std::wstring  StringToWString(const std::string& str)
	{
		// in : 情報側はstring(あらかじめ分かっている)
		// out : 戻り値はwstring(文字列数もわからない)

		// MultiByteToWideCharは、２回呼び出すことを期待されている。
		// １回目の呼び出しは、変換後文字列のサイズを得るため
		// ２回目の呼び出しで、返還後文字列を得る

		std::wstring ret;   // 最終的に返すワイド文字列(最初は何も入っていない)

		// １回目の呼び出し
		int result = MultiByteToWideChar(
			CP_ACP,     // 基の文字列のコードページを書きます(ACPはANSI文字列=ShitfJIS)
			0,          // 変換オプションですが、今回は特に指定なしですので0
			str.data(), // 元の文字列の先頭ポインタ
			static_cast<int>(str.size()), // 元の文字列の文字列数
			nullptr,    // out, optionalなので、nullptrを渡しておく
			0);         // ここでは意味ないので0にしておく

		// 一度目の呼び出しの戻り値が、変換後文字列の文字数になっています。
		assert(result >= 0);

		ret.resize(result); // 受け取れる文字列のバッファを確保する

		result = MultiByteToWideChar(
			CP_ACP,     // 元のコードページ
			0,          // オプション
			str.data(), // 元の文字列へのアドレス
			static_cast<int>(str.size()), // 元文字列の文字数
			ret.data(), // 変換後文字列のアドレス
			static_cast<int>(ret.size()));// 変換後の文字列数

		return ret;
	}

	/// <summary>
	/// UTF_8からワイド文字列への変換
	/// </summary>
	/// <param name="buf">変換元のUTF_8(マルチバイト文字列)</param>
	/// <returns>返還後のワイド文字列</returns>
	static std::wstring Utf8ToWString(const char* const buf, const int size)
	{
		std::wstring ret;
		ret.resize(size);
		MultiByteToWideChar(CP_ACP, 0, buf, -1, ret.data(), size);
		return ret;
	}

	/// <summary>
	/// 分割関数
	/// </summary>
	/// <param name="str">元の文字列</param>
	/// <param name="del">分割する文字</param>
	/// <returns>分割した文字群</returns>
	static std::vector<std::string> Split(std::string& str, char del)
	{
		// 区切り開始位置
		size_t first = 0;
		// 区切り最終位置
		size_t last = str.find_first_of(del);

		std::vector<std::string> result;

		while (first < str.size())
		{
			// 区切ったものを結果に入れていく
			std::string subStr(str, first, last - first);

			result.push_back(subStr);

			// 位置更新
			first = last + 1;
			last = str.find_first_of(del, first);
			if (last == std::string::npos)
			{
				last = str.size();
			}
		}

		return result;
	}
	/// <summary>
	/// 分割関数
	/// </summary>
	/// <param name="str">元の文字列</param>
	/// <param name="del">分割する文字</param>
	/// <returns>分割した文字群</returns>
	static std::vector<std::wstring> Split(std::wstring& str, char del)
	{
		// 区切り開始位置
		size_t first = 0;
		// 区切り最終位置
		size_t last = str.find_first_of(del);

		std::vector<std::wstring> result;

		while (first < str.size())
		{
			// 区切ったものを結果に入れていく
			std::wstring subStr(str, first, last - first);

			result.push_back(subStr);

			// 位置更新
			first = last + 1;
			last = str.find_first_of(del, first);
			if (last == std::string::npos)
			{
				last = str.size();
			}
		}

		return result;
	}

	/// <summary>
	/// Csvで読み込んだVec3データをwstringからVec3に変換する
	/// ※形は"(x/y/z)"にしなければいけない
	/// </summary>
	/// <param name="str">文字列</param>
	/// <returns>Vec3</returns>
	static Vec3 CsvToVec3(const std::wstring& str)
	{
		Vec3 res;

		// x抜き出し
		size_t first = 1;
		size_t last = str.find_first_of('/');
		res.x = std::stof(str.substr(first, last - first));
		// y抜き出し
		first = last + 1;
		last = str.find_first_of('/', first);
		res.y = std::stof(str.substr(first, last - first));
		// z抜き出し
		first = last + 1;
		last = str.find_first_of(')', first);
		res.z = std::stof(str.substr(first, last - first));

		return res;
	}

	/// <summary>
	/// Csvで読み込んだ時間データをwstringからint(フレーム化)に変換する
	/// ※形は"mmm.ss.dd"にしなければならない(mの数は何個でも)
	/// </summary>
	/// <param name="str">文字列</param>
	/// <returns>m * 3600 + s * 60 + d * 0.01 * 60</returns>
	static int CsvToTime(const std::wstring& str)
	{
		int res = 0;

		// 分抜き出し
		size_t first = 0;
		size_t last = str.find_first_of('.');
		res += std::stoi(str.substr(first, last - first)) * 3600;
		// 秒抜き出し
		first = last + 1;
		last = str.find_first_of('.', first);
		res += std::stoi(str.substr(first, last - first)) * 60;
		// 秒以下抜き出し
		first = last + 1;
		res += static_cast<int>(std::stof(str.substr(first)) * 0.01f * 60.0f);

		return res;
	}
};

