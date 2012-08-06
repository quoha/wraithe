/*************************************************************************
 * tools/cook.inc
 *
 * Copyright (c) 2012 Michael D Henderson, all rights reserved
 *
 * This file is part of wraithe (http://github.com/quoha/wraithe).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *************************************************************************/

set tell-position;

cktProjDir  = [cktRoot][cktProject]/;
cktVer      = [cktVerMajor].[cktVerMinor].[cktVerPatch];
cktDir      = [cktProject]-[cktVer];
cktCookVer  = 00.000.0000;

function print [cktName] - [cktDir] "(built by cktCook v"[cktCookVer]")";

/*===========================================================================
 * compiler setup
 *  ccInclude -- include paths to pass to the c_incl utility and cc program
 */
if [not [defined all_libs ]] then all_libs       = ;
if [not [defined extCFlags]] then extCFlags      = ;
if [not [defined cc       ]] then cc             = [find_command clang];
if [not [defined cc       ]] then cc             = [find_command gcc];
if [not [defined cc       ]] then cc             = gcc;
if [not [defined ccInclude]] then ccInclude      = ;
if [not [defined ccLibs   ]] then ccLibs         = ;
if [not [defined ccFlags  ]] then ccFlags        = ;

/*===========================================================================
 * linker setup
 */
if [defined cktIsMac] then {
  cktSharedLibExt=".dylib";
} else {
  cktSharedLibExt=".so";
}

/*===========================================================================
 * utility paths
 */
if [not [defined ar      ]] then ar       = [find_command ar];
if [not [defined cpshl   ]] then cpshl    = tools/cpScript;
if [not [defined delete  ]] then delete   = tools/delete;
if [not [defined distrib ]] then distrib  = tools/distrib;
if [not [defined lemon   ]] then lemon    = [find_command lemon];
if [not [defined manifest]] then manifest = tools/manifest;
if [not [defined ragel   ]] then ragel    = [find_command ragel];
if [not [defined strip   ]] then strip    = [find_command strip];
if [not [defined testsh  ]] then testsh   = tools/testsh;
if [not [defined checksh ]] then checksh  = bin/check.sh;

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
cktTestUnits   = ;
cktCookedInc   = ;
cktSrcPaths    = [sort [collect [manifest] dir  ]];
cktSrcFiles    = [sort [collect [manifest] file ]];

/*===========================================================================
 * generate list of packages from the source. this is for both binaries and
 * libraries. we assume that cktSrcPaths is sorted.
 */
cktBinPackages = [match_mask src/bin/%    [cktSrcPaths]]
                 [match_mask src/bin-32/% [cktSrcPaths]];
cktDLLPackages = [match_mask src/dll/%    [cktSrcPaths]]
                 [match_mask src/dll-32/% [cktSrcPaths]];
cktLibPackages = [match_mask src/lib/%    [cktSrcPaths]]
                 [match_mask src/lib-32/% [cktSrcPaths]];
cktTstPackages = [match_mask src/tst/%    [cktSrcPaths]]
                 [match_mask src/tst-32/% [cktSrcPaths]];

/*===========================================================================
 * build the dependencies for lua packages
 */
loop local luaFile = [match_mask %0%.lua [cktSrcFiles]] {
  local tgtName = [fromto src/%1/%2 %2 [luaFile]];

  ourTarget        = lib/[tgtName];
  [ourTarget]_type = lua;

  if [defined [ourTarget]_dep] then {
    [ourTarget]_dep += [luaFile];
  } else {
    [ourTarget]_dep  = [luaFile];
  }

  cktAllPackages += [ourTarget];
}

/*===========================================================================
 * build the dependencies for shell packages
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


/*===========================================================================
 * build the dependencies for SQL scripts
 */
loop local sqlFile = [match_mask %0%.sql [cktSrcFiles]] {
  local tgtName = [fromto src/%1/%2 %2 [sqlFile]];

  ourTarget        = sql/[tgtName];
  [ourTarget]_type = sql;

  if [defined [ourTarget]_dep] then {
    [ourTarget]_dep += [sqlFile];
  } else {
    [ourTarget]_dep  = [sqlFile];
  }

  cktAllPackages += [ourTarget];
}

/*===========================================================================
 * build the dependencies for binary and library packages.
 *
 * if we find -32 at then end of the path, then assume that we will build a
 * 32-bit package.
 */
