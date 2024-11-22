//======================================================================================================================
// Project: DoomRunner
//----------------------------------------------------------------------------------------------------------------------
// Author:      Jan Broz (Youda008)
// Description: properties and capabilities of different engines
//======================================================================================================================

#include "EngineTraits.hpp"

#include "Utils/ContainerUtils.hpp"   // find
#include "Utils/FileSystemUtils.hpp"  // getFileBasenameFromPath
#include "Utils/OSUtils.hpp"          // getCachedPicturesDir

#include <QHash>
#include <QRegularExpression>


//======================================================================================================================
//  engine definitions - add support for new engines here

static const char * const engineFamilyStrings [] =
{
	"ZDoom",
	"PrBoom",
	"MBF",
	"ChocolateDoom",
};
static_assert( std::size(engineFamilyStrings) == size_t(EngineFamily::_EnumEnd), "Please update this table too" );

static const QHash< QString, EngineFamily > knownEngineFamilies =
{
	// the key is an executable name in lower case without the .exe suffix
	{ "zdoom",            EngineFamily::ZDoom },
	{ "lzdoom",           EngineFamily::ZDoom },
	{ "gzdoom",           EngineFamily::ZDoom },
	{ "qzdoom",           EngineFamily::ZDoom },
	{ "skulltag",         EngineFamily::ZDoom },
	{ "zandronum",        EngineFamily::ZDoom },
	{ "prboom",           EngineFamily::PrBoom },
	{ "prboom-plus",      EngineFamily::PrBoom },
	{ "glboom",           EngineFamily::PrBoom },
	{ "dsda-doom",        EngineFamily::PrBoom },
	{ "smmu",             EngineFamily::MBF },
	{ "eternity",         EngineFamily::MBF },
	{ "woof",             EngineFamily::MBF },
	{ "chocolate-doom",   EngineFamily::ChocolateDoom },
	{ "crispy-doom",      EngineFamily::ChocolateDoom },
	{ "doomretro",        EngineFamily::ChocolateDoom },
	{ "strife-ve",        EngineFamily::ChocolateDoom },
	// TODO: add all the EDGE ports
};

static const EngineFamilyTraits engineFamilyTraits [] =
{
	//              -warp or +map        -complevel or +compatmode   savedir param   has +screenshot_dir   needs -stdout
	/*ZDoom*/     { MapParamStyle::Map,  CompatLevelStyle::ZDoom,    "-savedir",     true,                 IS_WINDOWS },
	/*PrBoom*/    { MapParamStyle::Warp, CompatLevelStyle::PrBoom,   "-save",        false,                false },
	/*MBF*/       { MapParamStyle::Warp, CompatLevelStyle::PrBoom,   "-save",        false,                false },
	/*Chocolate*/ { MapParamStyle::Warp, CompatLevelStyle::None,     "-savedir",     false,                false },
};
static_assert( std::size(engineFamilyTraits) == std::size(engineFamilyStrings), "Please update this table too" );

static const QHash< QString, int > startingMonitorIndexes =
{
	// the key is an executable name in lower case without the .exe suffix
	{ "zdoom", 1 },
};

static const QStringList zdoomCompatLevels =
{
	"0 - Default",        // All compatibility options are turned off.
	"1 - Doom",           // Enables a set of options that should allow nearly all maps made for vanilla Doom to work in ZDoom:
	                      //   crossdropoff, dehhealth, light, missileclip, nodoorlight, shorttex, soundtarget, spritesort, stairs, trace, useblocking, floormove, maskedmidtex
	"2 - Doom (Strict)",  // Sets all of the above options and also sets these:
	                      //   corpsegibs, hitscan, invisibility, limitpain, nopassover, notossdrop, wallrun
	"3 - Boom",           // Allows maps made specifically for Boom to function correctly by enabling the following options:
	                      //   boomscroll, missileclip, soundtarget, trace, maskedmidtex
	"4 - ZDoom 2.0.63",   // Sets the two following options to be true, restoring the behavior of version 2.0.63:
	                      //   light, soundtarget
	"5 - MBF",            // As Boom above, but also sets these for closer imitation of MBF behavior:
	                      //   mushroom, mbfmonstermove, noblockfriends, maskedmidtex
	"6 - Boom (Strict)",  // As Boom above, but also sets these:
	                      //   corpsegibs, hitscan, invisibility, nopassover, notossdrop, wallrun, maskedmidtex
    "7 - MBF (Strict)",
    "8 - MBF 21",
    "9 - MBF 21 (Strict)",
};

static const QStringList prboomCompatLevels =
{
	"0  - Doom v1.2",     // (note: flawed; use PrBoom+ 2.5.0.8 or higher instead if this complevel is desired)
	"1  - Doom v1.666",
	"2  - Doom v1.9",
	"3  - Ultimate Doom",
	"4  - Final Doom & Doom95",
	"5  - DOSDoom",
	"6  - TASDOOM",
	"7  - Boom's inaccurate vanilla",
	"8  - Boom v2.01",
	"9  - Boom v2.02",
	"10 - LxDoom",
	"11 - MBF",
	"12 - PrBoom (older version)",
	"13 - PrBoom (older version)",
	"14 - PrBoom (older version)",
	"15 - PrBoom (older version)",
	"16 - PrBoom (older version)",
	"17 - PrBoom (current)",
	"18 - unused",
	"19 - unused",
	"20 - unused",
	"21 - MBF21",
};

