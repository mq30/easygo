#include "../include/network/CActor.h"
#include "../include/network/CNetBase.h"
#include "../include/network/CConnector.h"
#include "../include/network/CSocket.h"
#include "../include/network/CListener.h"

namespace easygo {
	namespace network {

		CActor::CActor(PCNetBase netBase, PCConnector connector) : CConnection(netBase)
		{
			SetConnector(connector);
		}

		CActor::~CActor()
		{
		}

		void CActor::OnConnectComplete()
		{
			if (GetConnector())
			{
				GetConnector()->OnConnectComplete();
			}
		}

		void CActor::OnReadComplete(const char * buf, int len)
		{
			if (GetConnector())
			{
				GetConnector()->OnReadComplete(buf, len);
			}
		}

		void CActor::OnWriteComplete(int len)
		{
			if (NET_SOCK_CONNECTING == GetSockState())
			{
				SetSockState(NET_SOCK_CONNECTED);
				OnConnectComplete();
				return;
			}

			if (GetConnector())
			{
				GetConnector()->OnWriteComplete(len);
			}
		}

		void CActor::OnCloseComplete()
		{
			if (GetConnector())
			{
				GetConnector()->OnCloseComplete();
			}
		}

	} // namespace network
} // namespace easygo