loop local pkgPath = [cktBinPackages] [cktDLLPackages] [cktLibPackages] [cktTstPackages] {
  local tgtType   = [fromto src/%1/%2 %1 [pkgPath]];
  local tgtName   = [fromto src/%1/%2 %2 [pkgPath]];

  local tName    = ;
  local tType    = ;
  local tCCFlags = ;
  local tLDFlags = ;

  if [matches "bin" [tgtType]] then {
    tName = bin/[tgtName];
    tType = exe;
  } else if [matches "bin-32" [tgtType]] then {
    tName    = bin/[tgtName];
    tType    = exe32;
    tCCFlags = -m32;
    tLdFlags = -m32;
  } else if [matches "lib" [tgtType]] then {
    tName = lib/lib[tgtName].a;
    tType = lib;
    ourDebugTarget        = lib/lib[tgtName]_debug.a;
    [ourDebugTarget]_type = [tName]_type;
  } else if [matches "lib-32" [tgtType]] then {
    tName    = lib/lib[tgtName].a;
    tType    = lib32;
    tCCFlags = -m32;
    tLDFlags = -m32;
    ourDebugTarget        = lib/lib[tgtName]_debug.a;
    [ourDebugTarget]_type = [tName]_type;
  } else if [matches "dll" [tgtType]] then {
    loop local w = [split "." [tgtName]] {
      if [not [tName]] then {
        tName = [w];
      } else {
        tName = [catenate [tName] "/" [w]];
      }
    }
    tName    = [prepost "lib/" [cktSharedLibExt] [tName]];
    tType    = dll;
    tCCFlags = -fPIC -fno-common;
    tLDFlags = -fPIC -fno-common;
  } else if [matches "dll-32" [tgtType]] then {
    loop local w = [split "." [tgtName]] {
      if [not [tName]] then {
        tName = [w];
      } else {
        tName = [catenate [tName] "/" [w]];
      }
    }
    tName    = [prepost "lib/" [cktSharedLibExt] [tName]];
    tType    = dll32;
    tCCFlags = -m32 -fPIC;
    tLDFlags = -m32 -fPIC;
  } else if [matches "tst" [tgtType]] then {
    tName = tests/bin/[tgtName];
    tType = exe;
    cktTestUnits += [tName];
  } else if [matches "tst-32" [tgtType]] then {
    tName = tests/bin-32/[tgtName];
    tType = exe32;
    cktTestUnits += [tName];
  } else {
    echo do not know how to handle [pkgPath];
    loopstop;
  }

  [tName]_type = [tType];

  if [defined [tName]_ccFlags] then {
    [tName]_ccFlags += [tCCFlags];
  } else {
    [tName]_ccFlags  = ;
  }
  if [defined [tName]_ldFlags] then {
    [tName]_ldFlags += [tLDFlags];
  } else {
    [tName]_ldFlags  = ;
  }

  if [matches "dll" [tgtType]] then {
    [tName]_ccFlags += "-fPIC -fno-common";
    /*[tName]_ldFlags += "-shared -Wl,-soname,core.so";*/
  } else if [matches "dll32" [tgtType]] then {
    [tName]_ccFlags += "-fPIC";
  }

  local ourFiles = [match_mask [pkgPath]/%0% [cktSrcFiles]];
  if [defined [tName]_exclude] then {
    loop local srcFilter = [[tName]_exclude] {
      ourFiles = [filter_out %0[srcFilter] [ourFiles]];
    }
  }

  [tName]_target = [tName];
  [tName]_sh     = ;
  [tName]_gg     = [match_mask [pkgPath]/%0%.gg  [ourFiles]];
  [tName]_hdr    = [match_mask [pkgPath]/%0%.h   [ourFiles]]
                   [match_mask [pkgPath]/%0%.hpp [ourFiles]];
  [tName]_rl     = [match_mask [pkgPath]/%0%.rl  [ourFiles]];
  [tName]_src    = [stringset [match_mask [pkgPath]/%0%.c   [ourFiles]]
                              [match_mask [pkgPath]/%0%.cpp [ourFiles]]
                              [fromto %0%.gg %0%.c [[tName]_gg]]
                              [fromto %0%.rl %0%.c [[tName]_rl]]];
  [tName]_obj    = [fromto %0%.%1 %0%.o [[tName]_src]];

  if [not [defined [tName]_dep    ]] then [tName]_dep     = ;
  if [not [defined [tName]_inc    ]] then [tName]_inc     = ;

  [tName]_dep += [[tName]_obj];

  loop local objFile = [[tName]_obj] {
    [objFile]_inc  = [[tName]_inc];
    [objFile]_type = [[tName]_type];
    if [not [defined [objFile]_ccFlags]] then {
      [objFile]_ccFlags = ;
    }
    [objFile]_ccFlags += [[tName]_ccFlags];
  }

  loop local srcFile = [[tName]_src] {
    [srcFile]_dep_inc = [[tName]_inc];
    [srcFile]_type    = [[tName]_type];
  }

  if [defined cktCookedInc] then {
    cktCookedInc += [fromto %0%.%1  %0%.%1_dep  [[tName]_src]
                                                [[tName]_hdr]];
  } else {
    cktCookedInc  = [fromto %0%.%1  %0%.%1_dep  [[tName]_src]
                                                [[tName]_hdr]];
  }

  cktAllPackages += [tName];

  if [defined ourDebugTarget] then {
    if [not [defined [ourDebugTarget]_dep    ]] then [ourDebugTarget]_dep     = ;
    if [not [defined [ourDebugTarget]_inc    ]] then [ourDebugTarget]_inc     = ;
    if [not [defined [ourDebugTarget]_ldFlags]] then [ourDebugTarget]_ldFlags = ;
    [ourDebugTarget]_target = [ourDebugTarget];
    [ourDebugTarget]_sh     = ;
    [ourDebugTarget]_gg     = [match_mask [pkgPath]/%0%.gg  [cktSrcFiles]];
    [ourDebugTarget]_hdr    = [match_mask [pkgPath]/%0%.h   [cktSrcFiles]]
                              [match_mask [pkgPath]/%0%.hpp [cktSrcFiles]];
    [ourDebugTarget]_rl     = [match_mask [pkgPath]/%0%.rl  [cktSrcFiles]];
    [ourDebugTarget]_src    = [stringset [match_mask [pkgPath]/%0%.c   [cktSrcFiles]]
                                         [match_mask [pkgPath]/%0%.cpp [cktSrcFiles]]
                                         [fromto %0%.gg %0%.c [[ourDebugTarget]_gg]]
                                         [fromto %0%.rl %0%.c [[ourDebugTarget]_rl]]];
    [ourDebugTarget]_obj    = [fromto %0%.%1 %0%_debug.o [[ourDebugTarget]_src]];

    [ourDebugTarget]_dep += [[ourDebugTarget]_obj];

    cktAllPackages += [ourDebugTarget];
  }

  if [matches "ignore" [[tName]_type]] then {
    function print " info: pkgPath           is" [pkgPath];
    function print " info: tgtName           is" [tgtName];
    function print " info: tgtType           is" [tgtType];
    function print " info: ourTarget         is" [tName];
    function print " info: ourTarget_target  is" [tName]_target;
    function print " info: ourTarget_target  is" [[tName]_target];
    function print " info: ourTarget_src     is" [[tName]_src];
    function print " info: ourTarget_dep     is" [[tName]_dep];
    function print " info: ourTarget_hdr     is" [[tName]_hdr];
    function print " info: ourTarget_sh      is" [[tName]_sh ];
    function print " info: ourTarget_gg      is" [[tName]_gg ];
    function print " info: ourTarget_rl      is" [[tName]_rl ];
    function print " info: ourTarget_obj     is" [[tName]_obj];
    function print " info: ourTarget_inc     is" [[tName]_inc];
    function print " info: ourTarget_ccFlags is" [[tName]_ccFlags];
    function print " info: ourTarget_ldFlags is" [[tName]_ldFlags];
    function print " info: ourTarget_type    is" [[tName]_type];
  }
}

