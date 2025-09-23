/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "Lumasonic Decoder SDK", "index.html", [
    [ "Getting Started", "index.html", "index" ],
    [ "Decoder API", "md__r_e_a_d_m_e.html", [
      [ "Library Structure", "md__r_e_a_d_m_e.html#autotoc_md96", [
        [ "Header Include Files", "md__r_e_a_d_m_e.html#autotoc_md98", null ],
        [ "Static Library Files", "md__r_e_a_d_m_e.html#autotoc_md99", null ]
      ] ],
      [ "Building", "md__r_e_a_d_m_e.html#autotoc_md100", [
        [ "Includes", "md__r_e_a_d_m_e.html#autotoc_md101", null ],
        [ "Building on Linux", "md__r_e_a_d_m_e.html#autotoc_md102", null ]
      ] ],
      [ "Using the Decoder", "md__r_e_a_d_m_e.html#autotoc_md103", [
        [ "Creating a Decoder", "md__r_e_a_d_m_e.html#autotoc_md104", null ],
        [ "Initializing the Decoder", "md__r_e_a_d_m_e.html#autotoc_md105", null ],
        [ "Decoding Audio", "md__r_e_a_d_m_e.html#autotoc_md106", null ],
        [ "Reading Decoded Values", "md__r_e_a_d_m_e.html#autotoc_md108", null ],
        [ "Clearing Decoded Values", "md__r_e_a_d_m_e.html#autotoc_md109", null ],
        [ "Deleting a Decoder", "md__r_e_a_d_m_e.html#autotoc_md110", null ],
        [ "Configuring a Log File", "md__r_e_a_d_m_e.html#autotoc_md111", null ],
        [ "Safely Releasing a Log File", "md__r_e_a_d_m_e.html#autotoc_md112", null ],
        [ "Adjusting Output Brightness", "md__r_e_a_d_m_e.html#autotoc_md114", null ],
        [ "Detecting AudioStrobe", "md__r_e_a_d_m_e.html#autotoc_md116", null ]
      ] ],
      [ "Using the Reader", "md__r_e_a_d_m_e.html#autotoc_md117", [
        [ "Creating a Reader", "md__r_e_a_d_m_e.html#autotoc_md118", null ],
        [ "Reader Thread Modes", "md__r_e_a_d_m_e.html#autotoc_md119", null ],
        [ "Connecting a Reader to a Decoder", "md__r_e_a_d_m_e.html#autotoc_md121", null ],
        [ "Reader Listeners", "md__r_e_a_d_m_e.html#autotoc_md122", [
          [ "Implementing the Listener Interface", "md__r_e_a_d_m_e.html#autotoc_md123", null ],
          [ "Using Listeners with Readers", "md__r_e_a_d_m_e.html#autotoc_md124", null ]
        ] ],
        [ "Deleting a Reader", "md__r_e_a_d_m_e.html#autotoc_md125", null ],
        [ "Monitoring Reader Performance", "md__r_e_a_d_m_e.html#autotoc_md126", [
          [ "Resetting a Monitoring Session", "md__r_e_a_d_m_e.html#autotoc_md127", null ]
        ] ]
      ] ],
      [ "Using the C API", "md__r_e_a_d_m_e.html#autotoc_md128", [
        [ "The Lumasonic Codec", "md__r_e_a_d_m_e.html#autotoc_md129", null ],
        [ "Linking", "md__r_e_a_d_m_e.html#autotoc_md130", [
          [ "Linking on Linux", "md__r_e_a_d_m_e.html#autotoc_md131", null ]
        ] ],
        [ "Initializing the Codec", "md__r_e_a_d_m_e.html#autotoc_md132", [
          [ "Decoder and Reader IDs", "md__r_e_a_d_m_e.html#autotoc_md133", null ],
          [ "Initializing Decoders", "md__r_e_a_d_m_e.html#autotoc_md134", null ]
        ] ],
        [ "Processing Audio", "md__r_e_a_d_m_e.html#autotoc_md135", null ],
        [ "Reading Decoded Audio", "md__r_e_a_d_m_e.html#autotoc_md136", null ],
        [ "Creating Additional Decoders and Readers", "md__r_e_a_d_m_e.html#autotoc_md137", [
          [ "Connecting Created Instances", "md__r_e_a_d_m_e.html#autotoc_md138", null ]
        ] ],
        [ "Shutting Down the Codec", "md__r_e_a_d_m_e.html#autotoc_md139", null ]
      ] ]
    ] ],
    [ "Lumasonic File Player", "md__lumasonic_file_player.html", [
      [ "Audio Device Settings", "md__lumasonic_file_player.html#autotoc_md141", null ],
      [ "Read Thread Settings", "md__lumasonic_file_player.html#autotoc_md142", null ]
    ] ],
    [ "Example Projects", "md__examples.html", [
      [ "Dependencies", "md__examples.html#autotoc_md144", null ],
      [ "Building the Examples", "md__examples.html#autotoc_md145", [
        [ "Windows", "md__examples.html#autotoc_md146", null ],
        [ "macOS", "md__examples.html#autotoc_md147", null ],
        [ "Linux", "md__examples.html#autotoc_md148", null ]
      ] ],
      [ "Examples Summary", "md__examples.html#autotoc_md149", [
        [ "examples/decoder", "md__examples.html#autotoc_md151", null ],
        [ "examples/reader", "md__examples.html#autotoc_md153", null ],
        [ "examples/player", "md__examples.html#autotoc_md155", null ]
      ] ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"_lumasonic_codec_8h.html",
"class_lumasonic_file_player.html#aeacbd8ab461602684ac393588a4ff5aa"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';