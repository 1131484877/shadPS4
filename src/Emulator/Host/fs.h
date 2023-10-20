#pragma once
#include <types.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <optional>
#include <vector>

#include "Lib/Threads.h"

namespace Emulator::Host::Fs {

class MntPoints {
  public:
    struct MntPair {
        std::string host_path;
        std::string guest_path;  // e.g /app0/
    };

    MntPoints() {}
    virtual ~MntPoints() {}
    void mount(const std::string& host_folder, const std::string& guest_folder);
    void unMount(const std::string& path);
    void unMountAll();

  private:
    std::vector<MntPair> m_mnt_pairs;
    Lib::Mutex m_mutex;

};
struct File {
    bool valid = false;          // �� descriptor ����� ����� �������;
    FILE* file;                  // File handle ��� ������
    std::filesystem::path path;  // Path ��� ��� host FS

    File() : valid(true) {}
};

class HandleTable {
    std::vector<File> files;
    u32 openFileCount = 0;  // ���� descriptors ������ �������;

  public:
    static constexpr u32 MAX_FILE_DESCRIPTORS = 65536;

    HandleTable() {
        files.reserve(128);  // ������� reserve ���� ��� ������ ������ ��� �� ���������� allocations
        reset();
    }

    void reset() {
        for (auto& f : files) {
            if (f.valid) {
                // ������� fclose �� ������ ��� �,�� ���� ����� �� �����
            }
        }

        files.clear();
        openFileCount = 0;
    }

    // ���������� handle ��� �� �������������� ��� ������
    std::optional<u32> createHandle() {
        if (openFileCount >= MAX_FILE_DESCRIPTORS) {
            // ��� �������� �� ��������� ���� descriptors, ������ ������� �� max
            return std::nullopt;
        }

        // �������� ������� �� �������������� handle, ����� ��������� �� handle count
        openFileCount += 1;

        // ������� �� ������ ���� files ��� vector ����� �������� ��� �� �� ������� reuse
        for (size_t i = 0; i < files.size(); i++) {
            // ������� �������� ������, ������������ �� index ���
            if (!files[i].valid) {
                return i;
            }
        }

        // ��� ������� �������� ��� vector ���� ��� ������ ���������� �� cap, ����� ����������� �� vector
        // ��� ������������ �� index ��� �����������
        u32 handle = files.size();
        files.push_back(File());

        return handle;
    }

    void freeHandle(u32 handle) {
        if (handle >= files.size()) {
            // ������� invalid
            printf("POUTSA\n");
            return;
        }

        if (files[handle].valid) {
            // ���� ������� fclose ��� ����� ��� �� ���������� ��� reuse
            files[handle].valid = false;
            // ����� ��� ������� ������
            openFileCount -= 1;
        }
    }
};
}  // namespace Emulator::Host::Fs