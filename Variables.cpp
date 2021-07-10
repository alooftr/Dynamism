#include "Variables.h"

#include <fstream>
#include <ShlObj.h>
#include "archivex.h"
#include <filesystem>

item_setting* get_by_definition_index(const int definition_index)
{
	return &config_system.g_cfg.skins.skinChanger[definition_index];
}

void c_config::run(const char* name) noexcept {
	PWSTR pathToDocuments;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pathToDocuments))) {
		path = pathToDocuments;
		path /= "Dynamism.space/Configs";
		CoTaskMemFree(pathToDocuments);
	}

	if (!std::filesystem::is_directory(path)) {
		std::filesystem::remove(path);
		std::filesystem::create_directory(path);
	}

	std::transform(std::filesystem::directory_iterator{ path },
		std::filesystem::directory_iterator{ },
		std::back_inserter(configs),
		[](const auto& entry) { return entry.path().filename().string(); });
}

void c_config::refresh() noexcept
{
	PWSTR pathToDocuments;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pathToDocuments))) {
		path = pathToDocuments;
		path /= "Dynamism.space/Configs";
		CoTaskMemFree(pathToDocuments);

		for (auto& entry : std::filesystem::directory_iterator(path))
		{
			auto path = entry.path();
			auto filename = path.filename().string();

			configs.emplace_back(filename);

		}
	}
}

#include <streambuf>

#include <iterator>
#include "cheats/menu.h"
void c_config::load(size_t id) noexcept {
	//g_cfg = { };
	if (!std::filesystem::is_directory(path))
		std::filesystem::create_directory(path);

	std::ifstream in{ path / configs[id] };

	if (!in.good())
		return;

	ArchiveX<std::ifstream>{ in } >> g_cfg;

	in.close();
}

void c_config::save(size_t id) const noexcept {
	if (!std::filesystem::is_directory(path)) 
		std::filesystem::create_directory(path);
	

	std::ofstream out{ path / configs[id] };

	if (!out.good())
		return;

	ArchiveX<std::ofstream>{ out } << g_cfg;

	out.close();
}

void c_config::add(const char* name) noexcept {
	if (*name && std::find(std::cbegin(configs), std::cend(configs), name) == std::cend(configs))
		configs.emplace_back(name);
}

void c_config::remove(size_t id) noexcept {
	std::filesystem::remove(path / configs[id]);
	configs.erase(configs.cbegin() + id);
}

void c_config::rename(size_t item, const char* newName) noexcept {
	std::filesystem::rename(path / configs[item], path / newName);
	configs[item] = newName;
}

void c_config::reset() noexcept {
	g_cfg = { };
}


void c_config::run2(const char* name) noexcept {
	PWSTR pathToDocuments;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pathToDocuments))) {
		path2 = pathToDocuments;
		path2 /= "Dynamism.space/MovementRecorder";
		CoTaskMemFree(pathToDocuments);
	}

	if (!std::filesystem::is_directory(path2))
	std::filesystem::create_directory(path2);
	

	std::transform(std::filesystem::directory_iterator{ path2 },
		std::filesystem::directory_iterator{ },
		std::back_inserter(configs2),
		[](const auto& entry) { return entry.path().filename().string(); });
}

void c_config::load2(size_t id) noexcept {
	if (!std::filesystem::is_directory(path2))
		std::filesystem::create_directory(path2);

	std::ifstream in{ path2 / configs2[id] };

	if (!in.good())
		return;

	ArchiveX<std::ifstream>{ in } >> GetMrecorder;
	in.close();
}

void c_config::save2(size_t id) const noexcept {
	if (!std::filesystem::is_directory(path2))
		std::filesystem::create_directory(path2);

	std::ofstream out{ path2 / configs2[id] };

	if (!out.good())
		return;

	ArchiveX<std::ofstream>{ out } << GetMrecorder;
	out.close();
}

void c_config::add2(const char* name) noexcept {
	if (*name && std::find(std::cbegin(configs2), std::cend(configs2), name) == std::cend(configs2))
		configs2.emplace_back(name);
}

void c_config::remove2(size_t id) noexcept {
	std::filesystem::remove(path2 / configs2[id]);
	configs2.erase(configs2.cbegin() + id);
}

void c_config::rename2(size_t item, const char* newName) noexcept {
	std::filesystem::rename(path2 / configs2[item], path2 / newName);
	configs2[item] = newName;
}

void c_config::reset2() noexcept {
	GetMrecorder = { };
}