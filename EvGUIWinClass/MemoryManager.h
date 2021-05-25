#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>
class MemoryManager
{
public:

	enum ret_num
	{
		SUCCESS = 0,
		FAILURE = -1,
		ATTACH_FAIL = -2,
		ALLOC_FAIL  = -3,
		PROC_NOT_FOUND = -4,
		WRITE_FAIL = -5,
		READ_FAIL = -6
	};

	struct proc_struct
	{
		std::string process_name;
		DWORD       process_id;
		HANDLE      process_handle;
	};

	class MemoryManagerException 
		: std::exception
	{

	public:

		MemoryManagerException(
			const ret_num a_ret, 
			const int line, 
			const char* file);

		const std::string GetReason()
		{
			return reason_str;
		};

	private:

		std::string reason_str;

		
	};

	MemoryManager();

	MemoryManager(const std::string& a_process_name);

	template <class DataType>
	ret_num WriteToProcess(
		const LPVOID& addr,  // IN
		DataType& in_val     // IN
	);

	template <class DataType>
	ret_num ReadFromProcess(
		const LPVOID& addr,  // IN
		DataType& out_val    // OUT
	);

	LPVOID AllocInProcess(SIZE_T& mem_size);

	BYTE* GetBaseAddr() { return d_base_addr; }

	HANDLE GetHandle() { return d_proc_handle;  }

private:

	HANDLE d_proc_handle;

	ret_num ret_status;

	std::string process_name;

	proc_struct d_proc_struct;

	BYTE* d_base_addr;

	//PROCESSENTRY32 d_proc_entry;

	ret_num AttachToProcess(const std::string&);
	
};

template <class DataType>
MemoryManager::ret_num
MemoryManager::ReadFromProcess(
	const LPVOID& addr,  // IN
	DataType& out_val    // OUT
)
{
	if (ReadProcessMemory(
		d_proc_handle,
		addr,
		&out_val,
		sizeof(out_val),
		NULL) == 0)
	{
		ret_status = READ_FAIL;
		throw MemoryManagerException(
			ret_status,
			__LINE__,
			__FILE__);
	}
	ret_status = SUCCESS;
	return ret_status;
}

template <class DataType>
MemoryManager::ret_num
MemoryManager::WriteToProcess(
	const LPVOID& addr,  // IN
	DataType& in_val    // OUT
)
{
	if (WriteProcessMemory(
		d_proc_handle,
		addr,
		&in_val,
		sizeof(in_val),
		NULL) == 0)
	{
		ret_status = WRITE_FAIL;
		throw MemoryManagerException(
			ret_status,
			__LINE__,
			__FILE__);
	}
	ret_status = SUCCESS;
	return ret_status;
}