//======================================================================================================================
// Project: DoomRunner
//----------------------------------------------------------------------------------------------------------------------
// Author:      Jan Broz (Youda008)
// Description: properties and capabilities of different engines
//======================================================================================================================

#ifndef ENGINE_TRAITS_INCLUDED
#define ENGINE_TRAITS_INCLUDED


#include "Essential.hpp"

#include "CommonTypes.hpp"
#include "Utils/OSUtils.hpp"  // AppInfo

#include <QString>
#include <QStringList>

#include <cassert>

struct Engine;


//----------------------------------------------------------------------------------------------------------------------

enum class MapParamStyle
{
	Warp,  // -warp 1 8
	Map,   // +map E1M8
};

//----------------------------------------------------------------------------------------------------------------------

enum class CompatLevelStyle
{
	None,
	ZDoom,   // https://zdoom.org/wiki/CVARs:Configuration#compatmode
	PrBoom,  // https://doom.fandom.com/wiki/PrBoom#Compatibility_modes_and_settings
};

const QStringList & getCompatLevels( CompatLevelStyle style );

//----------------------------------------------------------------------------------------------------------------------

// https://upload.wikimedia.org/wikipedia/commons/a/a8/Doom-ports.svg
enum class EngineFamily
{
	ZDoom,
	PrBoom,
	MBF,  // Marine's Best Friend
	ChocolateDoom,

	_EnumEnd  ///< indicates an error
};
const char * familyToStr( EngineFamily family );
EngineFamily familyFromStr( const QString & familyStr );

//----------------------------------------------------------------------------------------------------------------------

/// Traits that are shared among different engines belonging to the same family.
struct EngineFamilyTraits
{
	MapParamStyle mapParamStyle;
	CompatLevelStyle compLvlStyle;
	const char * saveDirParam;
	bool hasScreenshotDirParam;
	bool needsStdoutParam;
};

/// Properties and capabilities of a particular engine that decide what command-line parameters will be used.
class EngineTraits {

	std::optional< os::AppInfo > _appInfo;
	const EngineFamilyTraits * _familyTraits = nullptr;

 public:

	// initialization

	/// Initializes application info.
	/** This may open and read the executable file, which may be a time-expensive operation. */
	void loadAppInfo( const QString & executablePath )  { _appInfo = os::loadAppInfo( executablePath ); }
	bool hasAppInfo() const                             { return _appInfo.has_value(); }

	/// Initializes family traits according to specified engine family.
	void assignFamilyTraits( EngineFamily family );
	bool hasFamilyTraits() const                        { return _familyTraits != nullptr; }

	// application properties - requires application info to be loaded

	const auto & exePath() const             { assert( hasAppInfo() ); return _appInfo->exePath; }
	const auto & exeBaseName() const         { assert( hasAppInfo() ); return _appInfo->exeBaseName; }

	const auto & sandboxType() const         { assert( hasAppInfo() ); return _appInfo->sandboxEnv.type; }
	const auto & sandboxAppName() const      { assert( hasAppInfo() ); return _appInfo->sandboxEnv.appName; }
	const auto & sandboxHomeDir() const      { assert( hasAppInfo() ); return _appInfo->sandboxEnv.homeDir; }

	const auto & exeAppName() const          { assert( hasAppInfo() ); return _appInfo->versionInfo.appName; }
	const auto & exeDescription() const      { assert( hasAppInfo() ); return _appInfo->versionInfo.description; }
	const auto & exeVersion() const          { assert( hasAppInfo() ); return _appInfo->versionInfo.version; }

	const auto & displayName() const         { assert( hasAppInfo() ); return _appInfo->displayName; }
	const auto & normalizedName() const      { assert( hasAppInfo() ); return _appInfo->normalizedName; }

	// default directories and path requirements - requires application info to be loaded

	QString getDefaultConfigDir() const;
	QString getDefaultDataDir() const;
	QString getDefaultSaveDir() const;
	QString getDefaultScreenshotDir() const;

	enum class SaveBaseDir
	{
		WorkingDir,  ///< path of save file must be relative to the current working directory
		SaveDir,     ///< path of save file must be relative to the -savedir argument if present or engine's data dir otherwise
	};
	SaveBaseDir baseDirStyleForSaveFiles() const;

	// command line parameters deduction - requires application info and family traits to be initialized

	CompatLevelStyle compatLevelStyle() const   { assert( hasFamilyTraits() ); return _familyTraits->compLvlStyle; }
	bool supportsCustomMapNames() const         { assert( hasFamilyTraits() ); return _familyTraits->mapParamStyle == MapParamStyle::Map; }

	const char * saveDirParam() const           { assert( hasFamilyTraits() ); return _familyTraits->saveDirParam; }
	bool hasScreenshotDirParam() const          { assert( hasFamilyTraits() ); return _familyTraits->hasScreenshotDirParam; }

	bool needsStdoutParam() const               { assert( hasFamilyTraits() ); return _familyTraits->needsStdoutParam; }

	// generates either "-warp 2 5" or "+map E2M5" depending on the engine capabilities
	QStringVec getMapArgs( int mapIdx, const QString & mapName ) const;

	// generates either "-complevel x" or "+compatmode x" depending on the engine capabilities
	QStringVec getCompatLevelArgs( int compatLevel ) const;

	// some engines index monitors from 1 and others from 0
	QString getCmdMonitorIndex( int ownIndex ) const;

	// miscellaneous

	// EngineFamily is user-overridable in EngineDialog, but this is our default automatic detection
	EngineFamily guessEngineFamily() const;

 private:

	bool isGZDoomVersionOrLater( Version atLeastVersion ) const;

};


#endif // ENGINE_TRAITS_INCLUDED
