#include "StageManager.h"
#include <list>
#include "Utility.h"
#include "StringUtility.h"
#include "Files.h"
#include "StageDataManager.h"
#include "SaveDataManager.h"
#include "AFile.h"
#include "CheckPoint.h"
#include "Collider/Colliders.h"
#include "Object/HandObject.h"
#include "Object/Glass.h"
#include "Object/Gimmick/Door.h"
#include "Object/Gimmick/Button.h"
#include "Object/Gimmick/Piston.h"
#include "Object/Gate/GateDelete.h"
#include "Object/Gimmick/Turret/Turret.h"
#include "Object/Gimmick/Laser/LaserCatcher.h"
#include "Object/Gimmick/Laser/LaserLaunchPad.h"
#include "Object/Gimmick/MoveFloor/MoveFloorManager.h"

namespace
{
	enum AFileKind : int
	{
		OBJECT,
		GIMMICK,
		MOVE_FLOOR,
		CP,
	};

	const wchar_t* const BASE_PATH = L"Data/Stage/";
	const wchar_t* const MODEL_INFO_PATH = L"StageModel/StageModelInfo";
	const wchar_t* const GIMMICK_INFO_PATH = L"StageGimmick/StageGimmickInfo";
	const wchar_t* const FLOOR_MOVE_INFO_PATH = L"FloorMove/FloorMoveInfo";
	const wchar_t* const CHECK_POINT_INFO_PATH = L"CheckPoint/CheckPointInfo";
	const wchar_t* const MODEL_ID_INFO_PATH = L"StageModelId/StageModelIdInfo";
	const wchar_t* const EXTENSION = L".loc";

	const std::unordered_map<std::string, ObjectTag> OBJECT_TAG =
	{
		{ "Floor", ObjectTag::FLOOR },
		{ "Wall", ObjectTag::WALL },
		{ "SystemWall", ObjectTag::WALL },
		{ "Roof", ObjectTag::ROOF },
		{ "NoGateFloor", ObjectTag::NO_GATE_FLOOR },
		{ "NoGateWall", ObjectTag::NO_GATE_WALL },
		{ "NoGateRoof", ObjectTag::NO_GATE_ROOF },
		{ "PerforatedWall", ObjectTag::PERFORATED_WALL },
		{ "DoorArch", ObjectTag::DOOR_ARCH },
		{ "Exit", ObjectTag::NONE },
		{ "Pillar", ObjectTag::NONE },
		{ "Stairs", ObjectTag::STAIRS },
		{ "Catwalk", ObjectTag::CATWALK },
		{ "CatwalkShort", ObjectTag::CATWALK },
		{ "CatwalkFence", ObjectTag::CATWALK_FENCE },
		{ "CatwalkFenceShort", ObjectTag::CATWALK_FENCE },
		{ "CatwalkPillar", ObjectTag::CATWALK_PILLAR },
	};
	const std::unordered_map<std::string, int> MODEL_ID =
	{
		{ "Floor", M_FLOOR },
		{ "Wall", M_WALL },
		{ "SystemWall", M_WALL },
		{ "Roof", M_ROOF },
		{ "NoGateFloor", M_NO_GATE_FLOOR },
		{ "NoGateWall", M_NO_GATE_WALL },
		{ "NoGateRoof", M_NO_GATE_ROOF },
		{ "PerforatedWall", M_PERFORATED_WALL },
		{ "Door", M_DOOR },
		{ "DoorArch", M_DOOR_ARCH },
		{ "Exit", M_EXIT },
		{ "Pillar", M_PILLAR },
		{ "Stairs", M_STAIRS },
		{ "Catwalk", M_CATWALK },
		{ "CatwalkShort", M_CATWALK_SHORT },
		{ "CatwalkFence", M_CATWALK_FENCE },
		{ "CatwalkFenceShort", M_CATWALK_FENCE_SHORT },
		{ "CatwalkPillar", M_CATWALK_PILLAR },
		{ "HandObj", M_HAND_OBJ },
		{ "Button", M_BUTTON },
		{ "Turret", M_TURRET },
		{ "LaserLaunchPad", M_LASER_LAUNCH_PAD },
		{ "LaserCatcher", M_LASER_CATHCER },
		{ "Piston", M_PISTON },
		{ "Glass", M_GLASS },
	};
}

