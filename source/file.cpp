#include "file.hpp"

#include <algorithm>

File::File(const std::string& name) : file(nullptr), filename(name), mode(Mode::MODE_CLOSED)
{}

File::~File()
{
    if (this->file)
        this->Close();
}

bool File::Open(File::Mode mode)
{
    if (this->GetFilename().empty())
        return false;

    const char* fmode = this->GetMode(mode);

    this->file = fopen(this->GetFilename().c_str(), fmode);

    if (!this->file)
    {
        printf("%s", strerror(errno));
        this->Close();
    }

    this->mode = mode;
    return this->file != nullptr;
}

bool File::Close()
{
    if (!this->file)
        return false;

    fclose(this->file);

    this->mode = Mode::MODE_CLOSED;
    this->file = nullptr;

    return true;
}

uint64_t File::GetSize()
{
    if (this->file == nullptr)
    {
        this->Open(Mode::MODE_READ);
        fseek(this->file, 0, SEEK_END);
        uint64_t size = ftell(this->file);
        this->Close();

        return size;
    }

    int64_t position = this->Tell();

    fseek(this->file, 0, SEEK_END);
    uint64_t size = ftell(this->file);
    fseek(this->file, position, SEEK_SET);

    return size;
}

bool File::Seek(uint64_t position)
{
    return this->file != nullptr && (fseek(this->file, position, SEEK_SET) == 0);
}

int64_t File::Tell()
{
    if (!this->file)
        return -1;

    return ftell(this->file);
}

int64_t File::Read(void* destination, int64_t size)
{
    if (!this->file || this->mode != Mode::MODE_READ)
    {
        printf("File is not opened for reading.\n");
        return 0;
    }

    long selfSize = this->GetSize();

    size = (size == ALL) ? selfSize : size;
    size = (size > selfSize) ? selfSize : size;

    if (size < 0)
    {
        printf("Invalid read size %ld\n", size);
        return 0;
    }

    return fread(destination, 1, size, this->file);
}

std::vector<uint8_t> File::Read(int64_t size)
{
    if (!this->IsOpen() && this->Open(Mode::MODE_READ))
    {
        printf("Could not read file %s.", this->GetFilename().c_str());
        return std::vector<uint8_t>(0);
    }

    int64_t max     = this->GetSize();
    int64_t current = this->Tell();

    size = (size == ALL) ? max : size;

    if (size < 0)
    {
        printf("Invalid read size %ld.", size);
        return std::vector<uint8_t>(0);
    }

    std::clamp(current, static_cast<int64_t>(0), max);

    if (current + size > max)
        size = max - current;

    std::vector<uint8_t> fileData(size);
    int64_t bytesRead = this->Read(fileData.data(), size);

    if (bytesRead < 0 || (bytesRead == 0 && bytesRead != size))
    {
        fileData.clear();
        printf("Could not read from file %s.", this->GetFilename().c_str());
        return std::vector<uint8_t>(0);
    }

    if (bytesRead < size)
    {
        std::vector<uint8_t> temp(bytesRead);
        memcpy(temp.data(), fileData.data(), static_cast<size_t>(bytesRead));

        fileData = temp;
    }

    if (!this->IsOpen())
        this->Close();

    return fileData;
}

bool File::Write(const void* data, int64_t size)
{
    if (!this->file || (this->mode != MODE_WRITE && this->mode != MODE_APPEND))
    {
        printf("File is not opened for writing.\n");
        return false;
    }

    if (size < 0)
    {
        printf("Invalid write size.\n");
        return false;
    }

    int64_t written = fwrite(data, 1, size, this->file);

    if (written != size)
        return false;

    return true;
}

bool File::Flush()
{
    if (!this->file || (this->mode != MODE_WRITE && this->mode != MODE_APPEND))
    {
        printf("File is not opened for writing.\n");
        return false;
    }

    return fflush(this->file) == 0;
}
