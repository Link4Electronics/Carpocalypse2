#ifndef __MOVIES__
#define __MOVIES__

#include "c2_hooks.h"
#include "Files.h"
#include "MacTypes.h"

#include "ImageCompression.h"

typedef struct MediaType**              Media;
typedef struct MovieType**              Movie;
typedef struct TrackType**              Track;

enum {
    flattenAddMovieToDataFork = 1L << 0,
};
enum {
    createMovieFileDeleteCurFile  = 1L << 31,
};

extern OSErr C2_HOOK_CDECL EnterMovies(void);

extern OSErr C2_HOOK_CDECL CloseMovieFile(short resRefNum);

extern void C2_HOOK_CDECL DisposeMovie(Movie theMovie);

extern OSErr C2_HOOK_CDECL EndMediaEdits(Media theMedia);

extern TimeValue C2_HOOK_CDECL GetMediaDuration(Media theMedia);

extern OSErr C2_HOOK_CDECL InsertMediaIntoTrack(
    Track       theTrack,
    TimeValue   trackStart,
    TimeValue   mediaTime,
    TimeValue   mediaDuration,
    Fixed       mediaRate);


extern Movie  C2_HOOK_CDECL FlattenMovieData(
    Movie           theMovie,
    long            movieFlattenFlags,
    const FSSpec *  theFile,
    OSType          creator,
    ScriptCode      scriptTag,
    long            createMovieFileFlags);

extern OSErr C2_HOOK_CDECL CreateMovieFile(
    const FSSpec *  fileSpec,
    OSType          creator,
    ScriptCode      scriptTag,
    long            createMovieFileFlags,
    short *         resRefNum,
    Movie *         newmovie);

#endif /* __MOVIES__ */
