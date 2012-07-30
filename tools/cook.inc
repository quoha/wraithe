/*======================================================================*\
 * wraithe/tools/cook.inc
 *
 * Copyright (c) 2012 Michael D Henderson
 *
 * This file is part of wraithe.
 *
\*======================================================================*/

set tell-position;

cktProjDir  = [cktRoot][cktProject]/;
cktVer      = [cktVerMajor].[cktVerMinor].[cktVerPatch];
cktDir      = [cktProject]-[cktVer];
cktCookVer  = 00.00.0000;

function print [cktName] - [cktDir] "(built by cooktools v"[cktCookVer]")";

/*===========================================================================
 * compiler setup
 *  ccInclude -- include paths to pass to the c_incl utility and cc program
 */
if [not [defined all_libs ]] then all_libs       = ;
if [not [defined extCFlags]] then extCFlags      = ;

if [not [defined cc       ]] then cc             = [find_command gcc];
if [not         [cc]       ] then cc             = gcc;
if [not [defined ccInclude]] then ccInclude      = ;
if [not [defined ccLibs   ]] then ccLibs         = ;
if [not [defined ccFlags  ]] then ccFlags        = ;

/*===========================================================================
 * utility paths
 */
if [not [defined ar     ]] then ar      = [find_command ar];
if [not         [ar]     ] then ar      = tools/ar;
if [not [defined delete ]] then delete  = tools/delete;
if [not [defined strip  ]] then strip   = [find_command strip];
if [not         [strip]  ] then strip   = tools/strip;
if [not [defined testsh ]] then testsh  = tools/testsh;

/*===========================================================================
 * Package Identification Strings
 */
cktUser = [getenv LOGNAME];
if [not [cktUser]] then cktUser = [getenv USER];
if [not [cktUser]] then cktUser = [basename [getenv HOME]];
if [not [cktUser]] then cktUser = "unknown";

cktIdentFlags = [catenate -DPKG_VER\=  '\'"' [cktVer]  '"\'']
                [catenate -DPKG_USER\= '\'"' [cktUser] '"\'']
                [catenate -DPKG_IDENT\= "'" '"@(#)' [cktProject]-[cktVer] ' " ' '__DATE__ " " __TIME__ " " __FILE__' "'" ]
                [catenate -DSRC_IDENT\= "'" '"@(#)"__DATE__" "__TIME__" "__FILE__' "'"]
                ;
ccFlags += [cktIdentFlags];

/*===========================================================================
 * support functions
 */

/*===========================================================================
 * allow cook to generate the list of sources files that we have
 */
cktAllPackages = ;
cktAllFiles    = [collect tools/manifest all  ];
cktCookedInc   = ;
cktSrcPaths    = [collect tools/manifest dir  ];
cktSrcFiles    = [collect tools/manifest file ];

/*===========================================================================
 * generate list of packages from the source. this is for both binaries and
 * libraries.
 */
cktBinPackages = [sort [match_mask src/bin/% [cktSrcPaths]]];
cktLibPackages = [sort [match_mask src/lib/% [cktSrcPaths]]];

/*===========================================================================
 * build the dependencies for binary, library and shell packages
 */
loop local shlFile = [match_mask %0%.sh [cktSrcFiles]] {
  local tgtName = [fromto src/%1/%2 %2 [shlFile]];

  ourTarget        = bin/scripts;
  [ourTarget]_type = shl;

  if [defined [ourTarget]_dep] then {
    [ourTarget]_dep += [shlFile];
  } else {
    [ourTarget]_dep  = [shlFile];
  }

  cktAllPackages += [ourTarget];

/*
  function print " info: ourTarget         is" [ourTarget];
  function print " info: ourTarget_target  is" [ourTarget]_target;
  function print " info: ourTarget_type    is" [[ourTarget]_type];
  function print " info: ourTarget_dep     is" [[ourTarget]_dep];
*/
}

