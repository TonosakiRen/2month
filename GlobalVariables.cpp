#include "GlobalVariables.h"
#include "ImGuiManager.h"
#include "externals/nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include <assert.h>

using namespace nlohmann;
GlobalVariables* GlobalVariables::GetInstance() {
	static GlobalVariables instance;
	return &instance;
}

void GlobalVariables::CreateGroup(const std::string& groupName) {
	//指定名のオブジェクトがなければ追加する
	datas_[groupName];
}

void GlobalVariables::SaveFile(const std::string& groupName) {

	//グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	//未登録チェック
	assert(itGroup != datas_.end());

	json root;

	root = json::object();

	//jsonオブジェクト登録
	root[groupName] = json::object();

	for (std::map<std::string, Item>::iterator itItem = itGroup->second.begin();
		itItem != itGroup->second.end(); ++itItem) {

		//項目名を取得
		const std::string& itemName = itItem->first;
		//項目の参照を取得
		Item& item = itItem->second;

		//int32_t型の値を保持していれば
		if (std::holds_alternative<int32_t>(item)) {
			//int32_tの値を登録
			root[groupName][itemName] = std::get<int32_t>(item);
		}
		else//float型の値を保持していれば
			if (std::holds_alternative<float>(item)) {
				// floatの値を登録
				root[groupName][itemName] = std::get<float>(item);
			}
			else // Vector3型の値を保持していれば
				if (std::holds_alternative<Vector3>(item)) {
					// float型のjson配列登録
					Vector3 value = std::get<Vector3>(item);
					root[groupName][itemName] = json::array({ value.x, value.y, value.z });
				}
	}

	//ディレクトリがなければ作成する
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directories(kDirectoryPath);
	}

	//書き込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	//書き込むようファイルストリーム
	std::ofstream ofs;
	//ファイルを書き込みように開く
	ofs.open(filePath);

	//ファイルオープン失敗？
	if (ofs.fail()) {
		std::string message = "Failed open data file for write";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}
	//ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();

}

void GlobalVariables::Loadfiles() {
	const std::string directoryPath = kDirectoryPath;
	//ディレクトリがなければスキップする
	if (!std::filesystem::exists(kDirectoryPath)) {
		return;
	}
	std::filesystem::directory_iterator dir_it(directoryPath);
	for (const std::filesystem::directory_entry& entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();
		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		//ファイル読み込み
		LoadFile(filePath.stem().string());
	}
}

void GlobalVariables::LoadFile(const std::string& groupName) {
	//読み込むJSonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	//読み込むようファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込むように開く
	ifs.open(filePath);

	//ファイルオープン失敗？
	if (ifs.fail()) {
		std::string message = "Failed open data file for write";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();

	//グループを検索
	json::iterator itGroup = root.find(groupName);
	//未登録チェック
	assert(itGroup != root.end());

	//各アイテムについて
	for (json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテム名を取得
		const std::string& itemName = itItem.key();
		//int32_t型の値を保持していれば
		if (itItem->is_number_integer()) {
			//int型の値を登録
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		}
		// float型の値を保持していれば
		else if (itItem->is_number_float()) {
			// float型のjson配列登録
			double value = itItem->get<double>();
			SetValue(groupName, itemName, static_cast<float>(value));
		}
		else if (itItem->is_array() && itItem->size() == 3) {
			//float型のjson配列登録
			Vector3 value = { itItem->at(0), itItem->at(1), itItem->at(2) };
			SetValue(groupName, itemName, value);
		}
	}

}

void GlobalVariables::Update() {
	if (!ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar())	return;

	//各グループについて
	for (std::map<std::string, Group>::iterator itGroup = datas_.begin(); itGroup != datas_.end();
		++itGroup) {
		//グループ名を取得
		const std::string& groupName = itGroup->first;
		//グループの参照を取得
		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str()))
			continue;

		//各項目について
		for (std::map<std::string, Item>::iterator itItem = group.begin();
			itItem != group.end(); ++itItem) {

			//項目名を取得
			const std::string& itemName = itItem->first;
			//項目の参照を取得
			Item& item = itItem->second;

			//int32_t型の値を保持していれば
			if (std::holds_alternative<int32_t>(item)) {
				int32_t* ptr = std::get_if<int32_t>(&item);
				ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);
			}
			// float型の値を保持していれば
			else if (std::holds_alternative<float>(item)) {
				float* ptr = std::get_if<float>(&item);
				ImGui::SliderFloat(itemName.c_str(), ptr, 0.0f, 100.0f);
			}
			// Vector3型の値を保持していれば
			else if (std::holds_alternative<Vector3>(item)) {
				Vector3* ptr = std::get_if<Vector3>(&item);
				ImGui::SliderFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), -10.0f, 10.0f);
			}
		}

		//改行
		ImGui::Text("\n");

		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End();
}

