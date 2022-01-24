# Based on libgit2/include/blame.h
#
# This file should be sourced into whatever namespace commands should
# be created in.

::cffi::alias define PBLAME pointer.git_blame

::cffi::enum define git_blame_flag_t {
    GIT_BLAME_NORMAL                          0
    GIT_BLAME_TRACK_COPIES_SAME_FILE          1
    GIT_BLAME_TRACK_COPIES_SAME_COMMIT_MOVES  2
    GIT_BLAME_TRACK_COPIES_SAME_COMMIT_COPIES 4
    GIT_BLAME_TRACK_COPIES_ANY_COMMIT_COPIES  8
    GIT_BLAME_FIRST_PARENT                    16
    GIT_BLAME_USE_MAILMAP                     32
    GIT_BLAME_IGNORE_WHITESPACE               64
}
::cffi::alias define GIT_BLAME_FLAG_T {int {enum git_blame_flag_t} bitmask}

::cffi::Struct create git_blame_options {
    version              uint
    flags                uint32_t
    min_match_characters uint16_t
    newest_commit        struct.git_oid
    oldest_commit        struct.git_oid
    min_line             size_t
    max_line             size_t
}

::cffi::Struct create git_blame_hunk {
    lines_in_hunk           size_t
    final_commit_id         struct.git_oid
    final_start_line_number size_t
    final_signature         pointer.git_signature
    orig_commit_id          struct.git_oid
    orig_path               STRING
    orig_start_line_number  size_t
    orig_signature          pointer.git_signature
    boundary                schar
}

libgit2 functions {
    git_blame_options_init GIT_ERROR_CODE {
        opts    {struct.git_blame_options out}
        version {uint {default 1}}
    }
    git_blame_get_hunk_count uint32_t {
        blame PBLAME
    }
    git_blame_get_hunk_byindex {struct.git_blame_hunk byref} {
        blame PBLAME
        index uint32_t
    }
    git_blame_get_hunk_byline {struct.git_blame_hunk byref} {
        blame  PBLAME
        lineno size_t
    }
    git_blame_file GIT_ERROR_CODE {
        pBlame  {PBLAME  out}
        pRepo   PREPOSITORY
        path    STRING
        options {struct.git_blame_options byref nullifempty}
    }
    git_blame_buffer GIT_ERROR_CODE {
        pBlame     {PBLAME out}
        reference  PBLAME
        buffer     STRING
        buffer_len size_t
    }
    git_blame_free void {
        pBlame {PBLAME dispose}
    }
}