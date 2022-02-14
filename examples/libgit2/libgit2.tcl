# Mingw build of libgit
# mkdir build
# cd build
# cmake .. -G "MinGW Makefiles"
# cmake --build .
# or maybe
# cmake --build . -DCMAKE_BUILD_TYPE=Release
# or
# cmake .. -G "MinGW Makefiles" -DBUILD_CLAR=OFF -DEMBED_SSH_PATH=/d/src/AAThirdparty/C,C++/libssh2-1.10.0 -DCMAKE_BUILD_TYPE=Release -DUSE_BUNDLED_ZLIB=ON
# Edit build2\src\CMakeFiles\git2.dir linklibs.rsp to add -lbcrypt
# For a verbose build, add "-- VERBOSE=1" before the cmake --build command


# To build debug with examples (64-bit)
# mkdir build
# cmake .. -A x64 -DBUILD_EXAMPLES=ON
# cmake --build .

# Build Debug with VC++/ssh
# (Note add -DCMAKE_BUILD_TYPE=Release for release builds)
# cd libgit2
# mkdir build
# cd build
#NOTE: USE FORWARD / in paths even for vc++ builds
# cmake .. -A x64 -DBUILD_EXAMPLES=ON -DBUILD_CLAR=OFF -DEMBED_SSH_PATH="D:/src/AAThirdparty/C,C++/libssh2-1.10.0" -DCMAKE_INSTALL_PREFIX="D:/temp/lg2"
# cmake --build . --target install

if {![info exists GIT_NS]} {
    set GIT_NS [namespace current]::git
}

namespace eval $GIT_NS {
    variable packageDirectory [file dirname [file normalize [info script]]]
}

namespace eval $GIT_NS {

    # This function must be called to initialize the package with the path
    # to the shared library to be wrapped. By default it will try
    # looking in a platform-specific dir on Windows only, otherwise leave
    # it up to the system loader to find one.
    proc init {{path ""}} {
        variable packageDirectory
        variable functionDefinitions
        variable libgit2Path


        # The order of loading these files is important!!!
        # Later files depend on earlier ones, just like the C headers
        variable scriptFiles {
            common.tcl
            strarray.tcl
            global.tcl
            errors.tcl
            types.tcl
            buffer.tcl
            oid.tcl
            oidarray.tcl
            repository.tcl
            indexer.tcl
            odb.tcl
            odb_backend.tcl
            object.tcl
            tree.tcl
            net.tcl
            refspec.tcl
            diff.tcl
            checkout.tcl
            blob.tcl
            tag.tcl
            cert.tcl
            credential.tcl
            transport.tcl
            pack.tcl
            proxy.tcl
            remote.tcl
            apply.tcl
            annotated_commit.tcl
            attr.tcl
            blame.tcl
            branch.tcl
            index.tcl
            merge.tcl
            cherrypick.tcl
            commit.tcl
            config.tcl
            credential_helpers.tcl
            describe.tcl
            email.tcl
            filter.tcl
            graph.tcl
            ignore.tcl
            mailmap.tcl
            message.tcl
            notes.tcl
            pathspec.tcl
            patch.tcl
            rebase.tcl
            refdb.tcl
            reflog.tcl
            refs.tcl
            reset.tcl
            revert.tcl
            revparse.tcl
            revwalk.tcl
            signature.tcl
            stash.tcl
            status.tcl
            submodule.tcl
            trace.tcl
            transaction.tcl
            worktree.tcl
            clone.tcl
        }

        if {$path eq ""} {
            set path "libgit2[info sharedlibextension]"
            if {$::tcl_platform(platform) eq "windows"} {
                # Prioritize loading from dir if possible over some system file
                if {$::tcl_platform(machine) eq "amd64"} {
                    set subdir AMD64
                } else {
                    set subdir X86
                }
                if {[file exists [file join $packageDirectory $subdir $path]]} {
                    set path [file join $packageDirectory $subdir $path]
                }
            }
        }
        set libgit2Path $path

        uplevel #0 {package require cffi}
        ::cffi::alias load C
        ::cffi::Wrapper create libgit2 $libgit2Path

        # String encoding expected by libgit2
        cffi::alias define STRING string.utf-8
        # Standard error handler
        cffi::alias define GIT_ERROR_CODE \
            [list int nonnegative [list onerror [namespace current]::ErrorCodeHandler]]
        cffi::alias define PSTRARRAY pointer.git_strarray
        cffi::alias define PSTRARRAYIN pointer.cffi_strarray

        cffi::alias define CB_PAYLOAD {pointer unsafe nullok}
        cffi::alias define git_object_size_t uint64_t

        # Note these are sourced into current namespace
        foreach file $scriptFiles {
            source [file join $packageDirectory $file]
        }

        set ret [git_libgit2_init]

        git_libgit2_version major minor rev
        if {$major != 1 || $minor != 3} {
            error "libgit2 version $major.$minor.$rev is not supported. This package requires version 1.3. Note libgit2 does not guarantee ABI compatibility between minor releases."
        }

        # Redefine to no-op
        proc init args "return $ret"
        return $ret
    }
}
