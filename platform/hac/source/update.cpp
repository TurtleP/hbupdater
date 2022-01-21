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

    /* get the executable data */
    std::vector<uint8_t> nroData(header.totalSize);
    executable.Seek(0);
    executable.Read(nroData.data(), nroData.size());

    /* get the assets data */
    Assets::Header assetsHeader {};
    executable.Read(&assetsHeader, Assets::HEADER_SIZE);

    if (strncmp(assetsHeader.magic, Assets::MAGIC, 4) != 0)
        error("Invalid Asset header.");

    /* get the icon data */
    uint64_t assetsOffset         = Assets::HEADER_SIZE;
    std::vector<uint8_t> iconData = update::read(args.iconPath);

    if (!iconData.empty())
    {
        assetsHeader.icon.offset = assetsOffset;
        assetsHeader.icon.size   = iconData.size();
    }
    else
    {
        if (assetsHeader.icon.size != 0)
        {
            iconData.resize(assetsHeader.icon.size);

            executable.Seek(header.totalSize + assetsHeader.icon.offset);
            executable.Read(iconData.data(), iconData.size());
        }
    }
    assetsOffset += iconData.size();

    /* get the NACP data */
    std::vector<uint8_t> nacpData = update::read(args.nacpPath);

    if (!nacpData.empty())
    {
        assetsHeader.nacp.offset = assetsOffset;
        assetsHeader.nacp.size   = nacpData.size();
    }
    else
    {
        if (assetsHeader.nacp.size != 0)
        {
            nacpData.resize(assetsHeader.nacp.size);

            /* capture original NACP offset */
            uint64_t nacpOffset = assetsHeader.nacp.offset;

            if (assetsOffset != nacpOffset)
                assetsHeader.nacp.offset = assetsOffset;

            /* read the original data */
            executable.Seek(header.totalSize + nacpOffset);
            executable.Read(nacpData.data(), nacpData.size());
        }
    }
    assetsOffset += nacpData.size();

    /* get the RomFS data */
    std::vector<uint8_t> romfsData = update::read(args.romfsPath);

    if (!romfsData.empty())
    {
        assetsHeader.romfs.offset = assetsOffset;
        assetsHeader.romfs.size   = romfsData.size();
    }
    else
    {
        if (assetsHeader.romfs.size != 0)
        {
            romfsData.resize(assetsHeader.romfs.size);

            /* capture original NACP offset */
            uint64_t romfsOffset = assetsHeader.romfs.offset;

            if (assetsOffset != romfsOffset)
                assetsHeader.romfs.offset = assetsOffset;

            /* read the original data */
            executable.Seek(header.totalSize + romfsOffset);
            executable.Read(romfsData.data(), romfsData.size());
        }
    }

    File updated(args.outPath);

    if (!updated.Open(File::MODE_WRITE))
        errorf("Failed to open %s for writing.", updated.GetFilename().c_str());

    /* write the executable data */
    updated.Write(nroData.data(), nroData.size());

    /* write our assets header */
    updated.Write(&assetsHeader, Assets::HEADER_SIZE);

    if (iconData.size() != 0)
    {
        updated.Seek(header.totalSize + assetsHeader.icon.offset);
        updated.Write(iconData.data(), iconData.size());
    }

    if (nacpData.size() != 0)
    {
        updated.Seek(header.totalSize + assetsHeader.nacp.offset);
        updated.Write(nacpData.data(), nacpData.size());
    }

    if (romfsData.size() != 0)
    {
        updated.Seek(header.totalSize + assetsHeader.romfs.offset);
        updated.Write(romfsData.data(), romfsData.size());
    }

    if (!updated.Close())
        errorf("Failed to close file %s.", updated.GetFilename().c_str());

    return 0;
}
