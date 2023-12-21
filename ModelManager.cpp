#include "ModelManager.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <cassert>
#include "DirectXCommon.h"
#include "TextureManager.h"
#include <fstream>
#include <sstream>
using namespace DirectX;

uint32_t ModelManager::Load(const std::string& fileName) {
	return ModelManager::GetInstance()->LoadInternal(fileName);
}

void ModelManager::CreateMeshes(ModelIndex& modelIndex)
{
    HRESULT result = S_FALSE;

    std::string line;//ファイルから読んだ1行を格納するもの
    std::vector<Vector3> positions;//位置
    std::vector<Vector3> normals;//法線
    std::vector<Vector2> texcoords;//テクスチャ座標
    std::vector<uint32_t> indexes; //index
    std::unordered_map<std::string, uint32_t> vertexDefinitionMap;
    std::string directoryPath = "Resources/models/" + modelIndex.name + "/";
    std::ifstream file(directoryPath + modelIndex.name + ".obj"); //ファイルを開く
    int multiMeshIndex = -1;
    Vector3 minModelSize{};
    Vector3 maxModelSize{};
    assert(file.is_open());//とりあえず開けなったら止める

    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;//先頭の識別子を読む
        if (identifier == "usemtl") {
            indexes.clear();
            multiMeshIndex++;
            if (modelIndex.meshes.size() <= multiMeshIndex) {
                modelIndex.meshes.emplace_back();
            }
        }
        else if (identifier == "v") {
            Vector3 position;
            s >> position.x >> position.y >> position.z;
            position.z *= -1.0f;
            /*position.w = 1.0f;*/
            positions.push_back(position);
            if (minModelSize.x > position.x) {
                minModelSize.x = position.x;
            }
            else if(maxModelSize.x < position.x){
                maxModelSize.x = position.x;
            }
            if (minModelSize.y > position.y) {
                minModelSize.y = position.y;
            }
            else if (maxModelSize.y < position.y) {
                maxModelSize.y = position.y;
            }
            if (minModelSize.z > position.z) {
                minModelSize.z = position.z;
            }
            else if (maxModelSize.z < position.z) {
                maxModelSize.z = position.z;
            }  
        }
        else if (identifier == "vn") {
            Vector3 normal;
            s >> normal.x >> normal.y >> normal.z;
           /* normal.z *= -1.0f;*/
            normals.push_back(normal);
        }
        else if (identifier == "vt") {
            Vector2 texcoord;
            s >> texcoord.x >> texcoord.y;
            texcoord.y = 1.0f - texcoord.y;
            texcoords.push_back(texcoord);
        }
        else if (identifier == "f") {
            // 面の頂点を取得
            std::vector<std::string> vertexDefinitions;
            while (true) {
                std::string vertexDefinition;
                s >> vertexDefinition;
                if (vertexDefinition.empty()) {
                    break;
                }
                vertexDefinitions.emplace_back(std::move(vertexDefinition));
            }
            assert(vertexDefinitions.size() > 2);
            std::vector<uint32_t> face(vertexDefinitions.size());
            for (uint32_t i = 0; i < vertexDefinitions.size(); ++i) {
                // 頂点が登録済み
                if (vertexDefinitionMap.contains(vertexDefinitions[i])) {
                    face[i] = vertexDefinitionMap[vertexDefinitions[i]];
                }
                else {
                    std::istringstream v(vertexDefinitions[i]);
                    uint32_t elementIndices[3] = {0,0,0};
                    bool useElement[3]{};
                    for (int32_t element = 0; element < 3; ++element) {
                        std::string index;
                        std::getline(v, index, '/');//区切りでインデックスを読んでいく
                        if (!index.empty()) {
                            elementIndices[element] = static_cast<uint32_t>(std::stoi(index)) - 1;
                            useElement[element] = true;
                        }
                    }
                    auto& vertex = modelIndex.vertices_.emplace_back();
                    vertex.pos = positions[elementIndices[0]];
                    if (useElement[1]) {
                        vertex.uv = texcoords[elementIndices[1]];
                    }
                    if (useElement[2]) {
                        vertex.normal = normals[elementIndices[2]];
                    }
                    face[i] = vertexDefinitionMap[vertexDefinitions[i]] = static_cast<uint32_t>(modelIndex.vertices_.size() - 1);
                }
            }


            for (uint32_t i = 0; i < face.size() - 2; ++i) {
                modelIndex.meshes[multiMeshIndex].indices_.emplace_back(face[i + 2ull]);
                modelIndex.meshes[multiMeshIndex].indices_.emplace_back(face[i + 1ull]);
                modelIndex.meshes[multiMeshIndex].indices_.emplace_back(face[0]);
            }
        }
        else if (identifier == "mtllib") {
            //materialTemplateLibraryファイルの名前を取得する
            std::string materialFilename;
            s >> materialFilename;
            //基本的にobjファイルと同一階層にmtlは存在させるので,ディレクトリ名とファイル名を渡す

            std::string uvFilePass;//構築するMaterialData
            std::string line;//fileから読んだ１行を格納するもの
            std::ifstream file("Resources/models/" + modelIndex.name + "/" + modelIndex.name + ".mtl"); //ファイルを開く
            assert(file.is_open());//とりあえず開けなかったら止める
            while (std::getline(file, line)) {
                std::string identifier;
                std::istringstream s(line);
                s >> identifier;
                //identifierに応じた処理
                if (identifier == "map_Kd") {
                    std::string textureFilename;
                    s >> textureFilename;
                    //連結してファイルパスにする
                    uvFilePass = directoryPath + textureFilename;
                    auto& mesh = modelIndex.meshes.emplace_back();
                    mesh.uvHandle_ = TextureManager::LoadUv(textureFilename, uvFilePass);
                }
            }
        }
    }

    modelIndex.modelSize = (maxModelSize - minModelSize);
    modelIndex.modelCenter = maxModelSize - Vector3(modelIndex.modelSize / 2.0f) ;
    //もしmodelの原点を一番下にしていたら
    if (minModelSize.y < 0.5f || minModelSize.y < -0.5f) {
        modelIndex.modelCenter.y = minModelSize.y;
    }
    
        // 頂点データのサイズ
        UINT sizeVB = static_cast<UINT>(sizeof(VertexData) * modelIndex.vertices_.size());

        modelIndex.vertexBuffer_.Create(sizeVB);

        modelIndex.vertexBuffer_.Copy(modelIndex.vertices_.data(), sizeVB);

        // 頂点バッファビューの作成
        modelIndex.vbView_.BufferLocation = modelIndex.vertexBuffer_.GetGPUVirtualAddress();
        modelIndex.vbView_.SizeInBytes = sizeVB;
        modelIndex.vbView_.StrideInBytes = sizeof(modelIndex.vertices_[0]);
       
    for (auto& mesh : modelIndex.meshes) {

        // インデックスデータのサイズ
        UINT sizeIB = static_cast<UINT>(sizeof(uint32_t) * mesh.indices_.size());

        mesh.indexBuffer_.Create(sizeIB);

        mesh.indexBuffer_.Copy(mesh.indices_.data(), sizeIB);

        // インデックスバッファビューの作成
        mesh.ibView_.BufferLocation = mesh.indexBuffer_.GetGPUVirtualAddress();
        mesh.ibView_.Format = DXGI_FORMAT_R32_UINT;
        mesh.ibView_.SizeInBytes = sizeIB;

    }
}


ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;
	return &instance;
}

