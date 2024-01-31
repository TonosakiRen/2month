#include "Stage.h"
#include "ModelManager.h"
#include "externals/imgui/imgui.h"
#include <string>
#include "GlobalVariables.h"
#include "Player.h"
#include "SpotLights.h"
#include "PointLights.h"
#include "ShadowSpotLights.h"
#include "Game/Character/EnemyManager.h"

void Stage::Initialize(const std::filesystem::path& loadFile, PointLights* pointLight, SpotLights* spotLight, ShadowSpotLights* shadowSpotLight) {
	Load(loadFile);
	pointLights_ = pointLight;
	spotLights_ = spotLight;
	shadowSpotLights_ = shadowSpotLight;
	SetSpotLight();

}

void Stage::Update(const Vector3& playerWorldPosition) {
	for (auto& wall : walls_) {
		wall->Update(playerWorldPosition);
	}
	for (auto& light : wallLights_) {
		light->Update(playerWorldPosition);
	}
	for (auto& floor : floors_) {
		floor->Update(playerWorldPosition);
	}
	for (auto& truck : trucks_) {
		truck->Update(playerWorldPosition);
	}
	for (auto& woodbox : woodboxs_) {
		woodbox->Update(playerWorldPosition);
	}
	for (auto& floor : moveFloors_) {
		floor->Update(playerWorldPosition);
	}
	for (auto& spotlight : stagelights_) {
		spotlight->Update(playerWorldPosition);
	}
	for (auto& trap : trapButtons_) {
		trap->Update(playerWorldPosition);
	}


	if (mHouse_.isFalled_) {
		Confine();
	}
	else if (mHouse_.isBreaked_) {
		ConfineBreak();
	}

}

void Stage::Draw() {
	for (auto& wall : walls_) {
		wall->Draw();
	}
	for (auto& light : wallLights_) {
		light->Draw();
	}
	for (auto& floor : floors_) {
		floor->Draw();
	}
	for (auto& truck : trucks_) {
		truck->Draw();
	}
	for (auto& woodbox : woodboxs_) {
		woodbox->Draw();
	}
	for (auto& floor : moveFloors_) {
		floor->Draw();
	}
	for (auto& spotlight : stagelights_) {
		spotlight->Draw();
	}
	for (auto& trap : trapButtons_) {
		trap->Draw();
	}
}


void Stage::ShadowDraw() {
	for (auto& light : wallLights_) {
		light->Draw();
	}
	for (auto& truck : trucks_) {
		truck->Draw();
	}
	for (auto& woodbox : woodboxs_) {
		woodbox->Draw();
	}
}

void Stage::SpotLightShadowDraw() {
	for (auto& light : wallLights_) {
		light->Draw();
	}
	for (auto& truck : trucks_) {
		truck->Draw();
	}
	for (auto& woodbox : woodboxs_) {
		woodbox->Draw();
	}
}

