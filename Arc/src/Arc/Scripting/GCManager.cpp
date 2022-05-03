#include "arcpch.h"
#include "GCManager.h"

#include <mono/metadata/object.h>
#include <mono/metadata/mono-gc.h>

namespace ArcEngine
{
	struct GCState
	{
		std::unordered_map<GCHandle, MonoObject*> StrongRefMap;
		std::unordered_map<GCHandle, MonoObject*> WeakRefMap;
	};

	static GCState* s_GCState;
	void GCManager::Init()
	{
		OPTICK_EVENT();

		s_GCState = new GCState();
	}

	void GCManager::Shutdown()
	{
		OPTICK_EVENT();

		if (s_GCState->StrongRefMap.size() > 0)
		{
			ARC_CORE_ERROR("Memory leak detected\n Not all GCHandles have been cleaned up!");

			for (auto& [handle, object] : s_GCState->StrongRefMap)
				mono_gchandle_free_v2(handle);

			s_GCState->StrongRefMap.clear();
		}

		if (s_GCState->WeakRefMap.size() > 0)
		{
			ARC_CORE_ERROR("Memory leak detected\n Not all GCHandles have been cleaned up!");

			for (auto& [handle, object] : s_GCState->WeakRefMap)
				mono_gchandle_free_v2(handle);

			s_GCState->WeakRefMap.clear();
		}

		mono_gc_collect(mono_gc_max_generation());
		while (mono_gc_pending_finalizers());

		delete s_GCState;
		s_GCState = nullptr;
	}

	void GCManager::CollectGarbage(bool blockUntilFinalized)
	{
		OPTICK_EVENT();

		mono_gc_collect(mono_gc_max_generation());
		if (blockUntilFinalized)
		{
			while(mono_gc_pending_finalizers());
		}
	}

	GCHandle GCManager::CreateObjectReference(MonoObject* managedObject, bool weakReference, bool pinned, bool track)
	{
		OPTICK_EVENT();

		GCHandle handle = weakReference
			? mono_gchandle_new_weakref_v2(managedObject, pinned)
			: mono_gchandle_new_v2(managedObject, pinned);

		ARC_CORE_ASSERT(handle, "Failed to get valid GC Handle!");

		if (track)
		{
			if (weakReference)
				s_GCState->WeakRefMap.emplace(handle, managedObject);
			else
				s_GCState->StrongRefMap.emplace(handle, managedObject);
		}

		return handle;
	}

	MonoObject* GCManager::GetReferencedObject(GCHandle handle)
	{
		OPTICK_EVENT();

		MonoObject* obj = mono_gchandle_get_target_v2(handle);
		if (obj == nullptr || mono_object_get_vtable(obj) == nullptr)
			return nullptr;
		return obj;
	}

	void GCManager::ReleaseObjectReference(GCHandle handle)
	{
		OPTICK_EVENT();

		if (mono_gchandle_get_target_v2(handle) != nullptr)
		{
			mono_gchandle_free_v2(handle);
		}
		else
		{
			ARC_CORE_ERROR("Trying to release an object reference using invalid handle");
			return;
		}

		if (s_GCState->StrongRefMap.find(handle) != s_GCState->StrongRefMap.end())
			s_GCState->StrongRefMap.erase(handle);

		if (s_GCState->WeakRefMap.find(handle) != s_GCState->WeakRefMap.end())
			s_GCState->WeakRefMap.erase(handle);
	}
}