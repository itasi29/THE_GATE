#include "TutorialManager.h"
#include <cassert>
#include "Files.h"
#include "TutorialUI.h"

namespace
{
	const wchar_t* const PATH = L"Data/Master/TutorialData.csv";

	enum class FileIndex
	{
		NOVEL_ID,
		FILE_NUM,
		FILE_EFFECTIVE_RANGE,
		FILE_ID
	};
}

TutorialManager::TutorialManager()
{
}

TutorialManager::~TutorialManager()
{
}

TutorialManager& TutorialManager::GetInstance()
{
	static TutorialManager instance;
	return instance;
}

void TutorialManager::Load()
{
	const auto& list = FileManager::GetInstance().LoadCsv(PATH);
	for (auto& item : list)
	{
		TutorialData data;
		data.novelId = std::stoi(item.at(static_cast<int>(FileIndex::NOVEL_ID)));
		data.fileNum = std::stoi(item.at(static_cast<int>(FileIndex::FILE_NUM)));
		data.fileInfo.resize(data.fileNum);
		for (int i = 0; i < data.fileNum; ++i)
		{
			auto& info = data.fileInfo[i];
			int index = i * 2;

			info.t1 = std::stoi(item.at(static_cast<int>(FileIndex::FILE_EFFECTIVE_RANGE) + index));
			info.t2 = std::stoi(item.at(static_cast<int>(FileIndex::FILE_ID) + index));
		}

		m_dataTable.push_back(data);
	}
}

void TutorialManager::Init()
{
	auto& file = FileManager::GetInstance();
	m_files.emplace_back(file.Load(I_PAD_A));
	m_files.emplace_back(file.Load(I_SELECT_OBI));
	m_files.emplace_back(file.Load(I_TUTORIAL_TEST_1));
	m_files.emplace_back(file.Load(I_TUTORIAL_TEST_2));
	m_files.emplace_back(file.Load(I_TUTORIAL_TEST_3));
}

void TutorialManager::Start(int id)
{
	if (m_tutorial)
	{
		assert(false && "既にスタートしています");
		return;
	}
	const auto& data = m_dataTable.at(id);

	m_tutorial = std::make_shared<TutorialUI>();
	m_tutorial->Init(data);
}

void TutorialManager::End()
{
	if (!m_tutorial) return;

	m_tutorial.reset();
	m_tutorial = nullptr;
}

void TutorialManager::Update()
{
	if (!m_tutorial) return;
	m_tutorial->Update();
}

void TutorialManager::Draw() const
{
	if (!m_tutorial) return;
	m_tutorial->Draw();
}

bool TutorialManager::IsNowPlay() const
{
	if (!m_tutorial) return false;
	return !m_tutorial->IsEnd();
}
