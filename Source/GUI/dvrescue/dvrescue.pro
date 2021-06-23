TEMPLATE = subdirs

USE_BREW = $$(USE_BREW)
!isEmpty(USE_BREW):equals(USE_BREW, true) {
    message("DEFINES += USE_BREW")
    DEFINES += USE_BREW
}

include(ffmpeg.pri)

contains(DEFINES, USE_BREW) {
    message('using ffmpeg from brew via PKGCONFIG')

    pkgConfig = "PKGCONFIG += libavdevice libavcodec libavfilter libavformat libpostproc libswresample libswscale libavcodec libavutil"
    linkPkgConfig = "CONFIG += link_pkgconfig"

    message('pkgConfig: ' $$pkgConfig)
    message('linkPkgConfig: ' $$linkPkgConfig)

} else {
    ffmpegIncludes = "INCLUDEPATH+=$$FFMPEG_INCLUDES"
    ffmpegLibs = "LIBS+=$$FFMPEG_LIBS"

    message('ffmpegIncludes: ' $$ffmpegIncludes)
    message('ffmpegLibs: ' $$ffmpegLibs)

    staticffmpeg = "CONFIG += static_ffmpeg"
    message('staticffmpeg: ' $$ffmpegLibs)
}

QTAVPLAYER = $$absolute_path($$_PRO_FILE_PWD_/dvrescue-QtAVPlayer)
message('QTAVPLAYER: ' $$QTAVPLAYER)
include($$QTAVPLAYER/QtAVPlayerLib.pri)

SUBDIRS += \
        dvrescue-qtavplayer \
	dvrescue \
        dvrescue_tests \
        dvrescue_qmltests

dvrescue-qtavplayer.file = dvrescue-QtAVPlayer/QtAVPlayerLib.pro

dvrescue.subdir = dvrescue
dvrescue_tests.subdir = dvrescue.tests
dvrescue_qmltests.subdir = dvrescue.qmltests

dvrescue.depends = dvrescue-qtavplayer
dvrescue_tests.depends = dvrescue-qtavplayer
dvrescue_qmltests.depends = dvrescue-qtavplayer
