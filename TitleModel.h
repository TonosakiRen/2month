#pragma once
#include"GameObject.h"
class TitleModel:

	public GameObject
	{
	public:

		void Initialize(Vector3 scale, Vector3 translate);
		void Update();
		void Draw();
		void DrawImGui();

	public:		
		Vector3 rotate;
	private:
		//bool ActiveChack(const Vector3& playerWorldPosition) const;
		bool isActive_ = true;
};

