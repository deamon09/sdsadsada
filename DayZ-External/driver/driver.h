#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include "../game/offsets.h"

#define DRIVER_DEVICE_NAME L"\\\\.\\\paysoniscoolio"
#define IOCTL_RW CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1645, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_BA CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1646, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_GUARDED CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1647, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define DRIVER_SECURITY 0x85b3b69

typedef struct _DRIVER_RW {
	INT32 security;
	INT32 process_id;
	ULONGLONG address;
	ULONGLONG buffer;
	ULONGLONG size;
	BOOLEAN write;
} DRIVER_RW, *PDRIVER_RW;

typedef struct _DRIVER_BA {
	INT32 security;
	INT32 process_id;
	ULONGLONG* address;
} DRIVER_BA, *PDRIVER_BA;

typedef struct _DRIVER_GA {
	INT32 security;
	ULONGLONG* address;
} DRIVER_GA, *PDRIVER_GA;

namespace Driver {
	inline HANDLE handle = INVALID_HANDLE_VALUE;
	inline INT32 process_id = 0;
	inline uintptr_t base_address = 0;
	inline uintptr_t guarded_region = 0;
	inline std::mutex io_mutex;

	inline bool IsValid() {
		return handle != INVALID_HANDLE_VALUE && process_id != 0;
	}

	inline bool Connect() {
		if (handle != INVALID_HANDLE_VALUE)
			return true;

		handle = CreateFileW(DRIVER_DEVICE_NAME, GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);

		return handle != INVALID_HANDLE_VALUE;
	}

	inline void Disconnect() {
		if (handle != INVALID_HANDLE_VALUE) {
			CloseHandle(handle);
			handle = INVALID_HANDLE_VALUE;
		}
	}

	inline INT32 FindProcess(const wchar_t* process_name) {
		PROCESSENTRY32W entry = { sizeof(PROCESSENTRY32W) };
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		
		if (snapshot == INVALID_HANDLE_VALUE)
			return 0;

		if (Process32FirstW(snapshot, &entry)) {
			do {
				if (_wcsicmp(entry.szExeFile, process_name) == 0) {
					CloseHandle(snapshot);
					process_id = entry.th32ProcessID;
					return entry.th32ProcessID;
				}
			} while (Process32NextW(snapshot, &entry));
		}

		CloseHandle(snapshot);
		return 0;
	}

	inline bool ReadPhysical(uintptr_t address, void* buffer, size_t size) {
		if (!IsValid() || !address || !buffer || !size)
			return false;

		DRIVER_RW args = {};
		args.security = DRIVER_SECURITY;
		args.process_id = process_id;
		args.address = address;
		args.buffer = reinterpret_cast<ULONGLONG>(buffer);
		args.size = size;
		args.write = FALSE;

		std::lock_guard<std::mutex> lock(io_mutex);
		return DeviceIoControl(handle, IOCTL_RW, &args, sizeof(args), 
			nullptr, 0, nullptr, nullptr);
	}

	inline bool WritePhysical(uintptr_t address, void* buffer, size_t size) {
		if (!IsValid() || !address || !buffer || !size)
			return false;

		DRIVER_RW args = {};
		args.security = DRIVER_SECURITY;
		args.process_id = process_id;
		args.address = address;
		args.buffer = reinterpret_cast<ULONGLONG>(buffer);
		args.size = size;
		args.write = TRUE;

		std::lock_guard<std::mutex> lock(io_mutex);
		return DeviceIoControl(handle, IOCTL_RW, &args, sizeof(args), 
			nullptr, 0, nullptr, nullptr);
	}

	inline uintptr_t GetBaseAddress() {
		if (base_address)
			return base_address;

		if (!IsValid())
			return 0;

		DRIVER_BA args = {};
		args.security = DRIVER_SECURITY;
		args.process_id = process_id;
		args.address = &base_address;

		std::lock_guard<std::mutex> lock(io_mutex);
		DeviceIoControl(handle, IOCTL_BA, &args, sizeof(args), 
			nullptr, 0, nullptr, nullptr);

		return base_address;
	}

	inline uintptr_t GetGuardedRegion() {
		if (guarded_region)
			return guarded_region;

		if (!IsValid())
			return 0;

		DRIVER_GA args = {};
		args.security = DRIVER_SECURITY;
		args.address = &guarded_region;

		std::lock_guard<std::mutex> lock(io_mutex);
		DeviceIoControl(handle, IOCTL_GUARDED, &args, sizeof(args), 
			nullptr, 0, nullptr, nullptr);

		return guarded_region;
	}

	template<typename T>
	inline bool ReadArray(uintptr_t address, T* buffer, size_t count) {
		return ReadPhysical(address, buffer, sizeof(T) * count);
	}

	template<typename T>
	inline bool WriteArray(uintptr_t address, T* buffer, size_t count) {
		return WritePhysical(address, buffer, sizeof(T) * count);
	}
}

template<typename T>
inline T Read(uintptr_t address) {
	T buffer{};
	Driver::ReadPhysical(address, &buffer, sizeof(T));
	return buffer;
}

template<typename T>
inline bool Write(uintptr_t address, const T& value) {
	return Driver::WritePhysical(address, const_cast<T*>(&value), sizeof(T));
}

// Compatibility aliases
template<typename T>
inline T read(uintptr_t address) { return Read<T>(address); }

template<typename T>
inline bool write(uintptr_t address, const T& value) { return Write<T>(address, value); }

inline std::string ReadString(uintptr_t address, size_t max_length = 256) {
	if (!address || max_length == 0 || max_length > 4096)
		return "";

	std::vector<char> buffer(max_length + 1, 0);
	if (!Driver::ReadPhysical(address, buffer.data(), max_length))
		return "";

	buffer[max_length] = '\0';
	return std::string(buffer.data());
}

inline std::string ReadArmaString(uintptr_t address) {
	if (!address)
		return "";

	int length = Read<int>(address + 0x8);
	if (length <= 0 || length > 1024)
		return "";

	return ReadString(address + 0x10, static_cast<size_t>(length));
}

inline std::wstring ReadWString(uintptr_t address, size_t max_length = 256) {
	if (!address || max_length == 0 || max_length > 4096)
		return L"";

	std::vector<wchar_t> buffer(max_length + 1, 0);
	if (!Driver::ReadPhysical(address, buffer.data(), max_length * sizeof(wchar_t)))
		return L"";

	buffer[max_length] = L'\0';
	return std::wstring(buffer.data());
}

namespace mem = Driver;
