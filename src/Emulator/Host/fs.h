#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <optional>
#include <vector>
#include <types.h>

#include "Lib/Threads.h"

namespace Emulator::Host::Fs {
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
}