#include "EnemyManager.h"
#include "SpotLights.h"
#include "PointLights.h"
#include "ShadowSpotLights.h"
#include "Game/Stage/Stage.h"
#include "GlobalVariables.h"

EnemyManager::EnemyManager() {

}

EnemyManager::~EnemyManager() {

}

void EnemyManager::Initialize(PointLights* pointLight, SpotLights* spotLight, ShadowSpotLights* shadowSpotLight) {

	pointLights_ = pointLight;
	spotLights_ = spotLight;
	shadowSpotLights_ = shadowSpotLight;

}

void EnemyManager::Update(const Vector3& playerPosition) {
	for (auto& enemy : nEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Update(playerPosition);
	}
	for (auto& enemy : nLightEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Update(playerPosition);
	}
	for (auto& enemy : tEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Update(playerPosition);
	}
	for (auto& enemy : sLightEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Update(playerPosition);
	}
}

void EnemyManager::OnCollisionPlayer(Collider& collider, const PlayerDate& date) {
	for (auto& enemy : nEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->OnCollision(collider, date);
	}
	for (auto& enemy : nLightEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->OnCollision(collider, date);
	}
	for (auto& enemy : tEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->OnCollision(collider, date);
	}
}

void EnemyManager::DrawImGui() {
#ifdef _DEBUG
	if (ImGui::BeginMenu("Enemis")) {
		if (ImGui::BeginMenu("NormalEnemy")) {
			if (ImGui::Button("Create")) {
				nEnemis_.emplace_back(std::make_unique<NormalEnemy>())->Initialize(Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
			}
			// 要素数確認
			ImGui::Text("ElementCount = %d", nEnemis_.size());
			for (int i = 0; i < nEnemis_.size(); i++) {
				if (ImGui::TreeNode(("nEnemyNumber : " + std::to_string(i)).c_str())) {
					nEnemis_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						nEnemis_.erase(nEnemis_.begin() + i);
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("NormalLightEnemy")) {
			if (ImGui::Button("Create")) {
				nLightEnemis_.emplace_back(std::make_unique<NormalLightEnemy>())->Initialize(Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
			}
			// 要素数確認
			ImGui::Text("ElementCount = %d", nLightEnemis_.size());
			for (int i = 0; i < nLightEnemis_.size(); i++) {
				if (ImGui::TreeNode(("nlEnemyNumber : " + std::to_string(i)).c_str())) {
					nLightEnemis_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						nLightEnemis_.erase(nLightEnemis_.begin() + i);
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("ThornEnemy")) {
			if (ImGui::Button("Create")) {
				tEnemis_.emplace_back(std::make_unique<ThornEnemy>())->Initialize(Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
			}
			// 要素数確認
			ImGui::Text("ElementCount = %d", tEnemis_.size());
			for (int i = 0; i < tEnemis_.size(); i++) {
				if (ImGui::TreeNode(("tEnemyNumber : " + std::to_string(i)).c_str())) {
					tEnemis_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						tEnemis_.erase(tEnemis_.begin() + i);
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("StandLightEnemy")) {
			if (ImGui::Button("Create")) {
				sLightEnemis_.emplace_back(std::make_unique<StandLightEnemy>())->Initialize(Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
			}
			// 要素数確認
			ImGui::Text("ElementCount = %d", sLightEnemis_.size());
			for (int i = 0; i < sLightEnemis_.size(); i++) {
				if (ImGui::TreeNode(("sLightEnemyNumber : " + std::to_string(i)).c_str())) {
					sLightEnemis_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						sLightEnemis_.erase(sLightEnemis_.begin() + i);
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
#endif // _DEBUG
}

void EnemyManager::Save(const char* itemName) {
	GlobalVariables* global = GlobalVariables::GetInstance();

	global->SetValue(itemName, "NormalEnemyConfirmation" + std::string(), static_cast<int>(nEnemis_.size()));
	for (uint32_t index = 0u; index < static_cast<uint32_t>(nEnemis_.size()); index++) {
		global->SetValue(itemName, ("NormalEnemyNumber : " + std::to_string(index) + " : Scale").c_str(), nEnemis_[index]->GetWorldTransform()->scale_);
		global->SetValue(itemName, ("NormalEnemyNumber : " + std::to_string(index) + " : Rotate").c_str(), nEnemis_[index]->GetWorldTransform()->quaternion_);
		global->SetValue(itemName, ("NormalEnemyNumber : " + std::to_string(index) + " : Translate").c_str(), nEnemis_[index]->GetWorldTransform()->translation_);
	}

	global->SetValue(itemName, "NormalLightEnemyConfirmation" + std::string(), static_cast<int>(nLightEnemis_.size()));
	for (uint32_t index = 0u; index < static_cast<uint32_t>(nLightEnemis_.size()); index++) {
		global->SetValue(itemName, ("NormalLightEnemyNumber : " + std::to_string(index) + " : Scale").c_str(), nLightEnemis_[index]->GetWorldTransform()->scale_);
		global->SetValue(itemName, ("NormalLightEnemyNumber : " + std::to_string(index) + " : Rotate").c_str(), nLightEnemis_[index]->GetWorldTransform()->quaternion_);
		global->SetValue(itemName, ("NormalLightEnemyNumber : " + std::to_string(index) + " : Translate").c_str(), nLightEnemis_[index]->GetWorldTransform()->translation_);
	}

	global->SetValue(itemName, "ThornEnemyConfirmation" + std::string(), static_cast<int>(tEnemis_.size()));
	for (uint32_t index = 0u; index < static_cast<uint32_t>(tEnemis_.size()); index++) {
		global->SetValue(itemName, ("ThornEnemyNumber : " + std::to_string(index) + " : Scale").c_str(), tEnemis_[index]->GetWorldTransform()->scale_);
		global->SetValue(itemName, ("ThornEnemyNumber : " + std::to_string(index) + " : Rotate").c_str(), tEnemis_[index]->GetWorldTransform()->quaternion_);
		global->SetValue(itemName, ("ThornEnemyNumber : " + std::to_string(index) + " : Translate").c_str(), tEnemis_[index]->GetWorldTransform()->translation_);
		global->SetValue(itemName, ("ThornEnemyNumber : " + std::to_string(index) + " : Amplitube").c_str(), tEnemis_[index]->amplitude_);
		global->SetValue(itemName, ("ThornEnemyNumber : " + std::to_string(index) + " : Time").c_str(), tEnemis_[index]->kMaxTime_);
	}

	global->SetValue(itemName, "StandLightEnemyConfirmation" + std::string(), static_cast<int>(sLightEnemis_.size()));
	for (uint32_t index = 0u; index < static_cast<uint32_t>(sLightEnemis_.size()); index++) {
		global->SetValue(itemName, ("StandLightEnemyNumber : " + std::to_string(index) + " : Scale").c_str(), sLightEnemis_[index]->GetWorldTransform()->scale_);
		global->SetValue(itemName, ("StandLightEnemyNumber : " + std::to_string(index) + " : Rotate").c_str(), sLightEnemis_[index]->GetWorldTransform()->quaternion_);
		global->SetValue(itemName, ("StandLightEnemyNumber : " + std::to_string(index) + " : Translate").c_str(), sLightEnemis_[index]->GetWorldTransform()->translation_);
		global->SetValue(itemName, ("StandLightEnemyNumber : " + std::to_string(index) + " : MaxTime").c_str(), static_cast<int>(sLightEnemis_[index]->kMaxTime_));
		global->SetValue(itemName, ("StandLightEnemyNumber : " + std::to_string(index) + " : FlashCount").c_str(), static_cast<int>(sLightEnemis_[index]->kMaxFlashCount_));
		global->SetValue(itemName, ("StandLightEnemyNumber : " + std::to_string(index) + " : Interval").c_str(), static_cast<int>(sLightEnemis_[index]->kInterval_));

	}

}

void EnemyManager::Load(const std::filesystem::path& loadFile) {
	GlobalVariables* global = GlobalVariables::GetInstance();
	std::string itemName = loadFile.string();

	int num = global->GetIntValue(itemName, "NormalEnemyConfirmation");
	if (!nEnemis_.empty()) { nEnemis_.clear(); }
	for (int i = 0; i < num; i++) {
		Vector3 scale = global->GetVector3Value(itemName, ("NormalEnemyNumber : " + std::to_string(i) + " : Scale").c_str());
		Quaternion rotate = global->GetQuaternionValue(itemName, ("NormalEnemyNumber : " + std::to_string(i) + " : Rotate").c_str());
		Vector3 trans = global->GetVector3Value(itemName, ("NormalEnemyNumber : " + std::to_string(i) + " : Translate").c_str());
		auto& enemy = nEnemis_.emplace_back(std::make_unique<NormalEnemy>());
		enemy->Initialize(scale, rotate, trans);
	}

	num = global->GetIntValue(itemName, "NormalLightEnemyConfirmation");
	if (!nLightEnemis_.empty()) { nLightEnemis_.clear(); }
	for (int i = 0; i < num; i++) {
		Vector3 scale = global->GetVector3Value(itemName, ("NormalLightEnemyNumber : " + std::to_string(i) + " : Scale").c_str());
		Quaternion rotate = global->GetQuaternionValue(itemName, ("NormalLightEnemyNumber : " + std::to_string(i) + " : Rotate").c_str());
		Vector3 trans = global->GetVector3Value(itemName, ("NormalLightEnemyNumber : " + std::to_string(i) + " : Translate").c_str());
		auto& enemy = nLightEnemis_.emplace_back(std::make_unique<NormalLightEnemy>());
		enemy->Initialize(scale, rotate, trans);
		//enemy->SetLight(shadowSpotLights_, index);
	}

	num = global->GetIntValue(itemName, "ThornEnemyConfirmation");
	if (!tEnemis_.empty()) { tEnemis_.clear(); }
	for (int i = 0; i < num; i++) {
		Vector3 scale = global->GetVector3Value(itemName, ("ThornEnemyNumber : " + std::to_string(i) + " : Scale").c_str());
		Quaternion rotate = global->GetQuaternionValue(itemName, ("ThornEnemyNumber : " + std::to_string(i) + " : Rotate").c_str());
		Vector3 trans = global->GetVector3Value(itemName, ("ThornEnemyNumber : " + std::to_string(i) + " : Translate").c_str());
		Vector3 amplitude = global->GetVector3Value(itemName, ("ThornEnemyNumber : " + std::to_string(i) + " : Amplitube").c_str());
		int kMaxtime = global->GetIntValue(itemName, ("ThornEnemyNumber : " + std::to_string(i) + " : Time").c_str());
		auto& enemy = tEnemis_.emplace_back(std::make_unique<ThornEnemy>());
		enemy->Initialize(scale, rotate, trans);
		enemy->SetState(amplitude, kMaxtime);
	}

	num = global->GetIntValue(itemName, "StandLightEnemyConfirmation");
	if (!sLightEnemis_.empty()) { sLightEnemis_.clear(); }
	for (int i = 0; i < num; i++) {
		Vector3 scale = global->GetVector3Value(itemName, ("StandLightEnemyNumber : " + std::to_string(i) + " : Scale").c_str());
		Quaternion rotate = global->GetQuaternionValue(itemName, ("StandLightEnemyNumber : " + std::to_string(i) + " : Rotate").c_str());
		Vector3 trans = global->GetVector3Value(itemName, ("StandLightEnemyNumber : " + std::to_string(i) + " : Translate").c_str());
		int maxtime = global->GetIntValue(itemName, ("StandLightEnemyNumber : " + std::to_string(i) + " : MaxTime").c_str());
		int flashCount = global->GetIntValue(itemName, ("StandLightEnemyNumber : " + std::to_string(i) + " : FlashCount").c_str());
		int interval = global->GetIntValue(itemName, ("StandLightEnemyNumber : " + std::to_string(i) + " : Interval").c_str());
		auto& enemy = sLightEnemis_.emplace_back(std::make_unique<StandLightEnemy>());
		enemy->Initialize(scale, rotate, trans);
		enemy->SetParameters(static_cast<uint32_t>(maxtime),static_cast<uint32_t>(flashCount),static_cast<uint32_t>(interval));
	}
}

void EnemyManager::Draw() {
	for (auto& enemy : nEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Draw();
	}
	for (auto& enemy : nLightEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Draw();
	}
	for (auto& enemy : tEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Draw();
	}
	for (auto& enemy : sLightEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Draw();
	}
}

void EnemyManager::ShadowDraw() {
	for (auto& enemy : nEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Draw();
	}
	for (auto& enemy : nLightEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Draw();
	}
	for (auto& enemy : tEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Draw();
	}
	for (auto& enemy : sLightEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Draw();
	}
}

void EnemyManager::SpotLightShadowDraw() {
	for (auto& enemy : nEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->EnemyDraw();
	}
	for (auto& enemy : nLightEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->EnemyDraw();
	}
	for (auto& enemy : tEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->EnemyDraw();
	}
}