StageManager::StageManager(const wchar_t* const stageName) :
	m_stageName(stageName),
	m_isClear(false)
{
	m_createFunc[MyEngine::ColKind::BOX] = &StageManager::LoadBoxColInfo;
	m_createFunc[MyEngine::ColKind::SPHERE] = &StageManager::LoadSphereColInfo;
	m_createFunc[MyEngine::ColKind::CAPSULE] = &StageManager::LoadCapsuleColInfo;
}

StageManager::~StageManager()
{
}

void StageManager::AsyncInit()
{
	auto stageNo = StageDataManager::GetInstance().GetStageNo(m_stageName);
	// オブジェクト
	std::wostringstream object;
	object << BASE_PATH << MODEL_INFO_PATH << stageNo << EXTENSION;
	m_afiles.push_back(std::make_shared<AFile>(object.str().c_str()));
	// ギミック
	std::wostringstream gimmick;
	gimmick << BASE_PATH << GIMMICK_INFO_PATH << stageNo << EXTENSION;
	m_afiles.push_back(std::make_shared<AFile>(gimmick.str().c_str()));
	// 動く床
	std::wostringstream move;
	move << BASE_PATH << FLOOR_MOVE_INFO_PATH << stageNo << EXTENSION;
	m_afiles.push_back(std::make_shared<AFile>(move.str().c_str()));
	// チェックポイント
	std::wostringstream cp;
	cp << BASE_PATH << CHECK_POINT_INFO_PATH << stageNo << EXTENSION;
	m_afiles.push_back(std::make_shared<AFile>(cp.str().c_str()));
	// モデル読み込み
	std::list<int> loadList;
	SetUseASyncLoadFlag(false);
	std::wostringstream modelId;
	modelId << BASE_PATH << MODEL_ID_INFO_PATH << stageNo << EXTENSION;
	int handle = FileRead_open(modelId.str().c_str());
	int size = 0;
	FileRead_read(&size, sizeof(size), handle);
	for (int i = 0; i < size; ++i)
	{
		uint8_t tagSize;
		FileRead_read(&tagSize, sizeof(uint8_t), handle);
		std::string tag;
		tag.resize(tagSize);
		FileRead_read(tag.data(), sizeof(char) * tagSize, handle);

		// FloorMoveはスキップする
		if (tag == "FloorMove" || tag == "GateDelete") continue;
		loadList.emplace_back(MODEL_ID.at(tag));
	}
	SetUseASyncLoadFlag(true);
	auto& fileMgr = FileManager::GetInstance();
	for (const auto& id : loadList)
	{
		m_files.push_back(fileMgr.Load(id));
	}
}

void StageManager::Init(Player* player, GateManager* gateMgr)
{
	LoadAndCreateObject(player, gateMgr);
	LoadGimmickLinkInfo();
	LoadFloorMoveInfo();
	LoadCheckPoint();

	for (auto& info : m_afiles) info->Close();
	m_afiles.clear();
}

void StageManager::Update()
{
	for (const auto& obj : m_objs)	obj->Update();
	auto it = std::remove_if(m_objs.begin(), m_objs.end(),
		[](const auto& obj)
		{
			if (!obj->IsExist())
			{
				obj->End();
				return true;
			}
			return false;
		});
	m_objs.erase(it, m_objs.end());
}

void StageManager::Draw() const
{
	for (const auto& obj : m_objs)	obj->Draw();
}

void StageManager::End()
{
	for (auto& obj : m_objs)		obj->End();
	for (auto& cp : m_checkPoints)	cp->End();
}

void StageManager::Restart()
{
	for (auto& obj : m_objs)	obj->Restart();
}