static const QStringList noCompatLevels = {};


//======================================================================================================================
//  code

//----------------------------------------------------------------------------------------------------------------------
//  compat levels

const QStringList & getCompatLevels( CompatLevelStyle style )
{
	if (style == CompatLevelStyle::ZDoom)
		return zdoomCompatLevels;
	else if (style == CompatLevelStyle::PrBoom)
		return prboomCompatLevels;
	else
		return noCompatLevels;
}

//----------------------------------------------------------------------------------------------------------------------
//  EngineFamily

const char * familyToStr( EngineFamily family )
{
	if (size_t(family) < std::size(engineFamilyStrings))
		return engineFamilyStrings[ size_t(family) ];
	else
		return "<invalid>";
}

EngineFamily familyFromStr( const QString & familyStr )
{
	int idx = find( engineFamilyStrings, familyStr );
	if (idx >= 0)
		return EngineFamily( idx );
	else
		return EngineFamily::_EnumEnd;
}

EngineFamily EngineTraits::guessEngineFamily() const
{
	assert( hasAppInfo() );

	auto iter = knownEngineFamilies.find( normalizedName() );
	if (iter != knownEngineFamilies.end())
		return iter.value();
	else
		return EngineFamily::ZDoom;
}


//======================================================================================================================
//  EngineTraits

//----------------------------------------------------------------------------------------------------------------------
//  initialization

void EngineTraits::assignFamilyTraits( EngineFamily family )
{
	if (size_t(family) < std::size(engineFamilyTraits))
		_familyTraits = &engineFamilyTraits[ size_t(family) ];
	else
		_familyTraits = &engineFamilyTraits[ 0 ];  // use ZDoom traits as fallback
}

//----------------------------------------------------------------------------------------------------------------------
//  default directories and path requirements

// The following default directories were observed when using GZDoom 4.9 and later.
// Why do you have to make everything so complicated Graph?!
/*
Windows - system-wide
  ini config:   E:\Youda\Documents\My Games\GZDoom\
  saves 4.10:   E:\Youda\Saved Games\GZDoom\doom.id.doom2.commercial\
  saves 4.11:   E:\Youda\Saved Games\GZDoom\savegames\doom.id.doom2.commercial\
  screenshots:  E:\Youda\Pictures\Screenshots\GZDoom\

Linux - system-wide
  ini config:   /home/youda/.config/gzdoom/
  saves 4.10:   /home/youda/.config/gzdoom/doom.id.doom2.commercial/
  saves 4.11:   /home/youda/.config/gzdoom/savegames/doom.id.doom2.commercial/
  screenshots:  /home/youda/.config/gzdoom/screenshots/

Linux - Flatpak
  sandbox home: /home/youda/.var/app/org.zdoom.GZDoom/
  ini config:   /home/youda/.var/app/org.zdoom.GZDoom/.config/gzdoom/
  saves 4.10:   /home/youda/.var/app/org.zdoom.GZDoom/.config/gzdoom/doom.id.doom2.commercial/
  saves 4.11:   /home/youda/.var/app/org.zdoom.GZDoom/.config/gzdoom/savegames/doom.id.doom2.commercial/
  screenshots:  /home/youda/.var/app/org.zdoom.GZDoom/.config/gzdoom/screenshots/

Linux - Snap
  sandbox home: /home/youda/snap/gzdoom/current/
  ini config:   /home/youda/snap/gzdoom/current/.config/gzdoom/
  saves 4.10:   /home/youda/snap/gzdoom/current/.config/gzdoom/doom.id.doom2.commercial/
  saves 4.11:   /home/youda/snap/gzdoom/current/.config/gzdoom/savegames/doom.id.doom2.commercial/
  screenshots:  /home/youda/snap/gzdoom/current/.config/gzdoom/screenshots/

Mac - system-wide
  ini config:   ~/Library/Preferences/gzdoom/
  saves 4.10:   ~/Library/Preferences/gzdoom/?
  saves 4.11:   ~/Library/Preferences/gzdoom/savegames?/
  screenshots:  ~/Library/Preferences/gzdoom/screenshots?/
*/

bool EngineTraits::isGZDoomVersionOrLater( Version atLeastVersion ) const
{
	// If we have version info from the executable file, decide based on the application name and version,
	// otherwise if the executable file name seems like GZDoom, assume the latest version.
	if (!exeAppName().isEmpty() && exeVersion().isValid())
		return exeAppName() == "GZDoom" && exeVersion() >= atLeastVersion;
	else
		return normalizedName() == "gzdoom";
}

