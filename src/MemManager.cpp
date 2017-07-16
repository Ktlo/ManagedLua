#include "stdafx.h"
#include "MemManager.h"

#include "DataType.h"
#include "MemEnumerator.h"

namespace ManagedLua {

	namespace Tools {

		MemManager::MemManager(size_t size) {
			info = new memInfo{ size, (byte *)std::malloc(size) };
			(*(info->memory + sizeof(size_t)) = 0xffui8);
			(*(size_t *)(info->memory) = (size));
			enumType = 0xffui8;
		}

		inline size_t MemManager::HeaderSize::get() {
			return DELTA;
		}

		inline size_t MemManager::Size::get() {
			return info->size;
		}

		size_t MemManager::Count(Byte type) {
			byte * cur = info->memory;
			byte * end = cur + info->size;
			size_t result = 0;
			if (type == 0xffui8) {
				for (; cur < end; cur += lengthof(cur))
					if (gettype(cur) != 0xffui8)
						result++;
			}
			else {
				for (; cur < end; cur += lengthof(cur))
					if (gettype(cur) == type)
						result++;
			}
			return result;
		}

		inline size_t MemManager::Count(DataType type) {
			return Count((Byte)type);
		}

		size_t MemManager::Count() {
			return Count(0xffui8);
		}

		size_t MemManager::MeasureAll(Byte type) {
			byte * cur = info->memory;
			byte * end = cur + info->size;
			size_t result = 0;
			if (type == 0xffui8) {
				for (; cur < end; cur += lengthof(cur))
					if (gettype(cur) != 0xffui8)
						result += lengthof(cur) - DELTA;
			}
			else {
				for (; cur < end; cur += lengthof(cur))
					if (gettype(cur) == type)
						result += lengthof(cur) - DELTA;
			}
			return result;
		}

		inline size_t MemManager::MeasureAll(DataType type) {
			return MeasureAll((Byte)type);
		}

		size_t MemManager::MeasureAll() {
			return MeasureAll(0xffui8);
		}

		MemManager ^ MemManager::EnumerateByType(DataType type) {
			enumType = (byte)type;
			return this;
		}

		MemManager ^ MemManager::EnumerateByType(Byte type) {
			enumType = type;
			return this;
		}

		System::Collections::Generic::IEnumerator<MemObject ^> ^ MemManager::GetEnumerator() {
			return gcnew MemEnumerator(this);
		}

		System::Collections::IEnumerator ^ MemManager::GetEnumerator2() {
			return gcnew MemEnumerator(this);
		}

	}

}