void StageManager::UpdateCheckPoint(int checkNo)
{
	if (m_isClear) return;

	SaveDataManager::GetInstance().UpdateCheckPoint(m_stageName, checkNo);
}

const Vec3& StageManager::GetCheckPoint() const
{
	auto cp = SaveDataManager::GetInstance().GetCpNo(m_stageName);
	return m_checkPoints.at(cp)->GetPos();
}

bool StageManager::CheckClear()
{
	auto cp = SaveDataManager::GetInstance().GetCpNo(m_stageName);
	m_isClear = cp + 1 >= m_checkPoints.size();
	return m_isClear;
}

void StageManager::LoadAndCreateObject(Player* player, GateManager* gateMgr)
{
	auto& file = m_afiles.at(AFileKind::OBJECT);

	int size;
	file->Read(&size, sizeof(int));
	for (int i = 0; i < size; ++i)
	{
		// タグ読み込み
		uint8_t tagSize = 0;
		file->Read(&tagSize, sizeof(uint8_t));
		std::string tag;
		tag.resize(tagSize);
		file->Read(tag.data(), sizeof(char) * tagSize);
		// データ読み込み
		ModelData data;
		file->Read(&data, sizeof(ModelData));

		// 判定データ読み込み
		std::list<Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>> list;
		int colSize = 0;
		file->Read(&colSize, sizeof(int));
		for (int i = 0; i < colSize; ++i)
		{
			MyEngine::ColKind kind;
			file->Read(&kind, sizeof(ColKind));
			const auto& func = m_createFunc.at(kind);
			auto res = (this->*func)(file, data);
			list.emplace_back(Tuple<MyEngine::ColKind, MyEngine::ColliderBase*>{ kind, res });
		}

		// オブジェクト生成
		std::shared_ptr<Object3DBase> obj;
		bool isGravity = false;
		bool isLoadModel = true;
		if (tag == "Turret")
		{
			Vec3 dir;
			file->Read(&dir, sizeof(Vec3));
			auto turret = std::make_shared<Turret>();
			turret->Init(dir, player);
			obj = turret;
			isGravity = true;
		}
		else if (tag == "LaserLaunchPad")
		{
			Vec3 dir;
			int reflectNum = 0;
			file->Read(&dir, sizeof(Vec3));
			file->Read(&reflectNum, sizeof(int));
			auto launchPad = std::make_shared<LaserLaunchPad>(gateMgr, reflectNum);
			launchPad->Init(dir);
			obj = launchPad;
		}
		else if (tag == "GateDelete")
		{
			obj = std::make_shared<GateDelete>(*gateMgr);
			isLoadModel = false;
		}
		else if (tag == "HandObj")
		{
			obj = std::make_shared<HandObject>(*gateMgr);
			isGravity = true;
		}
		else if (tag == "FloorMove")
		{
			obj = std::make_shared<MoveFloorManager>();
			isLoadModel = false;
		}
		else if (tag == "LaserCatcher") obj = std::make_shared<LaserCatcher>();
		else if (tag == "Piston")		obj = std::make_shared<Piston>();
		else if (tag == "Button")		obj = std::make_shared<Button>();
		else if (tag == "Door")			obj = std::make_shared<Door>();
		else if (tag == "Glass")		obj = std::make_shared<Glass>();
		else							obj = std::make_shared<Object3DBase>(MyEngine::Collidable::Priority::STATIC, OBJECT_TAG.at(tag));

		if (isLoadModel) obj->LoadModel(MODEL_ID.at(tag));
		obj->Init(data.pos, data.scale * 0.01f, data.rot, list, isGravity);
		m_objs.emplace_back(obj);
	}
}

