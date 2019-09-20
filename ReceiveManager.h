#pragma once
#include "../Helpers/Singleton.h"
#include "ReceiveComponent.h"
#include <map>

class ReceiveManager : public Singleton<ReceiveManager>
{
public:
	ReceiveManager();

	void AddComponent(size_t id, ReceiveComponent* comp);
	void Update();

private:
	std::map<size_t, ReceiveComponent*> m_pReceiveComponents;
};
