#include "update.hpp"
#include "file.hpp"

#include "assets.hpp"
#include "nro.hpp"

/**
 * @brief Update the SMDH and RomFS sections
 *
 * @param info Arguments from main
 * @return int success value
 */
int update::init(const args::Info& args)
{
    File executable(args.filepath);

    /* open the nro for reading */
    if (!executable.Open(File::MODE_READ))
        return -1;

    /* read the start struct */
    NRO::Start start {};
    executable.Read(&start, NRO::START_SIZE);

    /* read the header struct */
    NRO::Header header {};
    executable.Read(&header, NRO::HEADER_SIZE);

    if (strncmp(header.magic, NRO::MAGIC, 4) != 0)
        error("Invalid NRO header.");

    /* get the assets data */
    Assets::Header assetsHeader {};
    executable.Read(&assetsHeader, Assets::HEADER_SIZE);

    if (strncmp(assetsHeader.magic, Assets::MAGIC, 4) != 0)
        error("Invalid Asset header.");

    /* get the icon data */
    size_t offset                 = Assets::HEADER_SIZE;
    std::vector<uint8_t> iconData = update::read(args.iconPath);

    if (!iconData.empty())
    {
        assetsHeader.icon.offset = offset;
        assetsHeader.icon.size   = iconData.size();

        offset += iconData.size();
    }
    else
    {
        if (assetsHeader.icon.size != 0)
        {
            executable.Seek(assetsHeader.icon.offset);
            executable.Read(iconData.data(), assetsHeader.icon.size);
        }
    }

    /* get the NACP data */
    std::vector<uint8_t> nacpData = update::read(args.nacpPath);

    if (!nacpData.empty())
    {
        assetsHeader.nacp.offset = offset;
        assetsHeader.nacp.size   = nacpData.size();

        offset += nacpData.size();
    }
    else
    {
        if (assetsHeader.nacp.size != 0)
        {
            executable.Seek(assetsHeader.nacp.offset);
            executable.Read(nacpData.data(), assetsHeader.nacp.size);
        }
    }

    /* get the RomFS data */
    std::vector<uint8_t> romfsData = update::read(args.nacpPath);

    if (!romfsData.empty())
    {
        assetsHeader.romfs.offset = offset;
        assetsHeader.romfs.size   = romfsData.size();

        offset += romfsData.size();
    }
    else
    {
        if (assetsHeader.romfs.size != 0)
        {
            executable.Seek(assetsHeader.romfs.offset);
            executable.Read(romfsData.data(), assetsHeader.romfs.size);
        }
    }

    File updated(args.outPath);

    if (!updated.Open(File::MODE_WRITE))
        errorf("Failed to open %s for writing.", updated.GetFilename().c_str());

    /* write our start */
    updated.Write(&start, NRO::START_SIZE);

    /* write our header */
    updated.Write(&header, NRO::HEADER_SIZE);

    /* write our assets header */
    updated.Write(&assetsHeader, Assets::HEADER_SIZE);

    if (iconData.size() != 0)
        updated.Write(iconData.data(), iconData.size());

    if (nacpData.size() != 0)
        updated.Write(nacpData.data(), nacpData.size());

    if (romfsData.size() != 0)
        updated.Write(romfsData.data(), romfsData.size());

    if (!updated.Close())
        errorf("Failed to close file %s.", updated.GetFilename().c_str());

    return 0;
}
