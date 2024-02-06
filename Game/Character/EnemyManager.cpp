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
	TrapButton::masterNumber_ = 0u;
}

template<typename Container>
void TempEnemyUpdate(Container& container,const Vector3& playerPosition, uint32_t& num) {
	for (uint32_t index = 0u; index < container.size(); index++) {
		if (!container.at(index)->GetIsAlive()) {
			// 要素を削除したため、indexを進めない
			container.erase(container.begin() + index);
			index--;
			continue;
		}
		container.at(index)->Update(playerPosition);
		num++;
	}
}

void EnemyManager::Update(const Vector3& playerPosition) {
	remainderNumber_ = 0u;
	uint32_t gomi = 0u;
	TempEnemyUpdate(nEnemis_,playerPosition, remainderNumber_);
	TempEnemyUpdate(nLightEnemis_,playerPosition, remainderNumber_);
	TempEnemyUpdate(tEnemis_,playerPosition, gomi);
	TempEnemyUpdate(sLightEnemis_,playerPosition, gomi);
	TempEnemyUpdate(cEnemis_,playerPosition, remainderNumber_);
	TempEnemyUpdate(coins_,playerPosition, gomi);
	TempEnemyUpdate(hearts_,playerPosition, gomi);
	
	for (uint32_t index = 0u; index < nSpawners_.size(); index++) {
		if (!nSpawners_.at(index)->GetIsAlive()) {
			// 要素を削除したため、indexを進めない
			nSpawners_.erase(nSpawners_.begin() + index);
			index--;
			continue;
		}
		if (nSpawners_.at(index)->UpdateSpawn(playerPosition)) {
			auto& enemy = nEnemis_.emplace_back(std::make_unique<NormalEnemy>());
			auto& spawn = nSpawners_.at(index)->GetSRT();
			enemy->Initialize(spawn.scale, spawn.rotate, spawn.translate);
		}
		remainderNumber_++;
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
	for (auto& enemy : cEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->OnCollision(collider, date);
	}
	for (auto& coin : coins_) {
		if (!coin->GetIsAlive()) { continue; }
		coin->OnCollision(collider, date);
	}
	for (auto& heart : hearts_) {
		if (!heart->GetIsAlive()) { continue; }
		heart->OnCollision(collider, date);
	}
	for (auto& spawn : nSpawners_) {
		if (!spawn->GetIsAlive()) { continue; }
		spawn->OnCollision(collider, date);
	}

}

void EnemyManager::OnCollisionStage(Collider& collider) {
	for (auto& enemy : nEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->PushBackCollision(collider);
	}
	for (auto& enemy : nLightEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->PushBackCollision(collider);
	}
	for (auto& enemy : tEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->PushBackCollision(collider);
	}
	for (auto& enemy : cEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->PushBackCollision(collider);
	}
	for (auto& spawn : nSpawners_) {
		if (!spawn->GetIsAlive()) { continue; }
		spawn->PushBackCollision(collider);
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
		if (ImGui::BeginMenu("CannonEnemy")) {
			if (ImGui::Button("Create")) {
				cEnemis_.emplace_back(std::make_unique<CannonEnemy>())->Initialize(Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
			}
			// 要素数確認
			ImGui::Text("ElementCount = %d", cEnemis_.size());
			for (int i = 0; i < cEnemis_.size(); i++) {
				if (ImGui::TreeNode(("cEnemyNumber : " + std::to_string(i)).c_str())) {
					cEnemis_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						cEnemis_.erase(cEnemis_.begin() + i);
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Coin")) {
			if (ImGui::Button("Create")) {
				coins_.emplace_back(std::make_unique<Coin>())->Initialize(Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
			}
			// 要素数確認
			ImGui::Text("ElementCount = %d", coins_.size());
			for (int i = 0; i < coins_.size(); i++) {
				if (ImGui::TreeNode(("coinNumber : " + std::to_string(i)).c_str())) {
					coins_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						coins_.erase(coins_.begin() + i);
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Heart")) {
			if (ImGui::Button("Create")) {
				hearts_.emplace_back(std::make_unique<Heart>())->Initialize(Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
			}
			// 要素数確認
			ImGui::Text("ElementCount = %d", hearts_.size());
			for (int i = 0; i < hearts_.size(); i++) {
				if (ImGui::TreeNode(("heartNumber : " + std::to_string(i)).c_str())) {
					hearts_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						hearts_.erase(hearts_.begin() + i);
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("NormalSpawner")) {
			if (ImGui::Button("Create")) {
				nSpawners_.emplace_back(std::make_unique<NormalSpawner>())->Initialize(Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
			}
			// 要素数確認
			ImGui::Text("ElementCount = %d", nSpawners_.size());
			for (int i = 0; i < nSpawners_.size(); i++) {
				if (ImGui::TreeNode(("nSpawnerNumber : " + std::to_string(i)).c_str())) {
					nSpawners_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						nSpawners_.erase(nSpawners_.begin() + i);
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

	global->SetValue(itemName, "CannonEnemyConfirmation" + std::string(), static_cast<int>(cEnemis_.size()));
	for (uint32_t index = 0u; index < static_cast<uint32_t>(cEnemis_.size()); index++) {
		global->SetValue(itemName, ("CannonEnemyNumber : " + std::to_string(index) + " : Scale").c_str(), cEnemis_[index]->GetWorldTransform()->scale_);
		global->SetValue(itemName, ("CannonEnemyNumber : " + std::to_string(index) + " : Rotate").c_str(), cEnemis_[index]->GetWorldTransform()->quaternion_);
		global->SetValue(itemName, ("CannonEnemyNumber : " + std::to_string(index) + " : Translate").c_str(), cEnemis_[index]->GetWorldTransform()->translation_);
		global->SetValue(itemName, ("CannonEnemyNumber : " + std::to_string(index) + " : Interval").c_str(), static_cast<int>(cEnemis_[index]->kInterval_));
		global->SetValue(itemName, ("CannonEnemyNumber : " + std::to_string(index) + " : BulletSpeed").c_str(), cEnemis_[index]->bulletSpeed_);
	}

	global->SetValue(itemName, "CoinConfirmation" + std::string(), static_cast<int>(coins_.size()));
	for (uint32_t index = 0u; index < static_cast<uint32_t>(coins_.size()); index++) {
		global->SetValue(itemName, ("CoinNumber : " + std::to_string(index) + " : Scale").c_str(), coins_[index]->GetWorldTransform()->scale_);
		global->SetValue(itemName, ("CoinNumber : " + std::to_string(index) + " : Rotate").c_str(), coins_[index]->GetWorldTransform()->quaternion_);
		global->SetValue(itemName, ("CoinNumber : " + std::to_string(index) + " : Translate").c_str(), coins_[index]->GetWorldTransform()->translation_);
	}

	global->SetValue(itemName, "HeartConfirmation" + std::string(), static_cast<int>(hearts_.size()));
	for (uint32_t index = 0u; index < static_cast<uint32_t>(hearts_.size()); index++) {
		global->SetValue(itemName, ("HeartNumber : " + std::to_string(index) + " : Scale").c_str(), hearts_[index]->GetWorldTransform()->scale_);
		global->SetValue(itemName, ("HeartNumber : " + std::to_string(index) + " : Rotate").c_str(), hearts_[index]->GetWorldTransform()->quaternion_);
		global->SetValue(itemName, ("HeartNumber : " + std::to_string(index) + " : Translate").c_str(), hearts_[index]->GetWorldTransform()->translation_);
	}
	
	global->SetValue(itemName, "SpawnerConfirmation" + std::string(), static_cast<int>(nSpawners_.size()));
	for (uint32_t index = 0u; index < static_cast<uint32_t>(nSpawners_.size()); index++) {
		global->SetValue(itemName, ("SpawnerNumber : " + std::to_string(index) + " : Scale").c_str(), nSpawners_[index]->GetWorldTransform()->scale_);
		global->SetValue(itemName, ("SpawnerNumber : " + std::to_string(index) + " : Rotate").c_str(), nSpawners_[index]->GetWorldTransform()->quaternion_);
		global->SetValue(itemName, ("SpawnerNumber : " + std::to_string(index) + " : Translate").c_str(), nSpawners_[index]->GetWorldTransform()->translation_);
		global->SetValue(itemName, ("SpawnerNumber : " + std::to_string(index) + " : Interval").c_str(), nSpawners_[index]->GetSRT().interval);
		global->SetValue(itemName, ("SpawnerNumber : " + std::to_string(index) + " : MaxPop").c_str(), nSpawners_[index]->GetSRT().MaxPopEnemy);
		global->SetValue(itemName, ("SpawnerNumber : " + std::to_string(index) + " : hp").c_str(), nSpawners_[index]->GetSRT().hp);
	}


}

void EnemyManager::Load(const std::filesystem::path& loadFile) {
	GlobalVariables* global = GlobalVariables::GetInstance();
	std::string itemName = loadFile.string();
	global->LoadFile(itemName);

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

	num = global->GetIntValue(itemName, "CannonEnemyConfirmation");
	if (!cEnemis_.empty()) { cEnemis_.clear(); }
	for (int i = 0; i < num; i++) {
		Vector3 scale = global->GetVector3Value(itemName, ("CannonEnemyNumber : " + std::to_string(i) + " : Scale").c_str());
		Quaternion rotate = global->GetQuaternionValue(itemName, ("CannonEnemyNumber : " + std::to_string(i) + " : Rotate").c_str());
		Vector3 trans = global->GetVector3Value(itemName, ("CannonEnemyNumber : " + std::to_string(i) + " : Translate").c_str());
		int interval = global->GetIntValue(itemName, ("CannonEnemyNumber : " + std::to_string(i) + " : Interval").c_str());
		float speed = global->GetFloatValue(itemName, ("CannonEnemyNumber : " + std::to_string(i) + " : BulletSpeed").c_str());
		auto& enemy = cEnemis_.emplace_back(std::make_unique<CannonEnemy>());
		enemy->Initialize(scale, rotate, trans);
		enemy->kInterval_ = static_cast<uint32_t>(interval);
		enemy->bulletSpeed_ = speed;
	}

	num = global->GetIntValue(itemName, "CoinConfirmation");
	if (!coins_.empty()) { coins_.clear(); }
	for (int i = 0; i < num; i++) {
		Vector3 scale = global->GetVector3Value(itemName, ("CoinNumber : " + std::to_string(i) + " : Scale").c_str());
		Quaternion rotate = global->GetQuaternionValue(itemName, ("CoinNumber : " + std::to_string(i) + " : Rotate").c_str());
		Vector3 trans = global->GetVector3Value(itemName, ("CoinNumber : " + std::to_string(i) + " : Translate").c_str());
		auto& enemy = coins_.emplace_back(std::make_unique<Coin>());
		enemy->Initialize(scale, rotate, trans);
	}

	num = global->GetIntValue(itemName, "HeartConfirmation");
	if (!hearts_.empty()) { hearts_.clear(); }
	for (int i = 0; i < num; i++) {
		Vector3 scale = global->GetVector3Value(itemName, ("HeartNumber : " + std::to_string(i) + " : Scale").c_str());
		Quaternion rotate = global->GetQuaternionValue(itemName, ("HeartNumber : " + std::to_string(i) + " : Rotate").c_str());
		Vector3 trans = global->GetVector3Value(itemName, ("HeartNumber : " + std::to_string(i) + " : Translate").c_str());
		auto& enemy = hearts_.emplace_back(std::make_unique<Heart>());
		enemy->Initialize(scale, rotate, trans);
	}

	num = global->GetIntValue(itemName, "SpawnerConfirmation");
	if (!nSpawners_.empty()) { nSpawners_.clear(); }
	for (int i = 0; i < num; i++) {
		Vector3 scale = global->GetVector3Value(itemName, ("SpawnerNumber : " + std::to_string(i) + " : Scale").c_str());
		Quaternion rotate = global->GetQuaternionValue(itemName, ("SpawnerNumber : " + std::to_string(i) + " : Rotate").c_str());
		Vector3 trans = global->GetVector3Value(itemName, ("SpawnerNumber : " + std::to_string(i) + " : Translate").c_str());
		int interval = global->GetIntValue(itemName, ("SpawnerNumber : " + std::to_string(i) + " : Interval").c_str());
		int maxpop = global->GetIntValue(itemName, ("SpawnerNumber : " + std::to_string(i) + " : MaxPop").c_str());
		int hp = global->GetIntValue(itemName, ("SpawnerNumber : " + std::to_string(i) + " : hp").c_str());
		auto& enemy = nSpawners_.emplace_back(std::make_unique<NormalSpawner>());
		enemy->Initialize(scale, rotate, trans, static_cast<uint32_t>(interval), static_cast<uint32_t>(maxpop), hp);
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
	for (auto& enemy : cEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Draw();
	}
	for (auto& enemy : coins_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Draw();
	}
	for (auto& enemy : hearts_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Draw();
	}
	for (auto& spawn : nSpawners_) {
		if (!spawn->GetIsAlive()) { continue; }
		spawn->Draw();
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
	for (auto& enemy : cEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Draw();
	}
	for (auto& enemy : coins_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Draw();
	}
	for (auto& enemy : hearts_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->Draw();
	}
	for (auto& spawn : nSpawners_) {
		if (!spawn->GetIsAlive()) { continue; }
		spawn->Draw();
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
	for (auto& enemy : cEnemis_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->EnemyDraw();
	}
	for (auto& enemy : coins_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->EnemyDraw();
	}
	for (auto& enemy : hearts_) {
		if (!enemy->GetIsAlive()) { continue; }
		enemy->EnemyDraw();
	}
	for (auto& spawn : nSpawners_) {
		if (!spawn->GetIsAlive()) { continue; }
		spawn->EnemyDraw();
	}
}

void EnemyManager::HousePopInitialize(const float& centerPosX) {
	for (auto& enemy : nEnemis_) {
		Vector3 pos = enemy->GetWorldTransform()->translation_;
		pos.x += centerPosX;
		enemy->SetPosition(pos);
	}
	for (auto& enemy : tEnemis_) {
		Vector3 pos = enemy->GetWorldTransform()->translation_;
		pos.x += centerPosX;
		enemy->SetPosition(pos);
	}
	for (auto& enemy : cEnemis_) {
		Vector3 pos = enemy->GetWorldTransform()->translation_;
		pos.x += centerPosX;
		enemy->SetPosition(pos);
	}
	for (auto& enemy : nSpawners_) {
		Vector3 pos = enemy->GetWorldTransform()->translation_;
		pos.x += centerPosX;
		enemy->SetPosition(pos);
	}
}

bool EnemyManager::Exists() const {
	if (remainderNumber_ <= 0u) { return false; }
	return true;
}
