/*!
	@file
	@author		Albert Semenov
	@date		11/2007
	@module
*/
namespace delegates
{
	#ifndef COMBINE
	#define COMBINE(a,b)       COMBINE1(a,b)
	#endif
	#define COMBINE1(a,b)      a##b

	#define I_DELEGATE         COMBINE(IDelegate, SUFFIX)
	#define C_STATIC_DELEGATE  COMBINE(CStaticDelegate, SUFFIX)
	#define C_METHOD_DELEGATE  COMBINE(CMethodDelegate, SUFFIX)
	#ifndef C_DELEGATE
	#define C_DELEGATE         COMBINE(CDelegate, SUFFIX)
	#endif

	// ������� ����� ���� ���������
	template<TEMPLATE_PARAMS>
	class I_DELEGATE
	{
	public:
		virtual ~I_DELEGATE() {}
		virtual void Invoke(PARAMS) = 0;
		virtual bool Compare(I_DELEGATE<TEMPLATE_ARGS>* pDelegate) = 0;
	};

	// ������� ��� ����������� �������
	template<TEMPLATE_PARAMS>
	class C_STATIC_DELEGATE : public I_DELEGATE<TEMPLATE_ARGS>
	{
	public:
		typedef void (*PFunc)(PARAMS);
		C_STATIC_DELEGATE(PFunc pFunc) { mFunc = pFunc; }
		virtual void Invoke(PARAMS) { mFunc(ARGS); }
		virtual bool Compare(I_DELEGATE<TEMPLATE_ARGS>* pDelegate)
		{
			C_STATIC_DELEGATE<TEMPLATE_ARGS>* pStaticDel =
			dynamic_cast<C_STATIC_DELEGATE<TEMPLATE_ARGS>*>(pDelegate);
			if (pStaticDel == NULL || pStaticDel->mFunc != mFunc) return false;
			return true;
		}
	private:
		PFunc mFunc;
	};

	// ������� ��� ������ ������
	template<class TObj, TEMPLATE_PARAMS>
	class C_METHOD_DELEGATE : public I_DELEGATE<TEMPLATE_ARGS>
	{
	public:
		typedef void (TObj::*PMethod)(PARAMS);
		C_METHOD_DELEGATE(TObj* pObj, PMethod pMethod)
		{
			mObject = pObj;
			mMethod = pMethod;
		}
		virtual void Invoke(PARAMS) { (mObject->*mMethod)(ARGS); }
		virtual bool Compare(I_DELEGATE<TEMPLATE_ARGS>* pDelegate)
		{
			C_METHOD_DELEGATE<TObj, TEMPLATE_ARGS>* pMethodDel =
			dynamic_cast<C_METHOD_DELEGATE<TObj, TEMPLATE_ARGS>*>(pDelegate);
			if ( pMethodDel == NULL || pMethodDel->mObject != mObject || pMethodDel->mMethod != mMethod ) return false;
			return true;
		}

	private:
		TObj *mObject;
		PMethod mMethod;
	};


	// ������ ��� �������� �������� ����������� �������
	// ��������� : ��������� �� �������
	// ������ : newDelegate(funk_name);
	template<TEMPLATE_PARAMS>
	I_DELEGATE<TEMPLATE_ARGS>* newDelegate(void (*pFunc)(PARAMS))
	{
		return new C_STATIC_DELEGATE<TEMPLATE_ARGS>(pFunc);
	}

	// ������ ��� �������� �������� ������ ������
	// ��������� : ��������� �� ������ ������ � ��������� �� ����� ������
	// ������ : newDelegate(&object_name, &class_name::method_name);
	template <class TObj, TEMPLATE_PARAMS>
	I_DELEGATE<TEMPLATE_ARGS>* newDelegate(TObj* pObj, void (TObj::*pMethod)(PARAMS))
	{
		return new C_METHOD_DELEGATE<TObj, TEMPLATE_ARGS> (pObj, pMethod);
	}


	// ������ ������ ��������
	template<TEMPLATE_PARAMS>
	class C_DELEGATE
	{
	public:
		typedef I_DELEGATE<TEMPLATE_ARGS> IDelegate;

		C_DELEGATE() : mDelegate (0) {}
		~C_DELEGATE()
		{
			if (mDelegate) {
				delete mDelegate;
			}
		}

		bool IsNull() { return (mDelegate == 0); }

		C_DELEGATE<TEMPLATE_ARGS>& operator=(IDelegate* pDelegate)
		{
			if (mDelegate) {
				delete mDelegate;
			}
			mDelegate = pDelegate;
			return *this;
		}

		void operator()(PARAMS)
		{
			if (mDelegate == 0) return;
			mDelegate->Invoke(ARGS);
		}

	private:
		IDelegate * mDelegate;
	};

} // namespace delegates
