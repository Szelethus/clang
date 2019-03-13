//===--- Options.td - Options for clang -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//  This file defines the options accepted by clang.
//
//===----------------------------------------------------------------------===//

// Include the common option parsing interfaces.
include "llvm/Option/OptParser.td"

//===----------------------------------------------------------------------===//
// Flags.
//===----------------------------------------------------------------------===//

// DriverOption - The option is a "driver" option, and should not be forwarded
// to other tools.
def DriverOption : OptionFlag;

// LinkerInput - The option is a linker input.
def LinkerInput : OptionFlag;

// NoArgumentUnused - Don't report argument unused warnings for this option; this
// is useful for options like -static or -dynamic which a user may always end up
// passing, even if the platform defaults to (or only supports) that option.
def NoArgumentUnused : OptionFlag;

// Unsupported - The option is unsupported, and the driver will reject command
// lines that use it.
def Unsupported : OptionFlag;

// Ignored - The option is unsupported, and the driver will silently ignore it.
def Ignored : OptionFlag;

// CoreOption - This is considered a "core" Clang option, available in both
// clang and clang-cl modes.
def CoreOption : OptionFlag;

// CLOption - This is a cl.exe compatibility option. Options with this flag
// are made available when the driver is running in CL compatibility mode.
def CLOption : OptionFlag;

// CC1Option - This option should be accepted by clang -cc1.
def CC1Option : OptionFlag;

// CC1AsOption - This option should be accepted by clang -cc1as.
def CC1AsOption : OptionFlag;

// NoDriverOption - This option should not be accepted by the driver.
def NoDriverOption : OptionFlag;

// A short name to show in documentation. The name will be interpreted as rST.
class DocName<string name> { string DocName = name; }

// A brief description to show in documentation, interpreted as rST.
class DocBrief<code descr> { code DocBrief = descr; }

// Indicates that this group should be flattened into its parent when generating
// documentation.
class DocFlatten { bit DocFlatten = 1; }

// Indicates that this warning is ignored, but accepted with a warning for
// GCC compatibility.
class IgnoredGCCCompat : Flags<[HelpHidden]> {}

//===----------------------------------------------------------------------===//
// Groups.
//===----------------------------------------------------------------------===//

def Action_Group : OptionGroup<"<action group>">, DocName<"Actions">,
                   DocBrief<[{The action to perform on the input.}]>;


