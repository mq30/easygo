#ifndef EASYGO_BASE_CBASE_H
#define EASYGO_BASE_CBASE_H
#include "../include/declexport.h"

#ifndef EASYGO_BASE_DECL
#	ifdef EASYGO_DYNAMIC_LINK
#		ifdef EXPORT_BASE
#			define EASYGO_BASE_DECL EASYGO_DECL_EXPORT
#		else
#			define EASYGO_BASE_DECL EASYGO_DECL_IMPORT
#		endif
#	else
#		define EASYGO_BASE_DECL
#	endif
#endif

namespace easygo {
	namespace base {

		template<class T>
		class CBase
		{
		public:
			CBase() : m_pCounter(NULL)
			{
			}

			virtual ~CBase() 
			{
				if (NULL != m_pCounter && m_pCounter->DecRef())
				{
					delete m_pCounter;
				}
			}

		public:
			T* GetSelf() const 
			{
				return m_pCounter;
			}

			T* operator->() const
			{
				return m_pCounter;
			}

			operator bool() const
			{
				return NULL != m_pCounter;
			}

			bool operator!() const
			{
				return NULL == m_pCounter;
			}

			void Swap(CBase& base)
			{
				T* pTmpCounter = m_pCounter;
				m_pCounter = base.m_pCounter;
				base.m_pCounter = pTmpCounter;
			}

			void Assign(T* pCounter)
			{
				if (NULL != m_pCounter)
				{
					m_pCounter->DecRef();
				}

				m_pCounter = pCounter;
				if (NULL != m_pCounter)
				{
					m_pCounter->IncRef();
				}
			}

		public:
			CBase(T* pCounter)
			{
				m_pCounter = pCounter;
				if (NULL != m_pCounter)
				{
					m_pCounter->IncRef();
				}
			}

			CBase(const CBase& base) : m_pCounter(base.m_pCounter)
			{
				if (NULL != m_pCounter)
				{
					m_pCounter->IncRef();
				}
			}

			CBase& operator=(const CBase& base)
			{
				if (NULL != m_pCounter)
				{
					m_pCounter->DecRef();
				}

				m_pCounter = base.m_pCounter;

				if (NULL != m_pCounter)
				{
					m_pCounter->IncRef();
				}

				return *this;
			}

			CBase& operator=(T* pCounter)
			{
				if (NULL != m_pCounter)
				{
					m_pCounter->DecRef();
				}

				m_pCounter = pCounter;

				if (NULL != m_pCounter)
				{
					m_pCounter->IncRef();
				}

				return *this;
			}

			T& operator*() const
			{
				return *m_pCounter;
			}

		private:
			T* m_pCounter;
		};

		template<class TX, class TY>
		inline bool operator==(const CBase<TX>& baseX, const CBase<TY>& baseY)
		{
			return baseX.GetSelf() == baseY.GetSelf();
		}

		template<class TX, class TY>
		inline bool operator!=(const CBase<TX>& baseX, const CBase<TY>& baseY)
		{
			return baseX.GetSelf() != baseY.GetSelf();
		}

		template<class TX, class TY>
		inline bool operator==(const CBase<TX>& baseX, TY* baseY)
		{
			return baseX.GetSelf() == baseY;
		}

		template<class TX, class TY>
		inline bool operator!=(const CBase<TX>& baseX, TY* baseY)
		{
			return baseX.GetSelf() != baseY;
		}

		template<class TX, class TY>
		inline bool operator==(TX* baseX, const CBase<TY>& baseY)
		{
			return baseX == baseY.GetSelf();
		}

		template<class TX, class TY>
		inline bool operator!=(TX* baseX, const CBase<TY>& baseY)
		{
			return baseX != baseY.GetSelf();
		}

	} // namespace base
} // namespace easygo

#endif // EASYGO_BASE_CBASE_H