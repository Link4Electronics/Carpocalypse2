#include "video.h"

#include "font.h"

#include "errors.h"

#include "globvars.h"
#include "grafdata.h"
#include "graphics.h"

#include "loading.h"

#include "movie.h"

#include "platform.h"
#include "utility.h"

#include <FileTypesAndCreators.h>
#include <GXMath.h>
#include <Movies.h>
#include <QTML.h>
#include <Script.h>
#include <TextUtils.h>

#include <string.h>

#include "c2_string.h"
// GLOBAL: CARMA2_HW 0x006a0c24
extern int gQuickTime_initialized;
// GLOBAL: CARMA2_HW 0x006a23b8
int gShow_status_message;

// GLOBAL: CARMA2_HW 0x006a23bc
int gStatus_message_1;

// GLOBAL: CARMA2_HW 0x006a23c0
int gStatus_message_2;

// GLOBAL: CARMA2_HW 0x006a0c50
undefined4 gNext_replay_record_state;

// GLOBAL: CARMA2_HW 0x006a0af0
short gQuicktime_movie_file;

// GLOBAL: CARMA2_HW 0x006a0af8
int gQuicktime_max_compression_size;

// GLOBAL: CARMA2_HW 0x006a0c30
Movie gQuicktime_movie;

// GLOBAL: CARMA2_HW 0x006a0c34
Handle gQuicktime_frame_pixmap_handle;

// GLOBAL: CARMA2_HW 0x006a0c40
Handle gQuicktime_image_description_handle;

// GLOBAL: CARMA2_HW 0x006a0c3c
Handle gQuicktime_compressed_frame_handle;

// GLOBAL: CARMA2_HW 0x006a0c38
void* gQuicktime_compressed_frame_pixels;

// GLOBAL: CARMA2_HW 0x006a0c44
br_uint_16* gQuicktime_frame_pixels;

// GLOBAL: CARMA2_HW 0x006a0c48
br_uint_16* gQuicktime_frame_pixel_conversion_table;

// GLOBAL: CARMA2_HW 0x006a0af4
ImageSequence gQuicktime_image_sequence;

// GLOBAL: CARMA2_HW 0x006a0afc
Media gQuicktime_media;

// GLOBAL: CARMA2_HW 0x006a0c10
Track gQuicktime_track;

// GLOBAL: CARMA2_HW 0x006a0c2c
int gQuicktime_movie_counter;

// GLOBAL: CARMA2_HW 0x006a0b08
struct FSSpec gQt_file_spec;

// GLOBAL: CARMA2_HW 0x006a0c18
struct Rect gQuicktime_rect;

// GLOBAL: CARMA2_HW 0x0065ef2c
int gQuicktime_frame_flags = 1;

// GLOBAL: CARMA2_HW 0x0065ef34
undefined2 DAT_0065ef34;

// GLOBAL: CARMA2_HW 0x006a0c28
int gRecording_paused;

// GLOBAL: CARMA2_HW 0x006a0c54
tU32 gRecording_time;

// GLOBAL: CARMA2_HW 0x006baa28

// GLOBAL: CARMA2_HW 0x006baa28
int gRecording_mouse_disabled;


// FUNCTION: CARMA2_HW 0x004e2100
int C2_HOOK_FASTCALL GetNextReplayRecordState(void) {

    return gNext_replay_record_state;
}

// FUNCTION: CARMA2_HW 0x004e2110
void C2_HOOK_FASTCALL KillStatusMessage(void) {

    gShow_status_message = 0;
}

