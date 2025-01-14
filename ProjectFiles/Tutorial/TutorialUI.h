#pragma once
#include <unordered_map>
#include <memory>
#include <vector>
#include "Utility.h"

struct TutorialData;
class FileBase;

/// <summary>
/// UIとして説明するよう
/// </summary>
class TutorialUI
{
public:
	TutorialUI();
	~TutorialUI();

	void Init(const TutorialData& data);
	void End();
	void Update();
	void Draw() const;

	bool IsEnd() const { return m_isEnd; }

private:
	std::unordered_map<int, std::shared_ptr<FileBase>> m_files;
	std::vector<Tuple<int, int>> m_fileInfo;

	int m_novelId;
	int m_nowFileIndex;
	int m_count;
	int m_waveFrame;

	bool m_isEnd;
};
