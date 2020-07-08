#pragma once

template <typename T>
class WSingleton {
public:
	static T* Instance() {
		return m_pInstance;
	}

	WSingleton() {
		m_pInstance = static_cast<T*>(this);
	}

	~WSingleton() {
		m_pInstance = nullptr;
	}

	WSingleton(const WSingleton&) = delete;
	WSingleton& operator=(const WSingleton&) = delete;
	WSingleton(WSingleton&&) = delete;
	WSingleton& operator=(WSingleton&&) = delete;

private:
	inline static T* m_pInstance = nullptr;
};
