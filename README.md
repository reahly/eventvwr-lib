# eventvwr-lib

Example Usage:
```c
win32evtvwr::init( L"", L"Application", []( const std::vector<win32evtvwr::event_log_record_t>& logs ) -> void {
	for ( const auto& log : logs ) {
		printf( "record_number: %lu \n", log.record_number );
		printf( "event_id: %lu \n", log.event_id );
		printf( "time_generated: %lu \n", log.time_generated );
		printf( "time_written: %lu \n", log.time_written );
		printf( "event_type: %i \n", log.event_type );
		printf( "event_category: %i \n", log.event_category );
		printf( "source_name: %ls \n", log.source_name.c_str( ) );
		printf( "computer_name: %ls \n", log.computer_name.c_str( ) );
	}
} );
```