loop local pkgPath = [cktBinPackages] [cktLibPackages] {
  local tgtType  = [fromto src/%1/%2 %1 [pkgPath]];
  local tgtName  = [fromto src/%1/%2 %2 [pkgPath]];

  if [match_mask src/bin/% [pkgPath]] then {
    ourTarget = bin/[tgtName];
    [ourTarget]_type = exe;
  } else if [match_mask src/lib/% [pkgPath]] then {
    ourTarget = lib/lib[tgtName].a;
    [ourTarget]_type = lib;
  } else if [match_mask src/shl/% [pkgPath]] then {
    ourTarget = bin/[tgtName];
    [ourTarget]_type = shl;
  } else {
    echo do not know how to handle [pkgPath];
    loopstop;
  }

  [ourTarget]_target = [ourTarget];

  [ourTarget]_sh   = ;
  [ourTarget]_fbf  = [match_mask [pkgPath]/%0%.fbf [cktSrcFiles]];
  [ourTarget]_y    = [match_mask [pkgPath]/%0%.y   [cktSrcFiles]];
  [ourTarget]_hdr  = [match_mask [pkgPath]/%0%.h   [cktSrcFiles]]
                     [match_mask [pkgPath]/%0%.hpp [cktSrcFiles]];
  [ourTarget]_src  = [stringset [match_mask [pkgPath]/%0%.c   [cktSrcFiles]]
                                [match_mask [pkgPath]/%0%.cpp [cktSrcFiles]]
                                [fromto %0%.fbf  %0%.c [[ourTarget]_fbf   ]]
                                [fromto %0%.y    %0%.c [[ourTarget]_y     ]]
                     ];
  [ourTarget]_obj  = [fromto %0%.%1 %0%.o [[ourTarget]_src]];

  if [defined [ourTarget]_dep] then {
    [ourTarget]_dep += [[ourTarget]_obj];
  } else {
    [ourTarget]_dep  = [[ourTarget]_obj];
  }

  if [defined cktCookedInc] then {
    cktCookedInc += [fromto %0%.%1  %0%.%1_dep  [[ourTarget]_src]
                                                [[ourTarget]_hdr]];
  } else {
    cktCookedInc  = [fromto %0%.%1  %0%.%1_dep  [[ourTarget]_src]
                                                [[ourTarget]_hdr]];
  }

  if [not [defined [ourTarget]_inc    ]] then [ourTarget]_inc     = ;
  if [not [defined [ourTarget]_ldflags]] then [ourTarget]_ldflags = ;

  cktAllPackages += [ourTarget];

/*
  function print " info: pkgPath           is" [pkgPath];
  function print " info: tgtName           is" [tgtName];
  function print " info: tgtType           is" [tgtType];
  function print " info: ourTarget         is" [ourTarget];
  function print " info: ourTarget_target  is" [ourTarget]_target;
  function print " info: ourTarget_target  is" [[ourTarget]_target];
  function print " info: ourTarget_src     is" [[ourTarget]_src];
  function print " info: ourTarget_dep     is" [[ourTarget]_dep];
  function print " info: ourTarget_hdr     is" [[ourTarget]_hdr];
  function print " info: ourTarget_sh      is" [[ourTarget]_sh ];
  function print " info: ourTarget_y       is" [[ourTarget]_y  ];
  function print " info: ourTarget_obj     is" [[ourTarget]_obj];
  function print " info: ourTarget_inc     is" [[ourTarget]_inc];
  function print " info: ourTarget_ldflags is" [[ourTarget]_ldflags];
  function print " info: ourTarget_type    is" [[ourTarget]_type];
*/
}

/*===========================================================================
 */
clean.bak: {
  [delete] [match_mask %0%~ [cktAllFiles]] set silent;
}
clean.bin: {
  [delete] [match_mask %0core [cktAllFiles]] [cktAllPackages] set silent;
}
clean.dep: {
  [delete] [cktCookedInc] set silent;
}
clean.obj: {
  [delete] [match_mask %0%.o [cktSrcFiles]] set silent;
}
clean.scripts: {
  if [defined bin/scripts_type] then {
    [delete] [fromto %0%.sh %0% [fromto src/%1/%2 bin/%2 [bin/scripts_dep]]] set silent;
  }
}
clean: clean.bak clean.obj {
  [delete] Howto.list set silent;
}
clobber: clean clean.bak clean.bin clean.dep clean.obj clean.scripts;

distrib: clobber {
  [distrib] [cktProject] [cktVerMajor].[cktVerMinor].[cktVerPatch];
}

