#include <sstream>
#include <iostream>
#include <random>
#include <cstdint>
#include <cstring>
#include "../include/base/CUuid.h"

namespace easygo {
	namespace base {

		#define THIRTY_TWO_ZEROS "00000000000000000000000000000000"
		static const unsigned char TABLE[4] = { '8', '9', 'a', 'b' };

		CUuid::CUuid() 
		{
			GenerateRandomUUID();
		}

		CUuid::~CUuid() 
		{
		}

		CUuid::CUuid(const CUuid& uuid) 
		{
			memcpy(m_uuid.data, uuid.m_uuid.data, sizeof(m_uuid));
		}

		CUuid::CUuid(const std::string& uuid) 
		{
			std::string sec;
			std::stringstream ss(uuid);
			int i = 0;
			int j;

			while (std::getline(ss, sec, '-')) 
			{
				for (j = 0; j < sec.length(); j++) 
				{
					m_uuid.data[i] = sec.at(j);
					i++;
					if (sizeof(m_uuid.data) == i)
					{
						break;
					}
				}

				if (sizeof(m_uuid.data) == i)
				{
					break;
				}
			}
		}

		CUuid& CUuid::operator=(const CUuid& uuid) 
		{
			if (this != &uuid)
			{
				memcpy(m_uuid.data, uuid.m_uuid.data, sizeof(m_uuid));
			}

			return *this;
		}

		bool CUuid::IsNil() const 
		{
			for (int i = 0; i < sizeof(m_uuid.data); i++)
			{
				if ('0' != m_uuid.data[i])
				{
					return false;
				}
			}

			return true;
		}

		unsigned char CUuid::GetVersion(void) const 
		{
			return m_uuid.groups.group3[0];
		}

		std::string CUuid::String() const
		{
			std::string ret;
			std::stringstream ss;
			int i;

			for (i = 0; i < sizeof(m_uuid.groups.group1); i++)
			{
				ss << m_uuid.groups.group1[i];
			}
			ss << '-';

			for (i = 0; i < sizeof(m_uuid.groups.group2); i++)
			{
				ss << m_uuid.groups.group2[i];
			}
			ss << '-';

			for (i = 0; i < sizeof(m_uuid.groups.group3); i++)
			{
				ss << m_uuid.groups.group3[i];
			}
			ss << '-';

			for (i = 0; i < sizeof(m_uuid.groups.group4); i++)
			{
				ss << m_uuid.groups.group4[i];
			}
			ss << '-';

			for (i = 0; i < sizeof(m_uuid.groups.group5); i++)
			{
				ss << m_uuid.groups.group5[i];
			}

			ss >> ret;

			return ret;
		}

		std::string CUuid::GenerateUUID() {
			return CUuid().String();
		}

		void CUuid::GenerateRandomUUID() {
			std::random_device rd;
			std::mt19937 generator(rd());
			std::uniform_int_distribution<int64_t> distN(INT64_MIN, INT64_MAX);
			std::uniform_int_distribution<int> distY(0, 3);

			std::string ns;
			std::stringstream ss;

			ss << std::hex << distN(generator);
			ss << std::hex << distN(generator);
			ss << THIRTY_TWO_ZEROS;
			ss >> ns;

			int size = (int)sizeof(m_uuid.data);
			ns = ns.substr(0, size);

			for (int i = 0; i < size; i++)
			{
				m_uuid.data[i] = ns.at(i);
			}

			m_uuid.groups.group3[0] = '4'; // version 4
			m_uuid.groups.group4[0] = TABLE[distY(generator)];
		}

	} // namespace base
} // namespace easygo