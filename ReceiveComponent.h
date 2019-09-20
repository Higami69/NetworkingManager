#pragma once
#include "../Components/BaseComponent.h"

class ReceiveComponent : public BaseComponent
{
public:
	ReceiveComponent(size_t id);
	~ReceiveComponent();

	void* Receive() const;
	void SetData(void* data);

	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;

private:
	void* m_pData;
};