// C2PORT-NONMATCHING: best 68.32% | difftest PASS 2000 trials (spec difftest/specs/004e1400_FUN_004e1400.json) | reason VC5-pixel-loop-codegen
// FUNCTION: CARMA2_HW 0x004e1400
void C2_HOOK_FASTCALL FUN_004e1400(void* pFrame_pixmap_handle, br_pixelmap* pScreen) {
    int i;
    int width;
    int height;
    int pixel_step;
    int top;
    int row;
    int col;

    (void)pFrame_pixmap_handle;
    if (gQuicktime_frame_pixel_conversion_table == NULL) {
        gQuicktime_frame_pixel_conversion_table = BrMemAllocate(0x20000, 0xfd);
        if (gNbPixelBits == 0x10) {
            i = 0;
            do {
                unsigned int value;
                br_uint_8 blue;

                value = ((unsigned int)i >> 1) & 0x7fe0;
                blue = (br_uint_8)i;
                value |= blue & 0x1f;
                i++;
                gQuicktime_frame_pixel_conversion_table[i - 1] =
                        ((value & 0xff) << 8) | ((value >> 8) & 0xff);
            } while (i < 0x10000);
        } else {
            br_uint_32* palette;

            palette = gRender_palette->pixels;
            for (i = 0; i < 256; i++) {
                br_uint_32 colour;
                unsigned int value;

                colour = palette[i];
                value = (((colour >> 19) & 0x1f) << 5) | ((colour >> 11) & 0x1f);
                value = (value << 5) | ((colour >> 3) & 0x1f);
                gQuicktime_frame_pixel_conversion_table[i] =
                        ((value & 0xff) << 8) | ((value >> 8) & 0xff);
            }
        }
    }

    PossibleLock(1);
    if (gNbPixelBits == 0x10) {
        width = gCurrent_graf_data->width / 2;
    } else {
        width = gCurrent_graf_data->width;
    }
    height = gCurrent_graf_data->height;
    if (gNbPixelBits == 0x10) {
        height /= 2;
    }

    if (gNbPixelBits == 0x10) {
        pixel_step = 2;
        if (pScreen->height == 200) {
            top = 20;
            height = 200;
            pixel_step = 1;
        } else {
            top = 0;
        }
        row = 0;
        if (height > 0) {
            int destination_offset;

            destination_offset = top * width * 2;
            do {
                if (width > 0) {
                    int source_x;
                    int destination_pixel;

                    source_x = 0;
                    destination_pixel = destination_offset / 2;
                    col = width;
                    do {
                        int source_pixel;

                        source_pixel = (pixel_step * row * (int)pScreen->row_bytes) / 2 + source_x;
                        gQuicktime_frame_pixels[destination_pixel] =
                                gQuicktime_frame_pixel_conversion_table[((br_uint_16*)pScreen->pixels)[source_pixel]];
                        destination_pixel++;
                        source_x += pixel_step;
                        col--;
                    } while (col != 0);
                }
                row++;
                destination_offset += width * 2;
            } while (row < height);
            PossibleUnlock(1);
            return;
        }
    } else {
        row = 0;
        do {
            if (width > 0) {
                col = 0;
                do {
                    int source_pixel;

                    source_pixel = row * (int)pScreen->row_bytes + col;
                    gQuicktime_frame_pixels[row * width + col] =
                            gQuicktime_frame_pixel_conversion_table[((br_uint_8*)pScreen->pixels)[source_pixel]];
                    col++;
                } while (col < width);
            }
            row++;
        } while (row < 200);
    }
    PossibleUnlock(1);
}

void C2_HOOK_FASTCALL ShowStatusMessage(int msg1, int msg2) {

    gShow_status_message = 1;
    gStatus_message_1 = msg1;
    gStatus_message_2 = msg2;
}

// FUNCTION: CARMA2_HW 0x004e2190
void C2_HOOK_FASTCALL TidyMovieARDisplay(void) {
    undefined4 state;

    state = gNext_replay_record_state;
    gNext_replay_record_state = 20;
    RenderAFrame(1);
    RenderAFrame(1);
    gNext_replay_record_state = state;
}

void C2_HOOK_FASTCALL FreeOffQTshite(void) {
    tPath_name path;

    if (gQuicktime_movie_file != 0) {
        CloseMovieFile(gQuicktime_movie_file);
    }
    DisposeMovie(gQuicktime_movie);
    PDFileDelete(gQuick_time_temp_path, 1);
    sprintf(path, "%s.#Res", gQuick_time_temp_path);
    PDFileDelete(path, 1);
    if (gQuicktime_frame_pixmap_handle != NULL) {
        DisposeHandle(gQuicktime_frame_pixmap_handle);
        gQuicktime_frame_pixmap_handle = NULL;
    }
    if (gQuicktime_image_description_handle != NULL) {
        DisposeHandle(gQuicktime_image_description_handle);
        gQuicktime_image_description_handle = NULL;
    }
    if (gQuicktime_compressed_frame_handle != NULL) {
        DisposeHandle(gQuicktime_compressed_frame_handle);
        gQuicktime_compressed_frame_handle = NULL;
    }
}



// FUNCTION: CARMA2_HW 0x004e2140
int C2_HOOK_FASTCALL GetMovieWidth(void) {

    if (gNbPixelBits == 16) {
        return gCurrent_graf_data->width / 2;
    } else {
        return gCurrent_graf_data->width;
    }
}

// FUNCTION: CARMA2_HW 0x004e2160
int C2_HOOK_FASTCALL GetMovieHeight(void) {

    if (gNbPixelBits == 16) {
        return gCurrent_graf_data->height / 2;
    } else {
        return gCurrent_graf_data->height;
    }
}

// FUNCTION: CARMA2_HW 0x004e0f20
OSErr C2_HOOK_FASTCALL InitMovie(void) {

    gQuicktime_rect.top = 0;
    gQuicktime_rect.left = 0;
    gQuicktime_rect.right = GetMovieWidth();
    gQuicktime_rect.bottom = GetMovieHeight();
    if (CreateMovieFile(&gQt_file_spec, sigMoviePlayer, smCurrentScript, createMovieFileDeleteCurFile, &gQuicktime_movie_file, &gQuicktime_movie) != noErr) {
        PathCat(gQuick_time_temp_path, gApplication_path, "QTTMP");
        PathCat(gQuick_time_movie_path_stub, gApplication_path, "MOVIE");
        return CreateMovieFile(&gQt_file_spec, sigMoviePlayer, smCurrentScript, createMovieFileDeleteCurFile, &gQuicktime_movie_file, &gQuicktime_movie);
    }
}

