#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/stat.h>

#include <vector>

class File
{
  public:
    enum Mode
    {
        MODE_CLOSED,
        MODE_READ,
        MODE_WRITE,
        MODE_APPEND,
        MODE_MAX_ENUM
    };

    static constexpr int64_t ALL = -1;

    File(const std::string& name);

    ~File();

    bool Open(File::Mode mode);

    bool Close();

    int64_t Tell();

    int64_t Read(void* destination, int64_t size = ALL);

    std::vector<uint8_t> Read(int64_t size = ALL);

    bool Write(const void* buffer, int64_t size);

    bool Flush();

    bool IsOpen()
    {
        return this->mode != MODE_CLOSED && this->file != nullptr;
    }

    const std::string GetFilename() const
    {
        return this->filename;
    }

    const char* GetMode(File::Mode mode)
    {
        switch (mode)
        {
            default:
            case Mode::MODE_CLOSED:
                return "c";
            case Mode::MODE_READ:
                return "rb";
            case Mode::MODE_WRITE:
                return "wb";
        }
    }

    uint64_t GetSize();

  private:
    FILE* file;

    std::string filename;
    Mode mode;
};