/*===========================================================================
 * production rules
 */
package.arg:
{
  local pkgBase = /tmp/[cktProject]-[cktVerMajor].[cktVerMinor].[cktVerPatch]/;
  local pkgRoot = [pkgBase]pkg/;
  local pkgDir  = [pkgRoot]scripts/;

  rm -rf [pkgDir];
  mkdir -p [pkgDir]parm;
  chmod 775 [pkgRoot] [pkgDir]parm;  /* must protect the parm dir */

  cp src/parm/\*.arg [pkgDir]parm;

  cd [pkgRoot] && tar zcvf [pkgBase][cktProject]-[cktVerMajor].[cktVerMinor].[cktVerPatch]-arg.tgz scripts;

  rm -rf [pkgRoot];
}

package.bin: all
{
  local pkgBase = /tmp/[cktProject]-[cktVerMajor].[cktVerMinor].[cktVerPatch]/;
  local pkgRoot = [pkgBase]pkg/;
  local pkgDir  = [pkgRoot]scripts/;

  rm -rf [pkgDir];
  mkdir -p [pkgDir]bin;
  chmod 770 [pkgRoot];

  function print " --->: install " [pkgDir];

  loop local pkgPath = [sort [cktBinPackages] [cktLibPackages]] {
    local tgtType  = [fromto src/%1/%2 %1 [pkgPath]];
    local tgtName  = [fromto src/%1/%2 %2 [pkgPath]];

    function print " info: install" [pkgPath];

    if [match_mask src/bin/% [pkgPath]] then {
      ourTarget = bin/[tgtName];

      cp -p [[ourTarget]_target] [pkgDir]bin/;
    }
  }

  cd [pkgRoot] && tar zcvf [pkgBase][cktProject]-[cktVerMajor].[cktVerMinor].[cktVerPatch]-bin.tgz scripts;

  rm -rf [pkgRoot];
}

package.cfg:
{
  local pkgBase = /tmp/[cktProject]-[cktVerMajor].[cktVerMinor].[cktVerPatch]/;
  local pkgRoot = [pkgBase]pkg/;
  local pkgDir  = [pkgRoot]scripts/;

  rm -rf [pkgDir];
  mkdir -p [pkgDir]config;
  chmod 770 [pkgRoot] [pkgDir]config;  /* must protect the config dir */

  cp src/cfg/\*.cfg [pkgDir]config;

  cd [pkgRoot] && tar zcvf [pkgBase][cktProject]-[cktVerMajor].[cktVerMinor].[cktVerPatch]-cfg.tgz scripts;

  rm -rf [pkgRoot];
}

package.lib: all
{
  local pkgBase = /tmp/[cktProject]-[cktVerMajor].[cktVerMinor].[cktVerPatch]/;
  local pkgRoot = [pkgBase]pkg/;
  local pkgDir  = [pkgRoot]scripts/local/;

  rm -rf [pkgDir];
  mkdir -p [pkgDir]bin [pkgDir]include [pkgDir]lib [pkgDir]man [pkgDir]share;
  chmod 770 [pkgRoot];

  function print " --->: install " [pkgDir];

  loop local pkgPath = [sort [cktLibPackages]] {
    local tgtType  = [fromto src/%1/%2 %1 [pkgPath]];
    local tgtName  = [fromto src/%1/%2 %2 [pkgPath]];

    function print " info: install" [pkgPath];

    if [match_mask src/lib/% [pkgPath]] then {
      ourTarget = lib/lib[tgtName].a;

      cp -p [[ourTarget]_target] [pkgDir]lib/;

      loop local srcFile = [sort [[ourTarget]_hdr]] {
        if [match_mask %0%/local.h [srcFile]] then {
          /* ignore local files */
        } else {
          cp -p [srcFile] [pkgDir]include/;
        }
      }
    }
  }

  cd [pkgRoot] && tar zcvf [pkgBase][cktProject]-[cktVerMajor].[cktVerMinor].[cktVerPatch]-lib.tgz scripts/local;

  rm -rf [pkgRoot];
}

