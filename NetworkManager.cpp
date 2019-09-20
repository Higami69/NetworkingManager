#include "stdafx.h"
#include "NetworkManager.h"
#include <queue>

DataPacket::~DataPacket()
{
	if (data != nullptr && data != INVALID_ADRESS)
	{
		free(data);
	}
}

DataPacket::DataPacket(const DataPacket& other)
{
	this->size = other.size;
	this->data = malloc(size);
	memcpy(data, other.data, other.size);
	this->id = other.id;
}

DataPacket& DataPacket::operator=(const DataPacket& other)
{
	this->size = other.size;
	this->data = malloc(size);
	memcpy(this->data, other.data, other.size);
	this->id = other.id;

	return *this;
}

DataPacket::DataPacket(DataPacket&& other) noexcept
{
	this->size = other.size;
	this->data = other.data;
	other.data = nullptr;
	this->id = other.id;
}

DataPacket& DataPacket::operator=(DataPacket&& other) noexcept
{
	this->size = other.size;
	this->data = other.data;
	other.data = nullptr;
	this->id = other.id;

	return *this;
}

NetworkManager::NetworkManager()
	:m_IsThreadRunning(false)
	,m_Port(0)
	,m_OtherClientPort(0)
	,m_OtherClientIp("")
{
}

NetworkManager::~NetworkManager()
{
	delete m_OtherClientIp;
}

void NetworkManager::StartThreads()
{
	if (!m_IsThreadRunning)
	{
		m_pIoContext = std::make_shared<asio::io_context>();

		asio::ip::udp::endpoint localEndpoint(asio::ip::udp::v4(), m_Port);
		m_pSocket = std::make_shared<asio::ip::udp::socket>(*m_pIoContext, asio::ip::udp::v4());
		m_pSocket->bind(localEndpoint);

		Logger::LogInfo(L"Socket: " + std::to_wstring(m_pSocket->is_open()));
		Logger::LogInfo(L"Port: " + std::to_wstring(m_Port));

		m_IsThreadRunning = true;

		m_SendThread = thread(&NetworkManager::RunSendThread, this, m_pSocket);
		m_ReceiveThread = thread(&NetworkManager::RunReceiveThread, this, m_pSocket);
	}
}

void NetworkManager::CloseThreads()
{
	m_IsThreadRunning = false;

	m_pSocket->shutdown(asio::ip::udp::socket::shutdown_both);
	m_pSocket->close();

	m_ReceiveThread.join();
	m_SendThread.join();
}

void NetworkManager::SetOtherClient(char* ip, unsigned short port)
{
	m_OtherClientIp = ip;
	m_OtherClientPort = port;
}

void NetworkManager::SetPort(unsigned short port)
{
	m_Port = port;
}

void NetworkManager::Send(DataPacket data)
{
	m_SendData.Push(data);
}

DataPacket NetworkManager::Receive()
{
	return m_ReceiveData.Pop();
}

void NetworkManager::RunSendThread(const std::shared_ptr<asio::ip::udp::socket>& socket)
{
	asio::error_code ec;

	asio::ip::udp::endpoint otherClient(asio::ip::address::from_string(m_OtherClientIp), m_OtherClientPort);
	std::cout << "OtherClient IP: " + string(m_OtherClientIp) + ", Port: " + std::to_string(m_OtherClientPort) + '\n';

	const size_t maxDataSize = 1024;

	while(m_IsThreadRunning)
	{
		//Send
		if(!m_SendData.IsEmpty())
		{
			DataPacket packet = m_SendData.Pop();
			void* data = malloc(packet.size + sizeof(size_t));
			memcpy(data, &packet.id, sizeof(size_t));
			void* p = (size_t*)data + 1;
			memcpy(p, packet.data, packet.size);

			try
			{
				m_SocketMutex.lock();
				socket->send_to(asio::const_buffer(data, packet.size + sizeof(size_t)), otherClient);
				m_SocketMutex.unlock();
				
			}
			catch(std::system_error e)
			{
				//std::cerr << e.what();
				m_SendData.Push(packet);

				m_SocketMutex.unlock();
			}

			free(data);
		}
	}
}

void NetworkManager::RunReceiveThread(const std::shared_ptr<asio::ip::udp::socket>& socket)
{
	asio::error_code ec;

	const size_t maxDataSize = 1024;
	void* data = malloc(maxDataSize + sizeof(size_t));

	while (m_IsThreadRunning)
	{
		//Receive
		try
		{
			DataPacket packet{};

			m_SocketMutex.lock();
			if (socket->available() > 0)
			{
				packet.size = socket->receive(asio::mutable_buffer(data, maxDataSize));
			}
			m_SocketMutex.unlock();

			if (packet.size > 0)
			{
				packet.data = malloc(packet.size);
				packet.size -= sizeof(size_t);
				packet.id = *(size_t*)data;
				memcpy(packet.data, (void*)((size_t*)data+1), packet.size);

				m_ReceiveData.Push(packet);
			}
			else
			{
				Sleep(1);
			}
		}
		catch(std::system_error e)
		{
			//std::cerr << e.what();

			m_SocketMutex.unlock();
		}
	}

	free(data);
}

