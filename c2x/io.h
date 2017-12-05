#pragma once

enum seek_flags { SeekSet, SeekCur, SeekEnd };
enum stream_flags
{
	StreamRead = 1,
	StreamWrite = 2,
	StreamText = 4,
};

namespace io
{
	// Abstract bi-stream interface
	struct stream
	{
		unsigned			getsize();
		virtual int			read(void* result, int count) = 0;
		virtual int			seek(int count, int rel = SeekCur) { return 0; };
		virtual int			write(const void* result, int count) = 0;
	};
	struct file : public stream
	{
		struct find
		{
			find(const char* url);
			~find();
			const char*		name();
			const char*		fullname(char* result);
			void			next();
			operator bool() const { return handle!=0; }
		private:
			char			path[261];
			char			reserved[512];
			void*			handle;
		};
		file();
		file(const char* url, unsigned flags = StreamRead);
		~file();
		operator bool() const { return handle!=0; }
		void				close();
		bool				create(const char* url, unsigned flags);
		static bool			exist(const char* url);
		static char*		getdir(char* url, int size);
		static char*		getmodule(char* url, int size);
		static bool			makedir(const char* url);
		int					read(void* result, int count) override;
		static bool			remove(const char* url);
		int					seek(int count, int rel) override;
		static bool			setdir(const char* url);
		int					write(const void* result, int count) override;
	private:
		int					handle;
	};
}