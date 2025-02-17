#include <iostream>
#include <fstream>
#include <filesystem>

#include <shobjidl.h>


//	Logs a text encoded as UTF-8 to the console.
static void Log(const std::string& text)
{
	std::cout << text << std::endl;
}

//	Logs a text encoded as UTF-8 to the console. Text will be formatted in the same way as with std::format.
template <class... _Types>
_NODISCARD static void Log(const std::string_view formatText, _Types&&... _Args)
{
	std::string text = std::vformat(formatText, std::make_format_args(_Args...));

	Log(text);
}

//	Logs a wide character text encoded as UTF-8 to the console.
static void Log(const std::wstring& text)
{
	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, NULL, 0, NULL, NULL);
	char* buffer = new char[bufferSize];
	WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, buffer, bufferSize, NULL, NULL);
	Log(buffer);
	delete[] buffer;
}

//	Logs a wide character text encoded as UTF-8 to the console. Text will be formatted in the same way as with std::format.
template <class... _Types>
_NODISCARD static void Log(const std::wstring_view formatText, _Types&&... _Args)
{
	std::wstring text = std::vformat(formatText, std::make_wformat_args(_Args...));

	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, NULL, 0, NULL, NULL);
	char* buffer = new char[bufferSize];
	WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, buffer, bufferSize, NULL, NULL);
	Log(buffer);
	delete[] buffer;
}

bool CreateDirectoryRecursive(const std::filesystem::path& dir)
{
	if (!std::filesystem::create_directories(dir))
	{
		if (std::filesystem::exists(dir))
		{
			// The folder already exists:
			return true;
		}
		return false;
	}
	return true;
}

void GetExecutableDirectory(std::filesystem::path& outDir)
{
	wchar_t dir[MAX_PATH];
	GetModuleFileName(NULL, dir, MAX_PATH);
	outDir = dir;
	outDir = outDir.parent_path();
}

void LogWrongInput()
{
	Log(L"Please supply a .ng2 archive to extract it, or supply the extracted folder of a .ng2 archive to repack it.");
}


enum NG2ARCID
{
	ARC_cmn,
	ARC_char,
	ARC_s_chap_01_duplicate,
	ARC_rtm,
	ARC_other,
	ARC_other_rtm,
	ARC_s_chap_01,
	ARC_s_chap_02,
	ARC_s_chap_03,
	ARC_s_chap_04,
	ARC_s_chap_05,
	ARC_s_chap_06,
	ARC_s_chap_07,
	ARC_s_chap_08,
	ARC_s_chap_09,
	ARC_s_chap_10,
	ARC_s_chap_11,
	ARC_s_chap_12,
	ARC_s_chap_13,
	ARC_s_chap_14
};

NG2ARCID GetArchiveId(const char* arcName)
{
	if		(strcmp(arcName, "cmn") == 0)		return NG2ARCID::ARC_cmn;
	else if (strcmp(arcName, "char") == 0)		return NG2ARCID::ARC_char;
	else if (strcmp(arcName, "rtm") == 0)		return NG2ARCID::ARC_rtm;
	else if (strcmp(arcName, "other") == 0)		return NG2ARCID::ARC_other;
	else if (strcmp(arcName, "other_rtm") == 0)	return NG2ARCID::ARC_other_rtm;
	else if (strcmp(arcName, "s_chap_01") == 0)	return NG2ARCID::ARC_s_chap_01;
	else if (strcmp(arcName, "s_chap_02") == 0)	return NG2ARCID::ARC_s_chap_02;
	else if (strcmp(arcName, "s_chap_03") == 0)	return NG2ARCID::ARC_s_chap_03;
	else if (strcmp(arcName, "s_chap_04") == 0)	return NG2ARCID::ARC_s_chap_04;
	else if (strcmp(arcName, "s_chap_05") == 0)	return NG2ARCID::ARC_s_chap_05;
	else if (strcmp(arcName, "s_chap_06") == 0)	return NG2ARCID::ARC_s_chap_06;
	else if (strcmp(arcName, "s_chap_07") == 0)	return NG2ARCID::ARC_s_chap_07;
	else if (strcmp(arcName, "s_chap_08") == 0)	return NG2ARCID::ARC_s_chap_08;
	else if (strcmp(arcName, "s_chap_09") == 0)	return NG2ARCID::ARC_s_chap_09;
	else if (strcmp(arcName, "s_chap_10") == 0)	return NG2ARCID::ARC_s_chap_10;
	else if (strcmp(arcName, "s_chap_11") == 0)	return NG2ARCID::ARC_s_chap_11;
	else if (strcmp(arcName, "s_chap_12") == 0)	return NG2ARCID::ARC_s_chap_12;
	else if (strcmp(arcName, "s_chap_13") == 0)	return NG2ARCID::ARC_s_chap_13;
	else if (strcmp(arcName, "s_chap_14") == 0)	return NG2ARCID::ARC_s_chap_14;
}