package.shl: bin/scripts
{
  local pkgBase = /tmp/[cktProject]-[cktVerMajor].[cktVerMinor].[cktVerPatch]/;
  local pkgRoot = [pkgBase]pkg/;
  local pkgDir  = [pkgRoot]scripts/;

  rm -rf [pkgDir];
  mkdir -p [pkgDir]bin;
  chmod 770 [pkgRoot];

  function print " --->: install " [pkgDir];

  if [defined bin/scripts_dep] then {
    cp -p [fromto %0%.sh %0% [fromto src/%1/%2 bin/%2 [bin/scripts_dep]]] [pkgDir]bin/;
  }

  cd [pkgRoot] && tar zcvf [pkgBase][cktProject]-[cktVerMajor].[cktVerMinor].[cktVerPatch]-shl.tgz scripts;

  rm -rf [pkgRoot];
}

package.src: clobber
{
  local pkgBase = /tmp/[cktProject]-[cktVerMajor].[cktVerMinor].[cktVerPatch]/;
  local pkgRoot = [pkgBase]pkg/;
  local pkgDir  = [pkgRoot][cktProject]-[cktVerMajor].[cktVerMinor].[cktVerPatch]/;

  rm -rf [pkgDir];
  mkdir -p [pkgDir];
  chmod 770 [pkgRoot];

  tar cf - --exclude .svn * | (cd [pkgDir] && tar xf -);

  cd [pkgRoot] && tar zcvf [pkgBase][cktProject]-[cktVerMajor].[cktVerMinor].[cktVerPatch]-src.tgz [cktProject]-[cktVerMajor].[cktVerMinor].[cktVerPatch];

  rm -rf [pkgRoot];
}


/*===========================================================================
 * from the user guide, create dependency list files for files
 */
%0%.%1_dep: %0%.%1
  set no-cascade
{
  c_incl --no-cache --no-recurs %0%.%1
    -I[dirname %0%.%1] [ccInclude]
    "--prefix='cascade %0%.%1 = '"
    "--suffix=';'"
    -o [target]
      set silent;
}

/*===========================================================================
 * rule to translate an FBF specification
 */
%0%.c %0%.h: %0%.fbf: tools/bin/fbf2c
{
  [fbf2c] %0%.fbf %0%.c %0%.h set silent;
}

/*===========================================================================
 * rule to translate a lemon specification
 *
 * note that by not including the .out in the specification, we're able to
 * leave it on error. that will help when debugging problems with the
 * grammar.
 */
%0%.c %0%.hpp: %0%.y: [lemon]
{
  [lemon] -c %0%.y set silent;
  [lemonx]   %0%.y set silent;
}

/*===========================================================================
 * force a C source file to compile in the current directory with the right
 * include files -- assumes that dependency files have been created
 */
%0%.o: %0%.c
{
  [cc] -I[dirname %0%.c] [ccInclude] [ccFlags] -c %0%.c -o [target];
}

/*===========================================================================
 * force a C++ source file to compile in the current directory with the right
 * include files -- assumes that dependency files have been created
 */
%0%.o: %0%.cpp
{
  [cc] -I[dirname %0%.cpp] [ccInclude] [ccFlags] -c %0%.cpp -o [target];
}

/*===========================================================================
 * create a library from object files
 *   example: lib/libsbll.a: lib/sbll_obj
 */
lib/lib%1.a: [[target]_obj]
  set unlink
{
  [ar] cq [target] [[target]_obj];
}

/*===========================================================================
 */
bin/scripts: [[target]_dep]
{
  loop local srcFile = [sort [[target]_dep]] {
    function print "script: install" [srcFile];
    local tgtFile = [fromto %0%.sh %0% [fromto src/%1/%2 bin/%2 [srcFile]]];
    function print "script: install" [tgtFile];
    tools/bin/cpScript [cktVer] [srcFile] [tgtFile];
  }
}

/*===========================================================================
 * create an executable from object files and special libraries
 *   example: bin/sbll: bin/sbll_dep
 */
bin/%: [[target]_dep]
  set unlink
{
  /*echo target [target] [target]_dep [[target]_dep];*/

  [cc] -o [target]
    [[target]_obj]
    [[target]_ldflags]
    [all_libs]
    ;
}

/*===========================================================================
 * include dependency files created above
 */
#include-cooked-nowarn [cktCookedInc]
