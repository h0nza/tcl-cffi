# (c) 2021 Ashok P. Nadkarni
# See LICENSE for license terms.
#
# Contains common definitions across test scripts

package require tcltest
eval tcltest::configure $argv

set NS cffi
if {[namespace exists ${NS}::test]} {
    return
}

package require $NS

namespace eval cffi::test {
    namespace import ::tcltest::test ::tcltest::testConstraint

    if {$::tcl_platform(platform) eq "windows"} {
        proc onwindows {} {return 1}
    } else {
        proc onwindows {} {return 0}
    }

    if {$::tcl_platform(pointerSize) == 4} {
        proc pointer32 {} {return 1}
    } else {
        proc pointer32 {} {return 0}
    }

    testConstraint structbyval [cffi::pkgconfig get structbyval]

    variable testDllPath [file normalize [file join [file dirname $::cffi::dll_path] cffitest[info sharedlibextension]]]
    variable unicharSize [expr {[package vsatisfies [info tclversion] 9] ? 4 : 2}]
    cffi::Wrapper create testDll $testDllPath
    testDll function getTestStructSize int {}
    if {[cffi::pkgconfig get backend] eq "dyncall"} {
        cffi::dyncall::Symbols create testSyms $testDllPath
        tcltest::testConstraint dyncall 1
    } else {
        tcltest::testConstraint libffi 1
    }
    if {[onwindows] && [pointer32]} {
        tcltest::testConstraint win32 1
    } else {
        tcltest::testConstraint notwin32 1
    }
    variable testStructSize
    set testStructSize [getTestStructSize]

    namespace path [namespace parent]

    variable voidTypes {void};  # Currently only one but that might change
    variable signedIntTypes {schar short int long longlong}
    variable unsignedIntTypes {uchar ushort uint ulong ulonglong}
    variable realTypes {float double}
    variable intTypes [concat $signedIntTypes $unsignedIntTypes]
    variable numericTypes [concat $intTypes $realTypes]
    variable stringTypes {string unistring binary}
    variable charArrayTypes {chars unichars bytes}
    if {[onwindows]} {
        lappend stringTypes winstring
        lappend charArrayTypes winchars
    }
    variable pointerTypes {pointer}

    variable baseTypes [concat $voidTypes $numericTypes $pointerTypes $stringTypes $charArrayTypes uuid]

    proc makeptr {p {tag {}}} {
        set width [expr {$::tcl_platform(pointerSize) * 2}]
        return [format "0x%.${width}lx" $p]^$tag
    }

    variable intMax
    variable intMin
    array set intMax {
        schar 127
        uchar 255
        short 32767
        ushort 65535
        int 2147483647
        uint 4294967295
        longlong   9223372036854775807
        ulonglong 18446744073709551615
    }
    array set intMin {
        schar -128
        uchar 0
        short -32768
        ushort 0
        int -2147483648
        uint 0
        longlong -9223372036854775808
        ulonglong 0
    }
    if {$::tcl_platform(wordSize) == 8} {
        set intMax(long)   $intMax(longlong)
        set intMax(ulong) $intMax(ulonglong)
        set intMin(long)   $intMin(longlong)
        set intMin(ulong) $intMin(ulonglong)
    } else {
        set intMax(long)   $intMax(int)
        set intMax(ulong) $intMax(uint)
        set intMin(long)   $intMin(int)
        set intMin(ulong) $intMin(uint)
    }

    # Test strings to be used for various encodings
    set testStrings(ascii) "abc"
    set testStrings(jis0208) \u543e
    set testStrings(unicode) \u00e0\u00e1\u00e2
    set testStrings(bytes) \x01\x02\x03

    cffi::Struct create ::StructValue {
        c schar
        i int
    } -clear
    cffi::Union create ::UnionValue {
        c schar
        i int
    } -clear
    cffi::Struct create ::StructWithVLA {
        count ushort
        values int[count]
    }
    # Generic test values for all types
    variable testValues
    foreach type {schar short int long longlong} {
        set testValues($type) $intMin($type)
    }
    foreach type {uchar ushort uint ulong ulonglong} {
        set testValues($type) $intMax($type)
    }
    set testValues(float) 42.0
    set testValues(double) 42.0
    set testValues(pointer\ unsafe) [makeptr 1]
    set testValues(chars\[[expr {[string length $testStrings(ascii)]+1}]\]) $testStrings(ascii)
    set testValues(unichars\[[expr {[string length $testStrings(unicode)]+1}]\]) $testStrings(unicode)
    set testValues(bytes\[[string length $testStrings(bytes)]\]) $testStrings(bytes)
    set testValues(string) $testStrings(ascii)
    set testValues(unistring) $testStrings(unicode)
    set testValues(binary) $testStrings(bytes)
    set testValues(struct.::StructValue) {c 42 i 4242}
    set testValues(union.::UnionValue) \x01\x02\x03\x04
    if {[onwindows]} {
        set testValues(winstring) $testStrings(unicode)
        set testValues(winchars\[[expr {[string length $testStrings(unicode)]+1}]\]) $testStrings(unicode)
    }
    set testValues(uuid) 01020304-0506-0708-090a-0b0c0d0e0f00