int RepackArchive(std::filesystem::path& inputDir)
{
	std::filesystem::path exeDir;
	GetExecutableDirectory(exeDir);


	std::ifstream packImgBinStream(L"./pack_img.bin", std::ios::binary);
	if (!packImgBinStream)
	{
		packImgBinStream = std::ifstream(exeDir / L"pack_img.bin", std::ios::binary);
		if (!packImgBinStream)
		{
			Log(L"File \"pack_img.bin\" not found. Please keep this file right next to NG2ArchiveTool or the .ng2 archive.\nThis file has to be parsed when extracting an archive, and this file has to be modified when repacking an archive.");
			return 0;
		}
	}

	std::ifstream xexStream(L"./xex_decrypted.bin", std::ios::binary);
	if (!xexStream)
	{
		xexStream = std::ifstream(exeDir / L"xex_decrypted.bin", std::ios::binary);
		if (!xexStream)
		{
			Log(L"File \"xex_decrypted.bin\" not found. Please place this file right next to NG2ArchiveTool or the .ng2 archive.\nYou can get this file by extracting \"default.xex\" using Noesis.");
			return 0;
		}
	}


	//  Parsing pack_img.bin
	packImgBinStream.seekg(0x0, std::ios::end);
	size_t packImgBinSize = packImgBinStream.tellg();

	char* packImgBuf = new char[packImgBinSize];

	packImgBinStream.seekg(0x0, std::ios::beg);
	packImgBinStream.read(packImgBuf, packImgBinSize);
	packImgBinStream.close();

	uint32_t fileNameIdsArrayOffsetArray = _byteswap_ulong(*(unsigned long*)(packImgBuf + 0x0));
	uint32_t fileCountOffsetArray = _byteswap_ulong(*(unsigned long*)(packImgBuf + 0x4));
	uint32_t fileOffsetsArrayOffsetArray = _byteswap_ulong(*(unsigned long*)(packImgBuf + 0x8));
	uint32_t fileSizesArrayOffsetArray = _byteswap_ulong(*(unsigned long*)(packImgBuf + 0xC));

	uint32_t maxFileCount = 0;
	uint32_t fileCounts[20] = {};
	for (int i = 0; i < 20; i++)
	{
		uint32_t fileCountOffset = _byteswap_ulong(*(uint32_t*)(packImgBuf + fileCountOffsetArray + i * 0x4));
		fileCounts[i] = _byteswap_ulong(*(uint32_t*)(packImgBuf + fileCountOffset));
		maxFileCount += fileCounts[i];
	}


	//  Parsing XEX header
	xexStream.seekg(0x3C, std::ios::beg);
	uint32_t e_lfanew = 0;
	xexStream.read((char*)&e_lfanew, sizeof(e_lfanew));

	xexStream.seekg((size_t)e_lfanew + 0x6, std::ios::beg);
	uint16_t NumberOfSections = 0;
	xexStream.read((char*)&NumberOfSections, sizeof(NumberOfSections));

	uint32_t rdataAddress = 0;
	uint32_t rdataSize = 0;
	for (uint16_t i = 0; i < NumberOfSections; i++)
	{
		xexStream.seekg((size_t)e_lfanew + 0x78 + (size_t)NumberOfSections * 0x8 + (size_t)i * 0x28, std::ios::beg);
		char Name[8] = {};
		xexStream.read(Name, sizeof(Name));
		if (strncmp(Name, ".rdata", 7) == 0)
		{
			xexStream.seekg(0x4, std::ios::cur);

			xexStream.read((char*)&rdataAddress, sizeof(rdataAddress));

			xexStream.read((char*)&rdataSize, sizeof(rdataSize));

			break;
		}
	}

	if (rdataAddress && rdataSize)
	{
		char* rdataBuf = new char[rdataSize];
		char** fileNames = new char* [maxFileCount];

		xexStream.seekg(rdataAddress, std::ios::beg);
		xexStream.read(rdataBuf, rdataSize);
		xexStream.close();

		const char* searchStr = "ANG_ts_box.bin";
		size_t searchStrLen = strlen(searchStr);

		intptr_t off = (((size_t)rdataSize - searchStrLen) / 0x4) * 0x4;
		for (; off >= 0x0; off -= 0x4)
		{
			if (strncmp(rdataBuf + off, searchStr, searchStrLen) == 0)
			{
				//  Found first string, "ANG_ts_box.bin"
				break;
			}
		}

		for (uint32_t i = 0; i < maxFileCount && off >= 0; i++)
		{
			fileNames[i] = rdataBuf + off;

			while (off > 0x4)
			{
				off -= 0x4;
				if (rdataBuf[off] != '\0' && rdataBuf[off - 1] == '\0')
				{
					//  Found next string
					break;
				}
			}
		}


		std::string archiveName = inputDir.filename().string();
		if (archiveName.ends_with("_files"))
		{
			archiveName = archiveName.substr(0, archiveName.size() - 6);
		}
		NG2ARCID arcId = GetArchiveId(archiveName.c_str());

		uint32_t fileNameIdsArrayOffset = _byteswap_ulong(*(uint32_t*)(packImgBuf + fileNameIdsArrayOffsetArray + arcId * 0x4));
		uint32_t* fileNameIdsArray = (uint32_t*)(packImgBuf + _byteswap_ulong(*(uint32_t*)(packImgBuf + fileNameIdsArrayOffset)));

		uint32_t fileOffsetsArrayOffset = _byteswap_ulong(*(uint32_t*)(packImgBuf + fileOffsetsArrayOffsetArray + arcId * 0x4));
		uint32_t* fileOffsetsArray = (uint32_t*)(packImgBuf + _byteswap_ulong(*(uint32_t*)(packImgBuf + fileOffsetsArrayOffset)));

		uint32_t fileSizesArrayOffset = _byteswap_ulong(*(uint32_t*)(packImgBuf + fileSizesArrayOffsetArray + arcId * 0x4));
		uint32_t* fileSizesArray = (uint32_t*)(packImgBuf + _byteswap_ulong(*(uint32_t*)(packImgBuf + fileSizesArrayOffset)));


		std::filesystem::path inputPath = inputDir.parent_path() / archiveName;

		uint32_t arcPos = 0;
		std::ofstream outputStream(inputPath.wstring() + L"-out.ng2", std::ios::binary);
		if (outputStream)
		{
			outputStream.seekp(0x0, std::ios::beg);
			for (uint32_t i = 0; i < fileCounts[arcId]; i++)
			{
				const char* fileName = fileNames[_byteswap_ulong(fileNameIdsArray[i])];

				std::filesystem::path filePath = inputPath / fileName;

				std::ifstream inputStream(filePath, std::ios::binary);
				if (inputStream)
				{
					fileOffsetsArray[i] = _byteswap_ulong(arcPos);

					inputStream.seekg(0x0, std::ios::end);
					size_t fileSize = inputStream.tellg();
					fileSizesArray[i] = _byteswap_ulong(fileSize);

					char* fileBuf = new char[fileSize];

					inputStream.seekg(0x0, std::ios::beg);
					inputStream.read(fileBuf, fileSize);

					outputStream.write(fileBuf, fileSize);

					size_t padding = (((fileSize == 0x0 ? 0x0 : (fileSize - 1)) / 0x8000) + 1) * 0x8000;

					arcPos += padding;
					outputStream.seekp(arcPos, std::ios::beg);

					delete[] fileBuf;
				}
			}

			//	Write a zero to last byte, so that ofstream realizes I want zero-padding at the end of the file thank you very much.
			char zero = 0;
			outputStream.seekp(-1, std::ios::cur);
			outputStream.write(&zero, sizeof(zero));
		}

		delete[] fileNames;
		delete[] rdataBuf;


		std::ofstream packImgBinOutStream(inputDir.parent_path() / L"pack_img-out.bin", std::ios::binary);
		if (packImgBinOutStream)
		{
			packImgBinOutStream.write(packImgBuf, packImgBinSize);
		}
	}

	delete[] packImgBuf;

	return 0;
}


