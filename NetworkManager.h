#pragma once
#include "../Helpers/Singleton.h"
#include "SafeQueue.h"
#include <thread>

#define INVALID_ADRESS (void*)0xcccccccc

struct DataPacket
{
	template <typename T>
	static DataPacket CreatePacket(T other, size_t id);

	DataPacket() = default;
	~DataPacket();
	DataPacket(const DataPacket& other);
	DataPacket& operator=(const DataPacket& other);
	DataPacket(DataPacket&& other) noexcept;
	DataPacket& operator=(DataPacket&& other) noexcept;

	size_t size;
	size_t id;
	void* data = nullptr;
};

template <typename T>
DataPacket DataPacket::CreatePacket(T other, size_t id)
{
	DataPacket result;

	result.size = sizeof(other);
	result.id = id;
	result.data = new T(other);

	return result;
}

class NetworkManager : public Singleton<NetworkManager>
{
public:
	NetworkManager();
	~NetworkManager();

	void StartThreads();
	void CloseThreads();

	void SetOtherClient(char* ip, unsigned short port);
	void SetPort(unsigned short port);

	void Send(DataPacket data);
	DataPacket Receive();

private:
	void RunSendThread(const std::shared_ptr<asio::ip::udp::socket>& socket);
	void RunReceiveThread(const std::shared_ptr<asio::ip::udp::socket>& socket);

	std::shared_ptr<asio::io_context> m_pIoContext;
	std::shared_ptr<asio::ip::udp::socket> m_pSocket;
	std::thread m_SendThread, m_ReceiveThread;
	std::mutex m_SocketMutex;
	SafeQueue<DataPacket> m_SendData, m_ReceiveData;
	bool m_IsThreadRunning = true;
	char* m_OtherClientIp;
	unsigned short m_OtherClientPort, m_Port;
};