/*===========================================================================
 */
clean.bak: {
  [delete] [match_mask %0%~ [cktSrcFiles]] set silent;
}
clean.bin: {
  [delete] [match_mask %0core [cktSrcFiles]] [cktAllPackages] set silent;
}
clean.dep: {
  [delete] [cktCookedInc] set silent;
}
clean.lua: {
  echo delete [fromto src/%1/%2 lib/%2 [match_mask %0%.lua [cktSrcFiles]]];
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

rebuild: clobber all;

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
  local addlFlags =;
  if [defined [target]_inc] then addlFlags = [[target]_inc];

  c_incl --no-cache --no-recurs %0%.%1
    -I[dirname %0%.%1] [ccInclude] [addlFlags]
    "--prefix='cascade %0%.%1 = '"
    "--suffix=';'"
    -o [target]
      set silent;
}

/*===========================================================================
 * rule to translate a lemon specification
 *
 * note that by not including the .out in the specification, we're able to
 * leave it on error. that will help when debugging problems with the
 * grammar.
 */
%0%.c %0%.h: %0%.gg: [lemon]
{
  [lemon] -c %0%.gg;
  /*[lemonx]   %0%.gg; */
}

vendor/bin/lemon: vendor/lemon/lemon.c
{
  gcc -o vendor/bin/lemon vendor/lemon/lemon.c;
}