// FUNCTION: CARMA2_HW 0x004e0fe0
int C2_HOOK_FASTCALL CreateMovie(void) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e1c70
short C2_HOOK_FASTCALL GenerateOneMovieFrame(br_pixelmap* pScreen) {
    int frame_flags;
    int pixmap_width;
    int frame_size;
    char codec_info;
    short result;

    if (gQuicktime_frame_flags != 0) {
        frame_flags = 0xc0;
        gQuicktime_frame_flags = 0;
    } else {
        frame_flags = 0x40;
    }
    PossibleLock(1);
    *(void**)(*gQuicktime_frame_pixmap_handle) = gQuicktime_frame_pixels;
    if (gNbPixelBits == 16) {
        pixmap_width = gCurrent_graf_data->width / 2;
    } else {
        pixmap_width = gCurrent_graf_data->width;
    }
    *(short*)(*gQuicktime_frame_pixmap_handle + 4) = pixmap_width * 2;
    FUN_004e1400(gQuicktime_frame_pixmap_handle, pScreen);
    result = FUN_0056d8c0(gQuicktime_image_sequence, gQuicktime_frame_pixmap_handle,
            &gQuicktime_rect, frame_flags, *gQuicktime_compressed_frame_handle,
            &frame_size, &codec_info, 0);
    if (result != 0) {
        PossibleUnlock(1);
        return result;
    }
    *(int*)(*gQuicktime_image_description_handle + 0x2c) = frame_size;
    result = FUN_0056b440(gQuicktime_media, gQuicktime_compressed_frame_handle, 0,
            *(int*)(*gQuicktime_image_description_handle + 0x2c),
            0x37, gQuicktime_image_description_handle, 1, 0, 0);
    if (result != 0) {
        PossibleUnlock(1);
        return result;
    }
    PossibleUnlock(1);
    return 0;
}

void C2_HOOK_FASTCALL TrapS3SoundOutput(void) {
}

void C2_HOOK_FASTCALL FreeS3SoundOutput(void) {
}

void C2_HOOK_FASTCALL WriteBannerFrame(void) {
    // Placeholder implementation - actual implementation should be in movie.c or another file
    // This is needed to resolve the undefined reference error
}

// FUNCTION: CARMA2_HW 0x004e1da0
int C2_HOOK_FASTCALL MovieRecordFrame(br_pixelmap *pScreen, int pFrame_period) {
    tU32 remaining;

    switch (gNext_replay_record_state) {
    case 1:
        gNext_replay_record_state = 2;
        return 0;
    case 2:
        WaitFor(2000);
        if (InitMovie()) {
            gNext_replay_record_state = 10;
            ShowStatusMessage(eMiscString_movie_error, eMiscString_aborting);
            return 0;
        }
        gNext_replay_record_state = 3;
        ShowStatusMessage(eMiscString_initialising_movie, eMiscString_please_wait);
        return 0;
    case 3:
        if (CreateMovie()) {
            gNext_replay_record_state = 10;
            ShowStatusMessage(eMiscString_movie_error, eMiscString_aborting);
            return 0;
        }
        gNext_replay_record_state = 4;
        ShowStatusMessage(eMiscString_recording_movie, 0);
        TrapS3SoundOutput();
        return 0;
    case 4:
        if (!gRecording_paused) {
            remaining = gRecording_time + pFrame_period;
            gRecording_paused = 0;
            for (; remaining > 1000 / 18; remaining -= 1000 / 18) {
                if (GenerateOneMovieFrame(pScreen) != 0) {
                    gNext_replay_record_state = 10;
                    ShowStatusMessage(eMiscString_movie_error, eMiscString_aborting);
                    return 0;
                }
            }
            gRecording_time = remaining;
            return 0;
        }
        break;
    case 5:
        gRecording_mouse_disabled = 0;
        return 0;
    case 10:
        WaitFor(2000);
        FreeOffQTshite();
        FreeS3SoundOutput();
        return 0;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e7350
void C2_HOOK_FASTCALL DrawStatusMessage(int pReplay_only) {
    int x;
    int y;

    if (gShow_status_message != 0) {
        if (gNext_replay_record_state == 4) {
            x = gCurrent_graf_data->field_0x350;
            y = gCurrent_graf_data->field_0x354;
            if (pReplay_only == 0) {
                DRPixelmapText(gBack_screen, x, y, &gFonts[1], GetMiscString(gStatus_message_1), gBack_screen->width);
            }
        } else {
            x = gCurrent_graf_data->width / 2;
            y = gCurrent_graf_data->height / 2;
            DRPixelmapCentredText(gBack_screen, x, y - 0x40, &gFonts[4], GetMiscString(gStatus_message_1));
            if (gStatus_message_2 != 0) {
                DRPixelmapCentredText(gBack_screen, x, y - 0x20, &gFonts[6], GetMiscString(gStatus_message_2));
            }
        }
    }
}