QString EngineTraits::getDefaultConfigDir() const
{
	assert( hasAppInfo() );

 #if IS_WINDOWS

	// On Windows, engines usually store their config in the directory of its binaries,
	// with the exception of latest GZDoom (thanks Graph) that started storing it to Documents\My Games\GZDoom
	QString dirOfExecutable = fs::getParentDir( exePath() );
	QString portableIniFilePath = fs::getPathFromEntryName( dirOfExecutable, "gzdoom_portable.ini" );
	if (isGZDoomVersionOrLater({4,9,0}) && !fs::isValidFile( portableIniFilePath ))
		return os::getDocumentsDir()%"/My Games/GZDoom";
	else
		return dirOfExecutable;

 #else

	// On Linux they store them in standard user's app config dir (usually something like /home/youda/.config/).
	return os::getConfigDirForApp( exePath() );

 #endif
}

QString EngineTraits::getDefaultDataDir() const
{
	assert( hasAppInfo() );

 #if IS_WINDOWS

	// On Windows, engines usually store their data (saves, screenshots) in the directory of its binaries,
	// with the exception of latest GZDoom (thanks Graph) that started storing it to Saved Games\GZDoom
	QString dirOfExecutable = fs::getParentDir( exePath() );
	QString portableIniFilePath = fs::getPathFromEntryName( dirOfExecutable, "gzdoom_portable.ini" );
	if (isGZDoomVersionOrLater({4,9,0}) && !fs::isValidFile( portableIniFilePath ))
		return os::getSavedGamesDir()%"/GZDoom";
	else
		return dirOfExecutable;

 #else

	// On Linux they generally store them in the config dir.
	return getDefaultConfigDir();

 #endif
}

QString EngineTraits::getDefaultSaveDir() const
{
	assert( hasAppInfo() );

	QString saveDir = getDefaultDataDir();
	if (isGZDoomVersionOrLater({4,11,0}))  // since GZDoom 4.11.0, the save files are stored in a subdirectory
		saveDir += "/savegames";
	return saveDir;
}

QString EngineTraits::getDefaultScreenshotDir() const
{
	assert( hasAppInfo() );

 #if IS_WINDOWS
	QString dirOfExecutable = fs::getParentDir( exePath() );
	QString portableIniFilePath = fs::getPathFromEntryName( dirOfExecutable, "gzdoom_portable.ini" );
	if (isGZDoomVersionOrLater({4,9,0}) && !fs::isValidFile( portableIniFilePath ))
		return os::getPicturesDir()%"/Screenshots/GZDoom";
	else
		return dirOfExecutable;
 #else
	return getDefaultDataDir() + "/screenshots";
 #endif
}

EngineTraits::SaveBaseDir EngineTraits::baseDirStyleForSaveFiles() const
{
	assert( hasAppInfo() );

	if (isGZDoomVersionOrLater({4,9,0}))
		return SaveBaseDir::SaveDir;
	else
		return SaveBaseDir::WorkingDir;
}

//----------------------------------------------------------------------------------------------------------------------
//  command line parameters deduction

static const QRegularExpression doom1MapNameRegex("E(\\d+)M(\\d+)");
static const QRegularExpression doom2MapNameRegex("MAP(\\d+)");

QStringVec EngineTraits::getMapArgs( int mapIdx, const QString & mapName ) const
{
	assert( hasAppInfo() && hasFamilyTraits() );

	if (mapName.isEmpty())
	{
		return {};
	}

	if (_familyTraits->mapParamStyle == MapParamStyle::Map)  // this engine supports +map, we can use the map name directly
	{
		return { "+map", mapName };
	}
	else  // this engine only supports the old -warp, we must deduce map number
	{
		QRegularExpressionMatch match;
		if ((match = doom1MapNameRegex.match( mapName )).hasMatch())
		{
			return { "-warp", match.captured(1), match.captured(2) };
		}
		else if ((match = doom2MapNameRegex.match( mapName )).hasMatch())
		{
			return { "-warp", match.captured(1) };
		}
		else  // in case the WAD defines it's own map names, we have to resort to guessing the number by using its combo-box index
		{
			return { "-warp", QString::number( mapIdx + 1 ) };
		}
	}
}

QStringVec EngineTraits::getCompatLevelArgs( int compatLevel ) const
{
	assert( hasAppInfo() && hasFamilyTraits() );

	// Properly working -compatmode is present only in GZDoom,
	// for other ZDoom-based engines use at least something, even if it doesn't fully work.
	if (exeBaseName() == "gzdoom" || exeBaseName() == "vkdoom")
		return { "-compatmode", QString::number( compatLevel ) };
	else if (_familyTraits->compLvlStyle == CompatLevelStyle::ZDoom)
		return { "+compatmode", QString::number( compatLevel ) };
	else if (_familyTraits->compLvlStyle == CompatLevelStyle::PrBoom)
		return { "-complevel", QString::number( compatLevel ) };
	else
		return {};
}

QString EngineTraits::getCmdMonitorIndex( int ownIndex ) const
{
	assert( hasAppInfo() && hasFamilyTraits() );

	int startingMonitorIndex = 0;
	if (auto iter = startingMonitorIndexes.find( exeBaseName() ); iter != startingMonitorIndexes.end())
		startingMonitorIndex = iter.value();

	return QString::number( startingMonitorIndex + ownIndex );
}