int ExtractArchive(std::filesystem::path& inputArc)
{
	std::filesystem::path exeDir;
	GetExecutableDirectory(exeDir);
	

	std::ifstream packImgBinStream(L"./pack_img.bin", std::ios::binary);
	if (!packImgBinStream)
	{
		packImgBinStream = std::ifstream(exeDir / L"pack_img.bin", std::ios::binary);
		if (!packImgBinStream)
		{
			Log(L"File \"pack_img.bin\" not found. Please keep this file right next to NG2ArchiveTool or the .ng2 archive.\nThis file has to be parsed when extracting an archive, and this file has to be modified when repacking an archive.");
			return 0;
		}
	}

	std::ifstream xexStream(L"./xex_decrypted.bin", std::ios::binary);
	if (!xexStream)
	{
		xexStream = std::ifstream(exeDir / L"xex_decrypted.bin", std::ios::binary);
		if (!xexStream)
		{
			Log(L"File \"xex_decrypted.bin\" not found. Please place this file right next to NG2ArchiveTool or the .ng2 archive.\nYou can get this file by extracting \"default.xex\" using Noesis.");
			return 0;
		}
	}

	std::ifstream arcStream(inputArc, std::ios::binary);
	if (!arcStream)
	{
		Log(L"Archive \"{}\" could not be opened.", inputArc.c_str());
		return 0;
	}


	//  Parsing pack_img.bin
	packImgBinStream.seekg(0x0, std::ios::end);
	size_t packImgBinSize = packImgBinStream.tellg();

	char* packImgBuf = new char[packImgBinSize];

	packImgBinStream.seekg(0x0, std::ios::beg);
	packImgBinStream.read(packImgBuf, packImgBinSize);
	packImgBinStream.close();

	uint32_t fileNameIdsArrayOffsetArray = _byteswap_ulong(*(unsigned long*)(packImgBuf + 0x0));
	uint32_t fileCountOffsetArray = _byteswap_ulong(*(unsigned long*)(packImgBuf + 0x4));
	uint32_t fileOffsetsArrayOffsetArray = _byteswap_ulong(*(unsigned long*)(packImgBuf + 0x8));
	uint32_t fileSizesArrayOffsetArray = _byteswap_ulong(*(unsigned long*)(packImgBuf + 0xC));

	uint32_t maxFileCount = 0;
	uint32_t fileCounts[20] = {};
	for (int i = 0; i < 20; i++)
	{
		uint32_t fileCountOffset = _byteswap_ulong(*(uint32_t*)(packImgBuf + fileCountOffsetArray + i * 0x4));
		fileCounts[i] = _byteswap_ulong(*(uint32_t*)(packImgBuf + fileCountOffset));
		maxFileCount += fileCounts[i];
	}


	//  Parsing XEX header
	xexStream.seekg(0x3C, std::ios::beg);
	uint32_t e_lfanew = 0;
	xexStream.read((char*)&e_lfanew, sizeof(e_lfanew));

	xexStream.seekg((size_t)e_lfanew + 0x6, std::ios::beg);
	uint16_t NumberOfSections = 0;
	xexStream.read((char*)&NumberOfSections, sizeof(NumberOfSections));
	
	uint32_t rdataAddress = 0;
	uint32_t rdataSize = 0;
	for (uint16_t i = 0; i < NumberOfSections; i++)
	{
		xexStream.seekg((size_t)e_lfanew + 0x78 + (size_t)NumberOfSections * 0x8 + (size_t)i * 0x28, std::ios::beg);
		char Name[8] = {};
		xexStream.read(Name, sizeof(Name));
		
		if (strncmp(Name, ".rdata", 7) == 0)
		{
			xexStream.seekg(0x4, std::ios::cur);

			xexStream.read((char*)&rdataAddress, sizeof(rdataAddress));

			xexStream.read((char*)&rdataSize, sizeof(rdataSize));

			break;
		}
	}
	
	if (rdataAddress && rdataSize)
	{
		char* rdataBuf = new char[rdataSize];
		char** fileNames = new char* [maxFileCount];

		xexStream.seekg(rdataAddress, std::ios::beg);
		xexStream.read(rdataBuf, rdataSize);
		xexStream.close();

		const char* searchStr = "ANG_ts_box.bin";
		size_t searchStrLen = strlen(searchStr);

		intptr_t off = (((size_t)rdataSize - searchStrLen) / 0x4) * 0x4;
		for (; off >= 0x0; off -= 0x4)
		{
			if (strncmp(rdataBuf + off, searchStr, searchStrLen) == 0)
			{
				//  Found first string, "ANG_ts_box.bin"
				break;
			}
		}

		for (uint32_t i = 0; i < maxFileCount && off >= 0; i++)
		{
			fileNames[i] = rdataBuf + off;

			while (off > 0x4)
			{
				off -= 0x4;
				if (rdataBuf[off] != '\0' && rdataBuf[off - 1] == '\0')
				{
					//  Found next string
					break;
				}
			}
		}


		std::string archiveName = inputArc.filename().string();
		archiveName = archiveName.substr(0, archiveName.size() - 4);
		NG2ARCID arcId = GetArchiveId(archiveName.c_str());

		uint32_t fileNameIdsArrayOffset = _byteswap_ulong(*(uint32_t*)(packImgBuf + fileNameIdsArrayOffsetArray + arcId * 0x4));
		uint32_t* fileNameIdsArray = (uint32_t*)(packImgBuf + _byteswap_ulong(*(uint32_t*)(packImgBuf + fileNameIdsArrayOffset)));
		
		uint32_t fileOffsetsArrayOffset = _byteswap_ulong(*(uint32_t*)(packImgBuf + fileOffsetsArrayOffsetArray + arcId * 0x4));
		uint32_t* fileOffsetsArray = (uint32_t*)(packImgBuf + _byteswap_ulong(*(uint32_t*)(packImgBuf + fileOffsetsArrayOffset)));
		
		uint32_t fileSizesArrayOffset = _byteswap_ulong(*(uint32_t*)(packImgBuf + fileSizesArrayOffsetArray + arcId * 0x4));
		uint32_t* fileSizesArray = (uint32_t*)(packImgBuf + _byteswap_ulong(*(uint32_t*)(packImgBuf + fileSizesArrayOffset)));


		std::filesystem::path outputPath = inputArc.parent_path() / archiveName;
		for (uint32_t i = 0; i < fileCounts[arcId]; i++)
		{
			const char* fileName = fileNames[_byteswap_ulong(fileNameIdsArray[i])];

			std::filesystem::path filePath = outputPath / fileName;
			CreateDirectoryRecursive(filePath.parent_path());

			std::ofstream outputStream(filePath, std::ios::binary);
			if (outputStream)
			{
				uint32_t fileOffset = _byteswap_ulong(fileOffsetsArray[i]);
				uint32_t fileSize = _byteswap_ulong(fileSizesArray[i]);

				char* fileBuf = new char[fileSize];

				arcStream.seekg(fileOffset, std::ios::beg);
				arcStream.read(fileBuf, fileSize);

				outputStream.write(fileBuf, fileSize);

				delete[] fileBuf;
			}
		}


		delete[] fileNames;
		delete[] rdataBuf;
	}

	delete[] packImgBuf;

	return 0;
}


int wmain(int argc, wchar_t* argv[])
{
	// Set console code page to UTF-8
	SetConsoleOutputCP(CP_UTF8);

	if (argc == 1)
	{
		LogWrongInput();
		Sleep(10000);
		return 0;
	}

	for (int i = 1; i < argc; i++)
	{
		std::filesystem::path input(argv[i]);
		if (std::filesystem::is_directory(input))
		{
			//  Repacking directory into .ng2 archive
			if (RepackArchive(input))
				break;
		}
		else
		{
			if (input.has_extension() && strcmp(input.extension().string().c_str(), ".ng2") == 0)
			{
				//  Extracting .ng2 archive
				if (ExtractArchive(input))
					break;
			}
		}
		
		if (i + 1 == argc)
		{
			Log("Finished all operations successfully.");
			return 0;
		}
	}

	LogWrongInput();
	return 0;
}