void StageManager::LoadGimmickLinkInfo()
{
	auto& file = m_afiles.at(AFileKind::GIMMICK);
	int size;
	file->Read(&size, sizeof(int));
	for (int i = 0; i< size; ++i)
	{
		// 送る側のオブジェクト番号取得
		int gimmickNo;
		file->Read(&gimmickNo, sizeof(int));
		// 受け取る側のオブジェクト番号取得
		int linkNo;
		file->Read(&linkNo, sizeof(int));

		auto gimmick = std::dynamic_pointer_cast<GimmickSendObject>(m_objs.at(gimmickNo));
		auto link = std::dynamic_pointer_cast<GimmickLinkObject>(m_objs.at(linkNo));

		gimmick->SetLinkObject(link.get());
	}
}

void StageManager::LoadFloorMoveInfo()
{
	auto& file = m_afiles.at(AFileKind::MOVE_FLOOR);
	int size;
	file->Read(&size, sizeof(int));

	for (int i = 0; i < size; ++i)
	{
		// ポジション数読み込み
		int num;
		file->Read(&num, sizeof(int));
		// ポジションデータ読み込み
		std::vector<Vec3> posTable(num);
		file->Read(posTable.data(), sizeof(Vec3) * num);
		// オブジェクト番号取得
		int index;
		file->Read(&index, sizeof(int));
		// ループフラグ取得
		bool isLoop;
		file->Read(&isLoop, sizeof(bool));

		auto obj = m_objs.at(index);
		if (obj->GetTag() == ObjectTag::PISTON)
		{
			auto piston = std::dynamic_pointer_cast<Piston>(obj);
			piston->SetMoveRange(posTable[0], posTable[1]);
		}
		else
		{
			auto moveFloorMgr = std::dynamic_pointer_cast<MoveFloorManager>(obj);
			moveFloorMgr->SetCheckPtTable(posTable);
			moveFloorMgr->SetLoop(isLoop);
		}
	}
}

void StageManager::LoadCheckPoint()
{
	auto& file = m_afiles.at(AFileKind::CP);
	int size;
	file->Read(&size, sizeof(int));

	for (int i = 0; i < size; ++i)
	{
		// データ読み込み
		Vec3 pos;
		Quaternion rot;
		float size;
		float radius;

		file->Read(&pos, sizeof(Vec3));
		file->Read(&rot, sizeof(Quaternion));
		file->Read(&size, sizeof(float));
		file->Read(&radius, sizeof(float));

		auto obj = std::make_shared<CheckPoint>(*this, i);
		obj->Init(pos, rot, size, radius);

		bool isLink;
		file->Read(&isLink, sizeof(bool));
		if (isLink)
		{
			int linkNo;
			file->Read(&linkNo, sizeof(int));
			auto link = dynamic_cast<GimmickLinkObject*>(m_objs.at(linkNo).get());
			obj->SetLinkObj(link);
		}

		m_checkPoints.emplace_back(obj);
	}
}

MyEngine::ColliderBase* StageManager::LoadBoxColInfo(const std::shared_ptr<AFile>& file, const ModelData& data)
{
	auto col = new MyEngine::ColliderBox;
	file->Read(&col->center, sizeof(Vec3));
	file->Read(&col->size, sizeof(Vec3));
	file->Read(&col->norm, sizeof(Vec3));
	file->Read(&col->rotation, sizeof(Quaternion));
	file->Read(&col->isTrigger, sizeof(bool));
	return col;
}

MyEngine::ColliderBase* StageManager::LoadSphereColInfo(const std::shared_ptr<AFile>& file, const ModelData& data)
{
	auto col = new MyEngine::ColliderSphere;
	file->Read(&col->center, sizeof(Vec3));
	file->Read(&col->radius, sizeof(float));
	file->Read(&col->isTrigger, sizeof(bool));
	return col;
}

MyEngine::ColliderBase* StageManager::LoadCapsuleColInfo(const std::shared_ptr<AFile>& file, const ModelData& data)
{
	auto col = new MyEngine::ColliderCapsule;
	file->Read(&col->center, sizeof(Vec3));
	file->Read(&col->dir, sizeof(Vec3));
	file->Read(&col->size, sizeof(float));
	file->Read(&col->radius, sizeof(float));
	file->Read(&col->isTrigger, sizeof(bool));
	return col;
}