void Stage::DrawImGui() {
#ifdef _DEBUG
	if (ImGui::BeginMenu("Object")) {
		if (ImGui::BeginMenu("Walls")) {
			if (ImGui::Button("Create")) {
				walls_.emplace_back(std::make_unique<Wall>())->Initialize(Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
			}
			// 要素数確認
			ImGui::Text("ElementCount = %d", walls_.size());
			for (int i = 0; i < walls_.size(); i++) {
				if (ImGui::TreeNode(("WallNumber : " + std::to_string(i)).c_str())) {
					walls_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						walls_.erase(walls_.begin() + i);
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("WallLights")) {
			if (ImGui::Button("Create")) {
				wallLights_.emplace_back(std::make_unique<WallLight>())->Initialize(Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
			}
			// 要素数確認
			ImGui::Text("ElementCount = %d", wallLights_.size());
			for (int i = 0; i < wallLights_.size(); i++) {
				if (ImGui::TreeNode(("WallRightNumber : " + std::to_string(i)).c_str())) {
					wallLights_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						wallLights_.erase(wallLights_.begin() + i);
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Floors")) {
			if (ImGui::Button("Create")) {
				floors_.emplace_back(std::make_unique<Floor>())->Initialize(Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
			}
			// 要素数確認
			ImGui::Text("ElementCount = %d", floors_.size());
			for (int i = 0; i < floors_.size(); i++) {
				if (ImGui::TreeNode(("FloorNumber : " + std::to_string(i)).c_str())) {
					floors_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						floors_.erase(floors_.begin() + i);
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("MoveFloors")) {
			if (ImGui::Button("Create")) {
				moveFloors_.emplace_back(std::make_unique<MoveFloor>())->Initialize(Vector3(-10.0f, 0.0f, 0.0f), Vector3(10.0f, 0.0f, 0.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f), 120.0f);
			}
			// 要素数確認
			ImGui::Text("ElementCount = %d", moveFloors_.size());
			for (int i = 0; i < moveFloors_.size(); i++) {
				if (ImGui::TreeNode(("FloorNumber : " + std::to_string(i)).c_str())) {
					moveFloors_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						moveFloors_.erase(moveFloors_.begin() + i);
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Trucks")) {
			if (ImGui::Button("Create")) {
				trucks_.emplace_back(std::make_unique<Truck>())->Initialize(Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
			}
			// 要素数確認
			ImGui::Text("ElementCount = %d", trucks_.size());
			for (int i = 0; i < trucks_.size(); i++) {
				if (ImGui::TreeNode(("TruckNumber : " + std::to_string(i)).c_str())) {
					trucks_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						trucks_.erase(trucks_.begin() + i);
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("WoodBox")) {
			if (ImGui::Button("Create")) {
				woodboxs_.emplace_back(std::make_unique<WoodBox>())->Initialize(Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
			}
			// 要素数確認
			ImGui::Text("ElementCount = %d", woodboxs_.size());
			for (int i = 0; i < woodboxs_.size(); i++) {
				if (ImGui::TreeNode(("WoodBoxNumber : " + std::to_string(i)).c_str())) {
					woodboxs_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						woodboxs_.erase(woodboxs_.begin() + i);
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("StageLight")) {
			if (ImGui::Button("Create")) {
				stagelights_.emplace_back(std::make_unique<StageLight>())->Initialize(Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
			}
			// 要素数確認
			ImGui::Text("ElementCount = %d", stagelights_.size());
			for (int i = 0; i < stagelights_.size(); i++) {
				if (ImGui::TreeNode(("StageLightNumber : " + std::to_string(i)).c_str())) {
					stagelights_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						stagelights_.erase(stagelights_.begin() + i);
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("TrapButton")) {
			if (ImGui::Button("Create")) {
				trapButtons_.emplace_back(std::make_unique<TrapButton>())->Initialize(Vector3(1.0f, 1.0f, 1.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
			}
			// 要素数確認
			ImGui::Text("ElementCount = %d", trapButtons_.size());
			for (int i = 0; i < trapButtons_.size(); i++) {
				if (ImGui::TreeNode(("TrapButtonNumber : " + std::to_string(i)).c_str())) {
					trapButtons_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						trapButtons_.erase(trapButtons_.begin() + i);
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

void Stage::Load(const std::filesystem::path& loadFile) {
	GlobalVariables* global = GlobalVariables::GetInstance();
	std::string selectName = loadFile.string();
	global->LoadFile(selectName);

	int num = global->GetIntValue(selectName, "WallConfirmation");
	walls_.clear(); // 要素の全削除
	for (int i = 0; i < num; i++) {
		Vector3 scale = global->GetVector3Value(selectName, ("WallNumber : " + std::to_string(i) + " : Scale").c_str());
		Quaternion rotate = global->GetQuaternionValue(selectName, ("WallNumber : " + std::to_string(i) + " : Rotate").c_str());
		Vector3 trans = global->GetVector3Value(selectName, ("WallNumber : " + std::to_string(i) + " : Translate").c_str());
		auto& wall = walls_.emplace_back(std::make_unique<Wall>());
		wall->Initialize(scale, rotate, trans);
	}

	num = global->GetIntValue(selectName, "WallLightConfirmation");
	wallLights_.clear(); // 要素の全削除
	for (int i = 0; i < num; i++) {
		Vector3 scale = global->GetVector3Value(selectName, ("WallLightNumber : " + std::to_string(i) + " : Scale").c_str());
		Quaternion rotate = global->GetQuaternionValue(selectName, ("WallLightNumber : " + std::to_string(i) + " : Rotate").c_str());
		Vector3 trans = global->GetVector3Value(selectName, ("WallLightNumber : " + std::to_string(i) + " : Translate").c_str());
		auto& wallLight = wallLights_.emplace_back(std::make_unique<WallLight>());
		wallLight->Initialize(scale, rotate, trans);
	}

	num = global->GetIntValue(selectName, "FloorConfirmation");
	floors_.clear(); // 要素の全削除
	for (int i = 0; i < num; i++) {
		Vector3 scale = global->GetVector3Value(selectName, ("FloorNumber : " + std::to_string(i) + " : Scale").c_str());
		Quaternion rotate = global->GetQuaternionValue(selectName, ("FloorNumber : " + std::to_string(i) + " : Rotate").c_str());
		Vector3 trans = global->GetVector3Value(selectName, ("FloorNumber : " + std::to_string(i) + " : Translate").c_str());
		auto& floors = floors_.emplace_back(std::make_unique<Floor>());
		floors->Initialize(scale, rotate, trans);
	}

	num = global->GetIntValue(selectName, "MoveFloorConfirmation");
	moveFloors_.clear(); // 要素の全削除
	for (int i = 0; i < num; i++) {
		Vector3 scale = global->GetVector3Value(selectName, ("MoveFloorNumber : " + std::to_string(i) + " : Scale").c_str());
		Quaternion rotate = global->GetQuaternionValue(selectName, ("MoveFloorNumber : " + std::to_string(i) + " : Rotate").c_str());
		Vector3 start = global->GetVector3Value(selectName, ("MoveFloorNumber : " + std::to_string(i) + " : StartPosition").c_str());
		Vector3 end = global->GetVector3Value(selectName, ("MoveFloorNumber : " + std::to_string(i) + " : EndPosition").c_str());
		float speed = global->GetFloatValue(selectName, ("MoveFloorNumber : " + std::to_string(i) + " : Speed").c_str());
		auto& floors = moveFloors_.emplace_back(std::make_unique<MoveFloor>());
		floors->Initialize(start, end, rotate, scale, speed);
	}

	num = global->GetIntValue(selectName, "TruckConfirmation");
	trucks_.clear(); // 要素の全削除
	for (int i = 0; i < num; i++) {
		Vector3 scale = global->GetVector3Value(selectName, ("TruckNumber : " + std::to_string(i) + " : Scale").c_str());
		Quaternion rotate = global->GetQuaternionValue(selectName, ("TruckNumber : " + std::to_string(i) + " : Rotate").c_str());
		Vector3 trans = global->GetVector3Value(selectName, ("TruckNumber : " + std::to_string(i) + " : Translate").c_str());
		auto& truck = trucks_.emplace_back(std::make_unique<Truck>());
		truck->Initialize(scale, rotate, trans);
	}


	num = global->GetIntValue(selectName, "WoodBoxConfirmation");
	woodboxs_.clear(); // 要素の全削除
	for (int i = 0; i < num; i++) {
		Vector3 scale = global->GetVector3Value(selectName, ("WoodBoxNumber : " + std::to_string(i) + " : Scale").c_str());
		Quaternion rotate = global->GetQuaternionValue(selectName, ("WoodBoxNumber : " + std::to_string(i) + " : Rotate").c_str());
		Vector3 trans = global->GetVector3Value(selectName, ("WoodBoxNumber : " + std::to_string(i) + " : Translate").c_str());
		auto& woodbox = woodboxs_.emplace_back(std::make_unique<WoodBox>());
		woodbox->Initialize(scale, rotate, trans);
	}

	num = global->GetIntValue(selectName, "StageLightConfirmation");
	stagelights_.clear(); // 要素の全削除
	for (int i = 0; i < num; i++) {
		Vector3 scale = global->GetVector3Value(selectName, ("StageLightNumber : " + std::to_string(i) + " : Scale").c_str());
		Quaternion rotate = global->GetQuaternionValue(selectName, ("StageLightNumber : " + std::to_string(i) + " : Rotate").c_str());
		Vector3 trans = global->GetVector3Value(selectName, ("StageLightNumber : " + std::to_string(i) + " : Translate").c_str());
		auto& stagelight = stagelights_.emplace_back(std::make_unique<StageLight>());
		stagelight->Initialize(scale, rotate, trans);
	}

	// playerの初期位置
	playerRespawnPoint_.scale = global->GetVector3Value(selectName, "Player : Scale");
	playerRespawnPoint_.rotate = global->GetQuaternionValue(selectName, "Player : Rotate");
	playerRespawnPoint_.translate = global->GetVector3Value(selectName, "Player : Translate");
}

void Stage::Save(const char* itemName) {
	GlobalVariables* global = GlobalVariables::GetInstance();

	global->SetValue(itemName, "WallConfirmation" + std::string(), static_cast<int>(walls_.size()));
	for (uint32_t index = 0u; index < static_cast<uint32_t>(walls_.size()); index++) {
		global->SetValue(itemName, ("WallNumber : " + std::to_string(index) + " : Scale").c_str(), walls_[index]->GetWorldTransform()->scale_);
		global->SetValue(itemName, ("WallNumber : " + std::to_string(index) + " : Rotate").c_str(), walls_[index]->GetWorldTransform()->quaternion_);
		global->SetValue(itemName, ("WallNumber : " + std::to_string(index) + " : Translate").c_str(), walls_[index]->GetWorldTransform()->translation_);
	}

	global->SetValue(itemName, "WallLightConfirmation" + std::string(), static_cast<int>(wallLights_.size()));
	for (uint32_t index = 0u; index < static_cast<uint32_t>(wallLights_.size()); index++) {
		global->SetValue(itemName, ("WallLightNumber : " + std::to_string(index) + " : Scale").c_str(), wallLights_[index]->GetWorldTransform()->scale_);
		global->SetValue(itemName, ("WallLightNumber : " + std::to_string(index) + " : Rotate").c_str(), wallLights_[index]->GetWorldTransform()->quaternion_);
		global->SetValue(itemName, ("WallLightNumber : " + std::to_string(index) + " : Translate").c_str(), wallLights_[index]->GetWorldTransform()->translation_);
	}

	global->SetValue(itemName, "FloorConfirmation" + std::string(), static_cast<int>(floors_.size()));
	for (uint32_t index = 0u; index < static_cast<uint32_t>(floors_.size()); index++) {
		global->SetValue(itemName, ("FloorNumber : " + std::to_string(index) + " : Scale").c_str(), floors_[index]->GetWorldTransform()->scale_);
		global->SetValue(itemName, ("FloorNumber : " + std::to_string(index) + " : Rotate").c_str(), floors_[index]->GetWorldTransform()->quaternion_);
		global->SetValue(itemName, ("FloorNumber : " + std::to_string(index) + " : Translate").c_str(), floors_[index]->GetWorldTransform()->translation_);
	}

	global->SetValue(itemName, "MoveFloorConfirmation" + std::string(), static_cast<int>(moveFloors_.size()));
	for (uint32_t index = 0u; index < static_cast<uint32_t>(moveFloors_.size()); index++) {
		global->SetValue(itemName, ("MoveFloorNumber : " + std::to_string(index) + " : Scale").c_str(), moveFloors_[index]->GetWorldTransform()->scale_);
		global->SetValue(itemName, ("MoveFloorNumber : " + std::to_string(index) + " : Rotate").c_str(), moveFloors_[index]->GetWorldTransform()->quaternion_);
		global->SetValue(itemName, ("MoveFloorNumber : " + std::to_string(index) + " : StartPosition").c_str(), moveFloors_[index]->GetParam().startPos_);
		global->SetValue(itemName, ("MoveFloorNumber : " + std::to_string(index) + " : EndPosition").c_str(), moveFloors_[index]->GetParam().endPos_);
		global->SetValue(itemName, ("MoveFloorNumber : " + std::to_string(index) + " : Speed").c_str(), moveFloors_[index]->GetParam().speed_);
	}

	global->SetValue(itemName, "TruckConfirmation" + std::string(), static_cast<int>(trucks_.size()));
	for (uint32_t index = 0u; index < static_cast<uint32_t>(trucks_.size()); index++) {
		global->SetValue(itemName, ("TruckNumber : " + std::to_string(index) + " : Scale").c_str(), trucks_[index]->GetWorldTransform()->scale_);
		global->SetValue(itemName, ("TruckNumber : " + std::to_string(index) + " : Rotate").c_str(), trucks_[index]->GetWorldTransform()->quaternion_);
		global->SetValue(itemName, ("TruckNumber : " + std::to_string(index) + " : Translate").c_str(), trucks_[index]->GetWorldTransform()->translation_);
	}

	global->SetValue(itemName, "WoodBoxConfirmation" + std::string(), static_cast<int>(woodboxs_.size()));
	for (uint32_t index = 0u; index < static_cast<uint32_t>(woodboxs_.size()); index++) {
		global->SetValue(itemName, ("WoodBoxNumber : " + std::to_string(index) + " : Scale").c_str(), woodboxs_[index]->GetWorldTransform()->scale_);
		global->SetValue(itemName, ("WoodBoxNumber : " + std::to_string(index) + " : Rotate").c_str(), woodboxs_[index]->GetWorldTransform()->quaternion_);
		global->SetValue(itemName, ("WoodBoxNumber : " + std::to_string(index) + " : Translate").c_str(), woodboxs_[index]->GetWorldTransform()->translation_);
	}

	global->SetValue(itemName, "StageLightConfirmation" + std::string(), static_cast<int>(stagelights_.size()));
	for (uint32_t index = 0u; index < static_cast<uint32_t>(stagelights_.size()); index++) {
		global->SetValue(itemName, ("StageLightNumber : " + std::to_string(index) + " : Scale").c_str(), stagelights_[index]->GetWorldTransform()->scale_);
		global->SetValue(itemName, ("StageLightNumber : " + std::to_string(index) + " : Rotate").c_str(), stagelights_[index]->GetWorldTransform()->quaternion_);
		global->SetValue(itemName, ("StageLightNumber : " + std::to_string(index) + " : Translate").c_str(), stagelights_[index]->GetWorldTransform()->translation_);
	}


}

void Stage::Collision(Player* player) {
	Vector3 pushBackVector;
	player->SetParent(nullptr);
	for (auto& wall : walls_) {
		if(wall->collider_.Collision(player->bodyCollider_, pushBackVector)) {
			//player->worldTransform_.translation_ -= pushBackVector;
			player->CollisionProcess(-pushBackVector);
		}
	}
	for (auto& floor : floors_) {
		if(floor->collider_.Collision(player->bodyCollider_, pushBackVector)) {
			//player->worldTransform_.translation_ -= pushBackVector;
			player->CollisionProcess(-pushBackVector);
		}
	}
	for (auto& floor : moveFloors_) {
		if(floor->collider_.Collision(player->bodyCollider_, pushBackVector)) {
			//player->worldTransform_.translation_ -= pushBackVector;
			player->CollisionProcess(-pushBackVector);
			float y = Normalize(-pushBackVector).y;
			if (y == 1.0f) {
				player->GetWorldTransform()->SetIsRotateParent(false);
				player->GetWorldTransform()->SetIsScaleParent(false);
				player->SetParent(floor->GetWorldTransform());
			}
		}
	}
	for (auto& truck : trucks_) {
		if(truck->collider_.Collision(player->bodyCollider_, pushBackVector)) {
			//player->worldTransform_.translation_ -= pushBackVector;
			player->CollisionProcess(-pushBackVector);
		}
	}
	for (auto& woodbox : woodboxs_) {
		if(woodbox->collider_.Collision(player->bodyCollider_, pushBackVector)) {
			//player->worldTransform_.translation_ -= pushBackVector;
			player->CollisionProcess(-pushBackVector);
		}
	}
	//for (auto& trap : trapButtons_) {
	//	if(trap->collider_.Collision(player->bodyCollider_, pushBackVector)) {
	//		//player->worldTransform_.translation_ -= pushBackVector;
	//		//player->CollisionProcess(-pushBackVector);
	//		ConfineInitialize(trap->GetWorldTransform()->GetWorldTranslate());
	//		// ボタンの削除
	//	}
	//}
	for (uint32_t index = 0u; index < static_cast<uint32_t>(trapButtons_.size()); index++) {
		if (trapButtons_.at(index)->collider_.Collision(player->bodyCollider_, pushBackVector)) {
			ConfineInitialize(trapButtons_.at(index)->GetWorldTransform()->GetWorldTranslate());
			// ボタンの削除
			trapButtons_.erase(trapButtons_.begin() + index);
			index--;
		}
	}

	//player->worldTransform_.Update();
}

void Stage::Collision(EnemyManager* enemis) {
	Vector3 pushBackVector;
	for (auto& wall : walls_) {
		enemis->OnCollisionStage(wall->collider_);
	}
	for (auto& floor : floors_) {
		enemis->OnCollisionStage(floor->collider_);
	}
	for (auto& truck : trucks_) {
		enemis->OnCollisionStage(truck->collider_);
	}
	for (auto& woodbox : woodboxs_) {
		enemis->OnCollisionStage(woodbox->collider_);
	}

}

void Stage::SetPlayerRespawn(Player* const player) const {
	player->GetWorldTransform()->scale_ = playerRespawnPoint_.scale;
	player->GetWorldTransform()->quaternion_ = playerRespawnPoint_.rotate;
	player->GetWorldTransform()->translation_ = playerRespawnPoint_.translate;
	player->GetWorldTransform()->Update();
}

void Stage::SetSpotLight() {
	spotLights_->lights_.at(0).worldTransform.translation_ = wallLights_.at(0)->GetLightPos();
	spotLights_->lights_.at(0).worldTransform.Update();
}

void Stage::ConfineInitialize(const Vector3& position) {
	mHouse_.boxNumber = static_cast<uint32_t>(woodboxs_.size());
	mHouse_.kBoxCount = 24u; // 3の倍数のみかつ2で割れるもの
	mHouse_.isFalled_ = true;

	int y = 0;
	int x = 0;
	int z = 0;
	Vector3 space(8.0f,1.0f,1.0f);
	Vector3 scale(2.0f, 2.0f, 2.0f);
	Quaternion rotate(0.0f, 0.0f, 0.0f, 1.0f);
	float initY = 5.0f;

	for (uint32_t index = 0u; index < mHouse_.kBoxCount; index++) {
		auto& woodbox = woodboxs_.emplace_back(std::make_unique<WoodBox>());
		
		Vector3 trans;
		if (index >= mHouse_.kBoxCount / 2u) {
			trans.x = position.x - (scale.x * space.x);
		}
		else {
			trans.x = position.x + (scale.x * space.x);
		}

		z++;
		if (index % 3u == 0u) {
			y++;
			if (index == mHouse_.kBoxCount / 2u || index == 0u) {
				y = 0u;
			}
			z = 0u;
		}
		trans.y = initY + scale.y + (static_cast<float>(y) * (scale.y * 2.0f + space.y));
		
		trans.z = (-3.0f) + (static_cast<float>(z - 1) * (scale.z * 2.0f + space.z));
		
		woodbox->Initialize(scale, rotate, trans);
	}
}

void Stage::Confine() {
	
	const float speed = 0.1f;
	int y = 0;
	uint32_t count = 0u;
	uint32_t integral = 0u;
	uint32_t kMaxCount = mHouse_.boxNumber + mHouse_.kBoxCount;
	for (uint32_t index = mHouse_.boxNumber; index < kMaxCount; index++) {
		auto& woodbox = woodboxs_.at(index);

		woodbox->GetWorldTransform()->translation_.y -= speed;

		if (integral % 3u == 0u) {
			y++;
			if (integral == mHouse_.kBoxCount / 2u || integral == 0u) {
				y = 0u;
			}
		}
		float incY = woodbox->GetWorldTransform()->scale_.y + (static_cast<float>(y) * (woodbox->GetWorldTransform()->scale_.y * 2.0f));

		if (woodbox->GetWorldTransform()->translation_.y <= incY) {
			woodbox->GetWorldTransform()->translation_.y = incY;
			count++;
			if (count >= mHouse_.kBoxCount) {
				mHouse_.isFalled_ = false;
				mHouse_.isBreaked_ = true; // とりあえず仮置き
			}
		}
		integral++;
	}
}

void Stage::ConfineBreak() {
	const float speed = 0.1f;
	int y = 0;
	uint32_t count = 0u;
	float kMinY = -10.0f;
	for (uint32_t index = mHouse_.boxNumber; index < mHouse_.boxNumber + mHouse_.kBoxCount; index++) {
		auto& woodbox = woodboxs_.at(index);

		woodbox->GetWorldTransform()->translation_.y -= speed;

		// 一定以下になったら削除
		if (woodbox->GetWorldTransform()->translation_.y <= kMinY) {
			woodboxs_.erase(woodboxs_.begin() + index);
			index--;
			if (--mHouse_.kBoxCount == 0u) {
				mHouse_.isBreaked_ = false;
			}
		}
	}
}
