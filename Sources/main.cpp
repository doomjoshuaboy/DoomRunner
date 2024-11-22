//======================================================================================================================
// Project: DoomRunner
//----------------------------------------------------------------------------------------------------------------------
// Author:      Jan Broz (Youda008)
// Description:
//======================================================================================================================

#include "MainWindow.hpp"
#include "Themes.hpp"
#include "Utils/StandardOutput.hpp"

#include <QApplication>
#include <QDir>

// TODO
#include "Utils/OSUtils.hpp"
#include "EngineTraits.hpp"
#include <QStandardPaths>
#include <QTextStream>
#include <QDialog>
#include <QVBoxLayout>
#include <QPlainTextEdit>


//======================================================================================================================

int main( int argc, char * argv [] )
{
	QApplication a( argc, argv );

	// All stored relative paths are relative to the directory of this application,
	// launching it from a different current working directory would break it.
	QDir::setCurrent( QApplication::applicationDirPath() );

	initStdStreams();

	QString paths;
	QTextStream stream(&paths);

	stream << "QStandardPaths::HomeLocation:           " << QStandardPaths::writableLocation( QStandardPaths::HomeLocation ) << "\n";
	stream << "\n";
	stream << "QStandardPaths::GenericConfigLocation:  " << QStandardPaths::writableLocation( QStandardPaths::GenericConfigLocation ) << "\n";
	stream << "QStandardPaths::AppConfigLocation:      " << QStandardPaths::writableLocation( QStandardPaths::AppConfigLocation ) << "\n";
	stream << "QStandardPaths::GenericDataLocation:    " << QStandardPaths::writableLocation( QStandardPaths::GenericDataLocation ) << "\n";
	stream << "QStandardPaths::AppDataLocation:        " << QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) << "\n";
	stream << "QStandardPaths::AppLocalDataLocation:   " << QStandardPaths::writableLocation( QStandardPaths::AppLocalDataLocation ) << "\n";
	stream << "\n";
	stream << "QStandardPaths::DocumentsLocation:      " << QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ) << "\n";
	stream << "QStandardPaths::PicturesLocation:       " << QStandardPaths::writableLocation( QStandardPaths::PicturesLocation ) << "\n";
	stream << "\n";

	stream << "DoomRunner (debug) data:   " << os::getThisLauncherDataDir() << "\n\n";

	EngineTraits gzdoom_sysWide;
#if IS_WINDOWS
	gzdoom_sysWide.loadAppInfo("E:/Youda/Projects/DoomRunner/TestDoomDir/GZDoom-4.10.0-userdir/gzdoom.exe");
#else
	gzdoom_sysWide.loadAppInfo("/usr/games/gzdoom");
#endif
	stream << "GZDoom (sys-wide) executable:  " << gzdoom_sysWide.exePath() << "\n";
	stream << "GZDoom (sys-wide) config:      " << gzdoom_sysWide.getDefaultConfigDir() << "\n";
	stream << "GZDoom (sys-wide) data:        " << gzdoom_sysWide.getDefaultDataDir() << "\n";
	stream << "GZDoom (sys-wide) saves:       " << gzdoom_sysWide.getDefaultSaveDir() << "\n";
	stream << "GZDoom (sys-wide) screenshots: " << gzdoom_sysWide.getDefaultScreenshotDir() << "\n";
	stream << "\n";

	EngineTraits gzdoom_custom;
#if IS_WINDOWS
	gzdoom_custom.loadAppInfo("E:/Youda/Projects/DoomRunner/TestDoomDir/GZDoom-4.10.0-portable/gzdoom.exe");
#else
	gzdoom_custom.loadAppInfo("/opt/gzdoom");
#endif
	stream << "GZDoom (custom) executable:    " << gzdoom_custom.exePath() << "\n";
	stream << "GZDoom (custom) config:        " << gzdoom_custom.getDefaultConfigDir() << "\n";
	stream << "GZDoom (custom) data:          " << gzdoom_custom.getDefaultDataDir() << "\n";
	stream << "GZDoom (custom) saves:         " << gzdoom_custom.getDefaultSaveDir() << "\n";
	stream << "GZDoom (custom) screenshots:   " << gzdoom_custom.getDefaultScreenshotDir() << "\n";
	stream << "\n";

#if !IS_WINDOWS && !IS_MACOS
	EngineTraits gzdoom_flatpak;
	gzdoom_flatpak.loadAppInfo("/var/lib/flatpak/app/org.zdoom.GZDoom/current/active/files/bin/gzdoom");
	stream << "GZDoom (flatpak) executable:   " << gzdoom_flatpak.exePath() << "\n";
	stream << "GZDoom (flatpak) config:       " << gzdoom_flatpak.getDefaultConfigDir() << "\n";
	stream << "GZDoom (flatpak) data:         " << gzdoom_flatpak.getDefaultDataDir() << "\n";
	stream << "GZDoom (flatpak) saves:        " << gzdoom_flatpak.getDefaultSaveDir() << "\n";
	stream << "GZDoom (flatpak) screenshots:  " << gzdoom_flatpak.getDefaultScreenshotDir() << "\n";
	stream << "\n";
#endif

	stream.flush();

	QDialog dialog;
	dialog.setObjectName( "StandardPaths" );
	dialog.setWindowTitle( "Standard system paths" );
	dialog.setWindowModality( Qt::WindowModal );
	QVBoxLayout * layout = new QVBoxLayout( &dialog );
	QPlainTextEdit * textEdit = new QPlainTextEdit( &dialog );
	textEdit->setReadOnly( true );
	textEdit->setWordWrapMode( QTextOption::NoWrap );
	QFont font = QFontDatabase::systemFont( QFontDatabase::FixedFont );
	font.setPointSize( 10 );
	textEdit->setFont( font );
	textEdit->setPlainText( paths );
	layout->addWidget( textEdit );
	dialog.resize( 1000, 600 );
	dialog.exec();

	themes::init();

	MainWindow w;
	w.show();
	int exitCode = a.exec();

	return exitCode;
}
