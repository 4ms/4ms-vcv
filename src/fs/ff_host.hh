#pragma once
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <string>

namespace MetaModule
{

struct File {
	std::FILE *fil{};

	File() {
		reset();
	}

	void reset() {
		// fil.obj.fs = nullptr;
		if (fil) {
			std::fclose(fil);
			delete fil;
			fil = nullptr;
		}
	}

	bool is_reset() {
		return fil == nullptr;
		// return fil.obj.fs == nullptr;
	}
};

struct Fileinfo {
	bool dir_entry = false;
	std::string name;

	const char *fname() {
		return name.c_str();
	}

	bool is_dir() {
		return dir_entry;
	}
};

struct Dir {
	std::filesystem::path path = "";
	std::filesystem::directory_iterator dir{};

	Dir() {
	}

	void reset() {
		if (std::filesystem::is_directory(path))
			dir = std::filesystem::directory_iterator{path};
		else
			dir = std::filesystem::directory_iterator{};
	}

	bool is_reset() {
		if (std::filesystem::is_directory(path))
			return dir == std::filesystem::directory_iterator{path};
		else
			return false;
	}
};

} // namespace MetaModule

using BYTE = uint8_t;
typedef unsigned int UINT;	/* int must be 16-bit or 32-bit */
typedef unsigned char BYTE; /* char must be 8-bit */
typedef uint16_t WORD;		/* 16-bit unsigned integer */
typedef uint32_t DWORD;		/* 32-bit unsigned integer */
typedef uint64_t QWORD;		/* 64-bit unsigned integer */
typedef WORD WCHAR;			/* UTF-16 character type */
typedef QWORD FSIZE_t;
typedef QWORD LBA_t;
typedef char TCHAR;

typedef enum {
	FR_OK = 0,				/* (0) Succeeded */
	FR_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer */
	FR_INT_ERR,				/* (2) Assertion failed */
	FR_NOT_READY,			/* (3) The physical drive cannot work */
	FR_NO_FILE,				/* (4) Could not find the file */
	FR_NO_PATH,				/* (5) Could not find the path */
	FR_INVALID_NAME,		/* (6) The path name format is invalid */
	FR_DENIED,				/* (7) Access denied due to prohibited access or directory full */
	FR_EXIST,				/* (8) Access denied due to prohibited access */
	FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	FR_NOT_ENABLED,			/* (12) The volume has no work area */
	FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
	FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any problem */
	FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	FR_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
	FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	FR_TOO_MANY_OPEN_FILES, /* (18) Number of open files > FF_FS_LOCK */
	FR_INVALID_PARAMETER	/* (19) Given parameter is invalid */
} FRESULT;

enum {
	FA_READ = 0x01,
	FA_WRITE = 0x02,
	FA_OPEN_EXISTING = 0x00,
	FA_CREATE_NEW = 0x04,
	FA_CREATE_ALWAYS = 0x08,
	FA_OPEN_ALWAYS = 0x10,
	FA_OPEN_APPEND = 0x30,
};

#define FF_MAX_LFN 255
#define FF_LFN_BUF 255

// /* Fast seek controls (2nd argument of f_lseek) */
// #define CREATE_LINKMAP ((FSIZE_t)0 - 1)

// struct FATFS;

// typedef struct {
// 	FATFS *fs; /* Pointer to the hosting volume of this object */
// 	WORD id;   /* Hosting volume mount ID */
// 	BYTE attr; /* Object attribute */
// 	BYTE
// 		stat; /* Object chain status (b1-0: =0:not contiguous, =2:contiguous, =3:fragmented in this session, b2:sub-directory stretched) */
// 	DWORD sclust;	 /* Object data start cluster (0:no cluster or root directory) */
// 	FSIZE_t objsize; /* Object size (valid when sclust != 0) */
// 	DWORD n_cont;	 /* Size of first fragment - 1 (valid when stat == 3) */
// 	DWORD n_frag;	 /* Size of last fragment needs to be written to FAT (valid when not zero) */
// 	DWORD c_scl;	 /* Containing directory start cluster (valid when sclust != 0) */
// 	DWORD c_size;	 /* b31-b8:Size of containing directory, b7-b0: Chain status (valid when c_scl != 0) */
// 	DWORD c_ofs;	 /* Offset in the containing directory (valid when file object and sclust != 0) */
// #if FF_FS_LOCK
// 	UINT lockid; /* File lock ID origin from 1 (index of file semaphore table Files[]) */
// #endif
// } FFOBJID;

// typedef struct {
// 	FFOBJID obj;		 /* Object identifier (must be the 1st member to detect invalid object pointer) */
// 	BYTE flag;			 /* File status flags */
// 	BYTE err;			 /* Abort flag (error code) */
// 	FSIZE_t fptr;		 /* File read/write pointer (Zeroed on file open) */
// 	DWORD clust;		 /* Current cluster of fpter (invalid when fptr is 0) */
// 	LBA_t sect;			 /* Sector number appearing in buf[] (0:invalid) */
// 	LBA_t dir_sect;		 /* Sector number containing the directory entry (not used at exFAT) */
// 	BYTE *dir_ptr;		 /* Pointer to the directory entry in the win[] (not used at exFAT) */
// 	DWORD *cltbl;		 /* Pointer to the cluster link map table (nulled on open, set by application) */
// 	BYTE buf[FF_MAX_SS]; /* File private data read/write window */
// } FIL;

// typedef struct {
// 	FFOBJID	obj;			/* Object identifier */
// 	DWORD	dptr;			/* Current read/write offset */
// 	DWORD	clust;			/* Current cluster */
// 	LBA_t	sect;			/* Current sector (0:Read operation has terminated) */
// 	BYTE*	dir;			/* Pointer to the directory item in the win[] */
// 	BYTE	fn[12];			/* SFN (in/out) {body[8],ext[3],status[1]} */
// 	DWORD	blk_ofs;		/* Offset of current entry block being processed (0xFFFFFFFF:Invalid) */
// #if FF_USE_FIND
// 	const TCHAR* pat;		/* Pointer to the name matching pattern */
// #endif
// } DIR;

// #define FF_SFN_BUF 12
// #define FF_LFN_BUF 255

// typedef struct {
// 	FSIZE_t fsize;				   /* File size */
// 	WORD fdate;					   /* Modified date */
// 	WORD ftime;					   /* Modified time */
// 	BYTE fattrib;				   /* File attribute */
// 	TCHAR altname[FF_SFN_BUF + 1]; /* Altenative file name */
// 	TCHAR fname[FF_LFN_BUF + 1];   /* Primary file name */
// } FILINFO;