/*===========================================================================
 * rule to translate a ragel specification (a .rl file to .c)
 *
 */
%0%.c: %0%.rl: [ragel]
{
  [ragel] -C -G2 -o %0%.c   %0%.rl;
  [ragel] -p -V  -o %0%.dot %0%.rl;
}

/*===========================================================================
 * force a C source file to compile in the current directory with the right
 * include files -- assumes that dependency files have been created
 */
%0%_debug.o: %0%.c
{
  local addlFlags =;
  if [defined [target]_inc] then addlFlags += [[target]_inc];
  if [defined [target]_ccFlags] then addlFlags += [[target]_ccFlags];

  [cc] [addlFlags] -I[dirname %0%.c] [ccInclude] [ccFlags] -c %0%.c -o [target];
}
%0%.o: %0%.c
{
  local addlFlags =;
  if [defined [target]_inc] then addlFlags += [[target]_inc];
  if [defined [target]_ccFlags] then addlFlags += [[target]_ccFlags];

  [cc] [addlFlags] -I[dirname %0%.c] [ccInclude] [ccFlags] -c %0%.c -o [target];
}

/*===========================================================================
 * force a C++ source file to compile in the current directory with the right
 * include files -- assumes that dependency files have been created
 */
%0%.o: %0%.cpp
{
  local addlFlags =;
  if [defined [target]_inc] then addlFlags += [[target]_inc];
  if [defined [target]_ccFlags] then addlFlags += [[target]_ccFlags];

  [cc] [addlFlags] -I[dirname %0%.cpp] [ccInclude] [ccFlags] -c %0%.cpp -o [target];
}

/*===========================================================================
 * run file through the pre-processor
 */
%0%.ppp: %0%.cpp
{
  local addlFlags =;
  if [defined [target]_inc] then addlFlags += [[target]_inc];
  if [defined [target]_ccFlags] then addlFlags += [[target]_ccFlags];

  [cc] [addlFlags] -I[dirname %0%.cpp] [ccInclude] [ccFlags] -c %0%.cpp -E > [target];
}

/*===========================================================================
 */
lib/%1.lua: [[target]_dep]
{
  loop local srcFile = [sort [[target]_dep]] {
    echo "lua: install " [srcFile] " to " [target] set silent;
    tools/bin/cpScript [cktVer] [srcFile] [target] set silent;
  }
}


/*===========================================================================
 * create a library from object files
 *   example: lib/libsbll.a: lib/sbll_obj
 */
lib/lib%1.a: [[target]_obj]
  set mkdir unlink
{
  [ar] cq [target] [[target]_obj];
}

lib/%0%.dylib: [[target]_obj]
  set mkdir unlink
{
  /*[cc] -dynamiclib [[target]_ldFlags] -o [target] [[target]_obj];*/
  echo [[target]_obj] set silent;
  echo fromto %0%.o %0%.c [[target]_obj] set silent;
  echo [fromto %0%.o %0%.c [[target]_obj]] set silent;
  clang -dynamiclib -fvisibility\=hidden [fromto %0%.o %0%.c [[target]_obj]] -o [target];
}

lib/%0%.so: [[target]_obj]
  set mkdir unlink
{
  /*[cc] [[target]_ldFlags] -shared -Wl,-soname,[target] -o [target] [[target]_obj];*/
  [cc] [[target]_ldFlags] -shared                      -o [target] [[target]_obj];
}

/*===========================================================================
 */
bin/scripts: [[target]_dep]
{
  loop local srcFile = [sort [[target]_dep]] {
    function print "script: install" [srcFile];
    local tgtFile = [fromto %0%.sh %0% [fromto src/%1/%2 bin/%2 [srcFile]]];
    function print "script: install" [tgtFile];
    [cpshl] [cktVer] [srcFile] [tgtFile];
  }
}

/*===========================================================================
 * create an executable from object files and special libraries
 *   example: bin/sbll: bin/sbll_dep
 */
bin/%: [[target]_dep]
  set unlink
{
  [cc] -o [target] [[target]_obj] [[target]_ldFlags] [all_libs];
}
tests/bin/%: [[target]_dep]
  set unlink
{
  [cc] -o [target] [[target]_obj] [[target]_ldFlags] [all_libs] -L/usr/local/scripts/local/lib/ -lCuTest;
}

/*===========================================================================
 */
sql/%1.sql: [[target]_dep]
{
  loop local srcFile = [sort [[target]_dep]] {
    tools/bin/cpScript [cktVer] [srcFile] [target] set silent;
  }
}


/*===========================================================================
 * include dependency files created above
 */
#include-cooked-nowarn [cktCookedInc]