void ModelManager::DrawInstanced(ID3D12GraphicsCommandList* commandList, uint32_t modelHandle, UINT textureRootParamterIndex) {
	assert(modelHandle < kNumModels);

    // 頂点バッファの設定
    commandList->IASetVertexBuffers(0, 1, &models_[modelHandle].vbView_);

    for (auto& mesh : models_[modelHandle].meshes) {
        // srvセット
        TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, textureRootParamterIndex, mesh.GetUv());
        // インデックスバッファの設定
        commandList->IASetIndexBuffer(mesh.GetIbView());
        // 描画コマンド
        commandList->DrawIndexedInstanced(static_cast<UINT>(mesh.indices_.size()), 1, 0, 0, 0);
    }
}

uint32_t ModelManager::LoadInternal(const std::string& name) {

	assert(useModelCount_ < kNumModels);
	uint32_t handle = useModelCount_;

	// 読み込み済みmodelを検索
	auto it = std::find_if(models_.begin(), models_.end(), [&](const auto& texture) {return texture.name == name; });

	if (it != models_.end()) {
		// 読み込み済みmodelの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(models_.begin(), it));
		return handle;
	}

	// 書き込むmodelの参照
	ModelIndex& modelIndex = models_.at(useModelCount_);
	modelIndex.name = name;

	CreateMeshes(modelIndex);

	useModelCount_++;
	return handle;
}

Vector3 ModelManager::GetModelSize(uint32_t modelHandle)
{
    return models_[modelHandle].modelSize;
}

Vector3 ModelManager::GetModelCenter(uint32_t modelHandle)
{
    return models_[modelHandle].modelCenter;
}

