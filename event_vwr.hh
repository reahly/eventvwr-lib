#pragma once
#include <functional>
#include <memory>
#include <string>
#include <Windows.h>

namespace win32evtvwr {
	struct event_log_record_t {
		DWORD record_number, event_id, time_generated, time_written;
		WORD event_type, event_category;
		std::wstring source_name, computer_name;
	};

	inline std::vector<event_log_record_t> setup( const HANDLE log, const DWORD flags ) {
		DWORD bytes_read, min_bytes_read;
		DWORD needed = 0x10000;
		std::unique_ptr<BYTE[]> buf( new BYTE[needed] );

		std::vector<event_log_record_t> records;
		auto status = ERROR_SUCCESS;
		while ( status == ERROR_SUCCESS ) {
			if ( !ReadEventLogW( log, flags, 0, buf.get( ), needed, &bytes_read, &min_bytes_read ) ) {
				status = GetLastError( );
				if ( status == ERROR_INSUFFICIENT_BUFFER ) {
					buf.reset( new BYTE[min_bytes_read] );
					needed = min_bytes_read;
				}
			}

			auto record = buf.get( );
			const auto size_loop = record + bytes_read;
			while ( record < size_loop ) {
				const auto ptr = reinterpret_cast<PEVENTLOGRECORD>( record );

				const auto source_name = reinterpret_cast<const wchar_t*>( reinterpret_cast<uintptr_t>( ptr ) + sizeof( EVENTLOGRECORD ) );
				records.emplace_back( event_log_record_t{
						ptr->RecordNumber,
						ptr->EventID & 0x1FFFFFFF,
						ptr->TimeGenerated,
						ptr->TimeWritten,
						ptr->EventType,
						ptr->EventCategory,
						std::wstring( source_name ),
						std::wstring( source_name + wcslen( source_name ) + 1 )
					} );

				record += reinterpret_cast<PEVENTLOGRECORD>( record )->Length;
			}
		}
		return records;
	}

	inline bool init( const std::wstring& server_name, const std::wstring& event_name, const std::function<void( std::vector<event_log_record_t> )>& callback, const DWORD flags = EVENTLOG_SEQUENTIAL_READ | EVENTLOG_BACKWARDS_READ ) {
		const auto event_log = OpenEventLogW( server_name.c_str( ), event_name.c_str( ) );
		if ( !event_log || event_log == INVALID_HANDLE_VALUE )
			return false;

		callback( setup( event_log, flags ) );

		CloseEventLog( event_log );
		return true;
	}
}