    variable badValues
    foreach type $numericTypes {
        set badValues($type) notanumber
    }
    set badValues(pointer\ unsafe) 1
    set badValues(struct.::StructValue) {x}
    set badValues(union.::UnionValue) \x01\x02\x03


    variable paramDirectionAttrs {in out inout}
    variable pointerAttrs {novaluechecks unsafe dispose counted pinned}
    variable stringAttrs {novaluechecks nullifempty}
    variable requirementAttrs {zero nonzero nonnegative positive}
    variable errorHandlerAttrs {errno}
    if {[onwindows]} {
        lappend errorAttrs winerror lasterror
        lappend errorHandlerAttrs winerror lasterror
    }
    variable errorAttrs [concat $requirementAttrs $errorHandlerAttrs]
    variable typeAttrs [concat $paramDirectionAttrs $pointerAttrs $errorAttrs byref saveerror]

    variable invalidFieldAttrs [list {*}$paramDirectionAttrs retval byref storeonerror storealways]


    # Error messages
    variable errorMessages
    array set errorMessages {
        unsupportedarraytype {The specified type is invalid or unsupported for array declarations.}
        typeparsemode {The specified type is not valid for the type declaration context.}
        attrparsemode {A type annotation is not valid for the declaration context.}
        attrtype {A type annotation is not valid for the data type.}
        attrconflict {Unknown, repeated or conflicting type annotations specified.}
        arraysize {Invalid array size or extra trailing characters.}
        scalarchars {Declarations of type chars, unichars, winchars and bytes must be arrays.}
        arrayreturn {Function return type must not be an array.}
        namesyntax {Invalid name syntax.}
        attrparamdir {One or more annotations are invalid for the parameter direction.}
        defaultdisallowed {Defaults are not allowed in this declaration context.}
        needbyref {Pass by value is not supported for this type. Annotate with "byref" to pass by reference if function expects a pointer.}
        store {Annotations "storeonerror" and "storealways" not allowed for "in" parameters.}
        fieldvararray {Fields cannot be arrays of variable size.}
        retvaluse {The "retval" annotation can only be used in parameter definitions in functions with void or integer return types with error checking annotations. Error defining function *}
    }

    set structDef {
        s string
        utf8 string.utf-8
        jis string.jis0208
        uni unistring
    }
    if {[onwindows]} {
        lappend structDef win winstring
    }
    cffi::Struct create ::StructWithStrings $structDef

    proc makeStructWithStrings {} {
        variable testStrings
        set struct [list s $testStrings(ascii) utf8 $testStrings(unicode) jis $testStrings(jis0208) uni $testStrings(unicode)]
        if {[onwindows]} {
            lappend struct win $testStrings(unicode)
        }
        return $struct
    }
    cffi::Struct create ::cffi::test::InnerTestStruct {
        c chars[15]
    }
    set def {
        c  schar
        i  int
        shrt short
        uint uint
        ushrt ushort
        l  long
        uc uchar
        ul ulong
        chars chars[11]
        ll longlong
        unic unichars[7]
        ull ulonglong
        b   bytes[3]
        f float
        s struct.cffi::test::InnerTestStruct
        d double
    }
    if {[onwindows]} {
        lappend def wchars {winchars[13]}
    }
    cffi::Struct create ::TestStruct $def

    proc checkTestStruct {sdict} {
        # Returns list of mismatched fields assuming as initialzed
        # by getTestStruct in tclCffiTest.c
        variable intMax
        variable intMin

        set values [list \
                        c $intMin(schar) \
                        i $intMin(int) \
                        shrt $intMin(short) \
                        uint $intMax(uint) \
                        ushrt $intMax(ushort) \
                        l $intMin(long) \
                        uc $intMax(uchar) \
                        ul $intMax(ulong) \
                        chars CHARS \
                        ll $intMin(longlong) \
                        unic UNIC \
                        ull $intMax(ulonglong) \
                        b "\x01\x02\x03" \
                        f -0.25 \
                        s [list c INNER] \
                        d 0.125]
        if {[onwindows]} {
            lappend values wchars WCHARS
        }
        set mismatches {}
        foreach {fld val} $values {
            if {[dict get $sdict $fld] != $val} {
                puts "$fld: [dict get $sdict $fld] != $val"
                lappend mismatches $fld
            }
        }
        return $mismatches
    }

