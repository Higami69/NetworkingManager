#include "stdafx.h"
#include "ReceiveComponent.h"
#include "ReceiveManager.h"

ReceiveComponent::ReceiveComponent(size_t id)
{
	ReceiveManager::GetInstance()->AddComponent(id, this);
	m_pData = nullptr;
}

ReceiveComponent::~ReceiveComponent()
{
	free(m_pData);
}


void* ReceiveComponent::Receive() const
{
	return m_pData;
}

void ReceiveComponent::SetData(void* data)
{
	if(m_pData != nullptr)
	{
		free(m_pData);
		m_pData = nullptr;
	}
	m_pData = data;
}

void ReceiveComponent::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void ReceiveComponent::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void ReceiveComponent::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}
