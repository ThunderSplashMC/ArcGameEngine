#pragma once

#include <cstdint>
#include <cstring>

namespace ArcEngine
{
	struct Buffer
	{
		uint8_t* Data = nullptr;
		uint64_t Size = 0;

		Buffer() = default;
		Buffer(uint64_t size) {	Allocate(size); }

		static Buffer Cope(Buffer other)
		{
			Buffer result = Buffer(other.Size);
			memcpy_s(result.Data, result.Size, other.Data, other.Size);
			return result;
		}

		void Allocate(uint64_t size)
		{
			Release();

			Data = new uint8_t[size];
			Size = size;
		}

		void Release()
		{
			delete[] Data;
			Data = nullptr;
			Size = 0;
		}

		template<typename T>
		T* As() { return (T*)Data; }

		operator bool() const {	return (bool)Data; }
	};

	struct ScopedBuffer
	{
		ScopedBuffer(Buffer buffer)
			: m_Buffer(buffer)
		{
		}

		ScopedBuffer(uint64_t size)
			: m_Buffer(size)
		{
		}

		~ScopedBuffer() { m_Buffer.Release(); }

		uint8_t* Data() { return m_Buffer.Data; }
		uint64_t Size() { return m_Buffer.Size; }

		template<typename T>
		T* As() { return m_Buffer.As<T>(); }

		operator bool() const { return m_Buffer; }

	private:
		Buffer m_Buffer;
	};
}