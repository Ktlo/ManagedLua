#include "stdafx.h"
#include "MemEnumerator.h"

#include "MemManager.h"
#include "MemObject.h"

namespace ManagedLua {

	namespace Tools {

		MemEnumerator::MemEnumerator(MemManager ^ manager) : mem(manager) {
			type = manager->enumType;
			cur = nullptr;
		}

		MemObject ^ MemEnumerator::Current::get() {
			if (cur == nullptr)
				throw gcnew InvalidOperationException;
			return gcnew MemObject(cur);
		}

		Object ^ MemEnumerator::Current2::get() {
			if (cur == nullptr)
				throw gcnew InvalidOperationException;
			return gcnew MemObject(cur);
		}

		bool MemEnumerator::MoveNext() {
			byte * data = mem->info->memory;
			byte * end = data + mem->info->size;
			if (cur == nullptr)
				cur = data;
			else
				cur += lengthof(cur);
			if (type == 0xffui8) {
				for (; cur < end; cur += lengthof(cur))
					if (gettype(cur) != 0xffui8)
						return true;
			}
			else {
				for (; cur < end; cur += lengthof(cur))
					if (gettype(cur) == type)
						return true;
			}
			return false;
		}

		void MemEnumerator::Reset() {
			cur = nullptr;
		}

		MemEnumerator::~MemEnumerator() {

		}

	}

}