    cffi::Union create ::TestUnion {
        i   int
        dbl double
        uc  uchar
    }
    variable testUnionSize 8
}

proc cffi::test::command_exists {cmd} {
    return [expr {[uplevel #0 namespace which $cmd] eq $cmd}]
}

proc nsqualify {name {ns {}}} {
    if {[string equal -length 2 :: $name]} {
        return $name
    }
    if {$ns eq {}} {
        set ns [uplevel 1 namespace current]
    }
    set ns [string trimright $ns :]
    return ${ns}::$name
}


proc cffi::test::scoped_ptr {p tag} {
    return [makeptr $p [nsqualify $tag [uplevel 1 namespace current]]]
}

proc cffi::test::lprefix {prefix args} {
    lmap e $args {
        return -level 0 $prefix$e
    }
}

proc cffi::test::lprefixns {args} {
    set ns [uplevel 1 namespace current]
    return [lmap e $args {
        nsqualify $e $ns
    }]
}

proc cffi::test::lprefixcffi {args} {
    return [lmap e $args {
        nsqualify $e ::cffi
    }]
}

proc cffi::test::lprefixcffic {args} {
    return [lmap e $args {
        nsqualify $e ::cffi::c
    }]
}


# step better be > 0
proc seq {start count {step 1}} {
    set seq {}
    for {set i 0} {$i < $count} {incr i} {
        lappend seq $start
        incr start $step
    }
    return $seq
}

# Verify two dicts are equal
proc cffi::test::dict_equal {l1 l2} {
    if {[dict size $l1] != [dict size $l2]} {
        return 0
    }
    dict for {k val} $l1 {
        if {![dict exists $l2 $k] || $val != [dict get $l2 $k]} {
            return 0
        }
        dict unset l2 $k
    }
    return [expr {[dict size $l2] == 0}]
}
tcltest::customMatch dict ::cffi::test::dict_equal

proc cffi::test::testsubcmd {cmd} {
    test [namespace tail $cmd]-missing-subcmd-0 "Missing subcommand" -body {
        $cmd
    } -result "wrong # args: should be \"$cmd subcommand ?arg ...?\"" -returnCodes error
}

proc cffi::test::testnumargs {label cmd {fixed {}} {optional {}} args} {
    set minargs [llength $fixed]
    set maxargs [expr {$minargs + [llength $optional]}]
    if {[lindex $optional end] eq "..."} {
        unset maxargs
    }
    set message "wrong # args: should be \"$cmd"
    if {[llength $fixed]} {
        append message " $fixed"
    }
    if {[llength $optional]} {
        append message " $optional"
    }
    if {[llength $fixed] == 0 && [llength $optional] == 0} {
        append message " \""
    } else {
        append message "\""
    }
    if {$minargs > 0} {
        set arguments [lrepeat [expr {$minargs-1}] x]
        test $label-minargs-0 "$label no arguments" \
            -body "$cmd" \
            -result $message -returnCodes error \
            {*}$args
        if {$minargs > 1} {
            test $label-minargs-1 "$label missing arguments" \
                -body "$cmd $arguments" \
                -result $message -returnCodes error \
            {*}$args
        }
    }
    if {[info exists maxargs]} {
        set arguments [lrepeat [expr {$maxargs+1}] x]
        test $label-maxargs-0 "$label extra arguments" \
            -body "$cmd $arguments" \
            -result $message -returnCodes error \
            {*}$args
    }
}

proc cffi::test::purge_pointers {} {
    foreach ptr [cffi::pointer list] {
        cffi::pointer invalidate $ptr
    }
}
proc cffi::test::make_counted_pointers args {
    lmap addr $args {
        set p [makeptr $addr]
        cffi::pointer counted $p
        set p
    }
}
proc cffi::test::make_safe_pointers args {
    lmap addr $args {
        set p [makeptr $addr]
        cffi::pointer safe $p
        set p
    }
}
proc cffi::test::make_unsafe_pointers args {
    lmap addr $args {
        makeptr $addr
    }
}
proc cffi::test::pointer_unsafe_add {p incr args} {
    set addr [pointer address $p]
    return [makeptr [incr addr $incr] {*}$args]
}
# Resets all enums
proc cffi::test::reset_enums {} {
    cffi::enum clear
}
# Resets all aliases
proc cffi::test::reset_aliases {} {
    cffi::alias clear
}
