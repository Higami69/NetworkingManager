#include "stdafx.h"
#include "ReceiveManager.h"
#include "NetworkManager.h"

ReceiveManager::ReceiveManager()
{
}

void ReceiveManager::AddComponent(size_t id, ReceiveComponent* comp)
{
	m_pReceiveComponents[id] = comp;
}

void ReceiveManager::Update()
{
	auto networkManager = NetworkManager::GetInstance();
	auto packet = networkManager->Receive();

	while(packet.data != nullptr)
	{
		m_pReceiveComponents[packet.id]->SetData(packet.data);
		packet.data = nullptr;
		packet = networkManager->Receive();
	}
}