void GlobalVariables::SetValue(
	const std::string& groupName, const std::string& key, int32_t value) {
	//グループの参照を取得
	Group& group = datas_[groupName];
	//新しい項目のデータを設定
	Item newItem{};
	newItem = value;
	//設定した項目をstd::mapに追加
	group[key] = newItem;
}

void GlobalVariables::SetValue(
	const std::string& groupName, const std::string& key, float value) {
	// グループの参照を取得
	Group& group = datas_[groupName];
	// 新しい項目のデータを設定
	Item newItem{};
	newItem = value;
	// 設定した項目をstd::mapに追加
	group[key] = newItem;
}

void GlobalVariables::SetValue(
	const std::string& groupName, const std::string& key, const Vector3& value) {
	// グループの参照を取得
	Group& group = datas_[groupName];
	// 新しい項目のデータを設定
	Item newItem{};
	newItem = value;
	// 設定した項目をstd::mapに追加
	group[key] = newItem;
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, int32_t value) {
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);
	// 項目が未登録なら
	if (itGroup != datas_.end()) {
		Group& group = datas_[groupName];
		if (!group.contains(key)) {
			SetValue(groupName, key, value);
		}
	}
}
// 項目の追加(float)
void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, float value) {
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);
	// 項目が未登録なら
	if (itGroup != datas_.end()) {
		Group& group = datas_[groupName];
		if (!group.contains(key)) {
			SetValue(groupName, key, value);
		}
	}
}
// 項目の追加(Vector3)
void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, const Vector3& value) {
	//グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	// 項目が未登録なら
	if (itGroup != datas_.end()) {
		Group& group = datas_[groupName];
		if (!group.contains(key)) {
			SetValue(groupName, key, value);
		}
	}
}

int32_t GlobalVariables::GetIntValue(const std::string& groupName, const std::string& key) const {
#ifdef _DEBUG
	// グループを検索
	const auto& groupIt = datas_.find(groupName);
	// 未登録チェック
	assert(groupIt != datas_.end());
#endif // _DEBUG

	// グループの参照を取得
	const Group& group = datas_.at(groupName);

	assert(group.contains(key));

	const Item& item = group.at(key);
	return std::get<int32_t>(item);
}
float GlobalVariables::GetFloatValue(const std::string& groupName, const std::string& key) const {//グループを検索
#ifdef _DEBUG
	// グループを検索 
	const auto& groupIt = datas_.find(groupName);
	// 未登録チェック
	assert(groupIt != datas_.end());
#endif // _DEBUG
	// グループの参照を取得
	const Group& group = datas_.at(groupName);

	assert(group.contains(key));

	const Item& item = group.at(key);
	return std::get<float>(item);
}
Vector3 GlobalVariables::GetVector3Value(const std::string& groupName, const std::string& key) const {//グループを検索
#ifdef _DEBUG
	// グループを検索
	const auto& groupIt = datas_.find(groupName);
	// 未登録チェック
	assert(groupIt != datas_.end());
#endif // _DEBUG
	// グループの参照を取得
	const Group& group = datas_.at(groupName);

	assert(group.contains(key));

	const Item& item = group.at(key);
	return std::get<Vector3>(item);
}