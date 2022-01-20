#include "file.hpp"
#include "update.hpp"

/**
 * @brief Read a file
 *
 * @param filepath Path to the file to read
 *
 * @return Data* data ptr object holding the file content
 */
std::vector<uint8_t> update::read(const char* filepath)
{
    File file(filepath);

    if (!file.Open(File::MODE_READ))
    {
        file.Close();

        return std::vector<uint8_t>(0);
    }

    return file.Read();
}
