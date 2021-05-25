#include <sstream>
#include "MemoryManager.h"

MemoryManager::MemoryManager(
	const std::string& a_process_name)
{
	process_name = a_process_name;

	ret_status = AttachToProcess(process_name);



	if (ret_status < 0)
	{
		throw MemoryManagerException(
				ret_status,
				__LINE__ - 6,
				__FILE__);
	}

}

MemoryManager::MemoryManager()
{
	d_proc_handle = OpenProcess(
						PROCESS_ALL_ACCESS, 
						FALSE, 
						GetCurrentProcessId());
}

MemoryManager::ret_num
MemoryManager::AttachToProcess(
	const std::string& a_process_name)
{
	PROCESSENTRY32 process_entry;
	MODULEENTRY32  module_entry;
	
	HANDLE process_itr = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	bool proc_found = false;
	while (Process32Next(process_itr, &process_entry))
	{
		if (strcmp(
				process_entry.szExeFile, 
				a_process_name.c_str()) == 0)
		{
			proc_found = true;
			d_proc_handle = OpenProcess(
								PROCESS_ALL_ACCESS, 
								FALSE, 
								process_entry.th32ProcessID);

			HANDLE module_itr = CreateToolhelp32Snapshot(
									TH32CS_SNAPMODULE, 
									process_entry.th32ProcessID);

			while (Module32Next(module_itr, &module_entry))
			{
				if (strcmp(
					module_entry.szModule,
					a_process_name.c_str()) == 0)
				{
					d_base_addr = module_entry.modBaseAddr; 
					break;
				}
			}
			return SUCCESS;
		}
	}

	return PROC_NOT_FOUND;
}


MemoryManager::MemoryManagerException::
	MemoryManagerException(
		const MemoryManager::ret_num a_ret,
		const int line,
		const char* file)
{
	std::ostringstream reason_buffer;

	reason_buffer <<
		"Exception caught: " << std::endl << '\t' <<
		file << ":" << line << std::endl << '\t' <<
		"REASON: ";

	switch (a_ret)
	{
	case FAILURE:
		reason_buffer <<
			"General Failure Error";
		break;

	case ATTACH_FAIL:
		reason_buffer <<
			"Failed to attach to process";
		break;

	case PROC_NOT_FOUND:
		reason_buffer <<
			"Process not found";
		break;

	case WRITE_FAIL:
		reason_buffer <<
			"Failed to write to process memory";
		break;

	case READ_FAIL:
		reason_buffer <<
			"Failed to read from process memory";
		break;

	case ALLOC_FAIL:
		LPSTR lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);
		reason_buffer << "Failed to allocate memory: " << std::endl
					  << '\t' << "DETAIL: " << lpMsgBuf;
		break;
	}
	reason_buffer << std::endl;

	reason_str = reason_buffer.str();
}

//
// Return the memory address that is allocated
//
LPVOID MemoryManager::AllocInProcess(SIZE_T& mem_size)
{
	LPVOID return_value;
	
	return_value = VirtualAllocEx(
		d_proc_handle,
		NULL,
		mem_size,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_READWRITE);

	if (return_value)
		return return_value;
	else
	{
		throw MemoryManagerException(
			ALLOC_FAIL, __LINE__, __FILE__);
	}
}