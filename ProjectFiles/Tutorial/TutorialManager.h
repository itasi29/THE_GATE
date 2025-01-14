#pragma once
#include <vector>
#include <list>
#include <string>
#include <memory>
#include "Utility.h"

class FileBase;
class TutorialUI;

struct TutorialData
{
	int novelId = -1;
	int fileNum = -1;
	std::vector<Tuple<int, int>> fileInfo;
};

class TutorialManager
{
private:
	TutorialManager();
	~TutorialManager();

	TutorialManager(const TutorialManager&) = delete;
	TutorialManager(const TutorialManager&&) = delete;
	TutorialManager operator= (const TutorialManager&) = delete;
	TutorialManager operator= (const TutorialManager&&) = delete;

public:
	static TutorialManager& GetInstance();

	void Load();
	void Init();
	void Start(int id);
	void End();
	void Update();
	void Draw() const;

	bool IsNowPlay() const;

private:
	std::list<std::shared_ptr<FileBase>> m_files;
	std::vector<TutorialData> m_dataTable;

	std::shared_ptr<TutorialUI> m_tutorial;
};

