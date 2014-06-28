#
# The MIT License
# Copyright (c) 2007-2010 Kimmo Varis
# Copyright (c) 2008 Matthias Mayer
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# $Id: create_release.py 7407 2010-11-03 17:50:26Z kimmov $

# This is a script for creating a WinMerge release.
# Tasks it does:
# - cleans previous build files from folders
# - sets version number for resources
# - updates POT and PO files
# - builds libraries (expat, scew, pcre)
# - builds WinMerge.exe and WinMergeU.exe
# - builds 32-bit and 64-bit ShellExtension targets
# - builds user manual
# - builds the InnoSetup installer
# - creates per-version distribution folder
# - exports SVN sources to distribution folder
# - creates binary distribution folder

#Tasks not done (TODO?):
# - creating packages from source and binary folders
# - running virus check
# - creating SHA-1 hashes for distributed files
# - create installer to correct folder
# - make installer compile less verbose
# - make building exes and dlls also less verbose
# - builds libraries twice: as independent project and from executable project (by prelink.bat)
# - check Python version
# - check JRE is installed
# - check InnoSetup is installed

# Tools needed:
# - Visual Studio 2003 or later
# - Python 2.5 or 2.6 :)
# - Subversion command line binaries from http://subversion.tigris.org/
# - InnoSetup 5
# - Manual build tools (in Developer tools at downloads)
# - Java runtime environment (JRE) (Manual build tools need it)
# There are lots of other dependencies, they are documented in /Docs/Developers documents. Especially be sure to read:
# - Compiling.html for compiling executables and dlls
# - readme-manual.html for building the manual
# - readme-InnoSetup.html for creating the installer

# Please note that this script is only tested in kimmov's environment. It simply may not work in other environments and
# configurations. If you find problems, please report them so we can improve the script.

# CONFIGURATION:
# Set these variables to match your environment and folders you want to use

# Relative path where to create a release folder
dist_root_folder = 'distrib'
root_path = ''
prog_ver = ''

# Build 64-bit targets? This requires 64-bit cross-compile cabable VS version.
# If disabled, 64-bit versions must be compiled before running this script
build_64bit = True

# END CONFIGURATION - you don't need to edit anything below...

from subprocess import *
import os
import os.path
import sys
import optparse
import shutil
import SetVersions
import ToolSettings
import UpgradeProjects

# global settings class instance
prog = ToolSettings.ToolSettings()

def get_vs_ide_bin():
    """Gets a full path to the Visual Studio IDE executable to run."""

    # These are identical for VS2003.Net, VS2005 and VS2008
    rel_path = 'Common7/IDE'
    vs_bin = 'devenv.com'

    vs_ide_path = os.path.join(prog.vs_path, rel_path)
    vs_cmd_path = os.path.join(vs_ide_path, vs_bin)
    return vs_cmd_path

def cleanup_build():
    """Deletes all build files around folder structure"""

    print 'Delete old build files...'
    winmerge_temp = 'BuildTmp'
    if os.path.exists(winmerge_temp):
        print 'Remove folder %s' % winmerge_temp
        shutil.rmtree(winmerge_temp, True)
    else:
        print 'Skipping folder %s' % winmerge_temp
    
    try:
        print 'Remove Unicode files'
        if os.path.exists('build/mergeunicoderelease/WinMergeU.exe'):
            os.remove('build/mergeunicoderelease/WinMergeU.exe')
        if os.path.exists('build/mergeunicoderelease/MergeLang.dll'):
            os.remove('build/mergeunicoderelease/MergeLang.dll')

        print 'Remove ShellExtension files'
        if os.path.exists('build/ShellExtension'):
            shutil.rmtree('build/ShellExtension', True)

        print 'Remove expat files'
        if os.path.exists('build/expat'):
            shutil.rmtree('build/expat', True)
        if os.path.exists('build/mergeunicoderelease/libexpat.dll'):
            os.remove('build/mergeunicoderelease/libexpat.dll')

        print 'Remove pcre files'
        if os.path.exists('build/pcre'):
            shutil.rmtree('build/pcre', True)
        if os.path.exists('build/mergeunicoderelease/pcre.dll'):
            os.remove('build/mergeunicoderelease/pcre.dll')

        if os.path.exists('build/scew'):
            shutil.rmtree('build/scew', True)

        if os.path.exists('build/Manual'):
            shutil.rmtree('build/Manual', True)

    except EnvironmentError, einst:
        print 'Error deleting files: '
        print einst
        return False;
    except:
        print 'Error deleting files: '
        print sys.exc_info()[0]
        return False
    return True

def get_product_version(file):
    """Get the product version number from config file."""

    version = SetVersions.get_product_version(file)
    return version

def set_resource_version(file):
    """Sets the version number to the resource."""

    print 'Update version number to resource(s)...'
    SetVersions.process_versions(file)

def setup_translations():
    """Updates translation files by running scripts in Src/Languages."""

    # Scripts must be run from the directory where they reside
    curdir = os.getcwd()
    os.chdir('Src/Languages')
    retval = call(['cscript', '/nologo', 'CreateMasterPotFile.vbs'])
    if retval == 0:
        retval = call(['cscript', '/nologo', 'UpdatePoFilesFromPotFile.vbs'])
    os.chdir(curdir)

    if retval == 0:
        return True
    else:
        print 'ERROR: Updating translations failed!'
        return False

def get_and_create_dist_folder(folder):
    """Formats a folder name for version-specific distribution folder
    and creates the folder."""

    abs_folder = os.path.realpath(dist_root_folder)
    dist_folder = os.path.join(abs_folder, folder)
    if os.path.exists(dist_folder):
        print 'Folder: ' + dist_folder + ' already exists!'
        print 'If you want to re-create this version, remove folder first!'
        return ''
    else:
        print 'Create distribution folder: ' + dist_folder
        os.mkdir(dist_folder)
        return dist_folder

def get_src_dist_folder(dist_folder, folder):
    """Format a source distribution folder path."""

    dist_src = os.path.join(dist_folder, folder + '-src')
    return dist_src

def svn_export(dist_src_folder):
    """Exports sources to distribution folder."""

    print 'Exporting sources to ' + dist_src_folder
    print 'Exporting from: ' + prog.source
    retval = 0
    if prog.source == 'workspace':
        retval = call([prog.svn_binary, 'export', '--non-interactive', '.', dist_src_folder])
    else:
        retval = call([prog.svn_binary, 'export', '--non-interactive', source_location, dist_src_folder])
    if retval == 0:
        return True
    else:
        print 'Error exporting sources! SVN return value: ' + retval
        return False

def cleanup_dlls_from_plugins(dist_src_folder):
    """Remove compiled plugin dll files from source distribution folders."""

    dll_folder = os.path.join(dist_src_folder, 'Plugins/dlls')
    files = os.listdir(dll_folder)

    print 'Removing dll files from plugin folder...'
    for cur_file in files:
        fullpath = os.path.join(dll_folder, cur_file)
        if os.path.isfile(fullpath):
            file_name, file_ext = os.path.splitext(cur_file)
            if (file_ext == '.dll'):
                os.remove(fullpath)

def build_libraries():
    """Builds library targets: expat, scew and pcre."""

    vs_cmd = get_vs_ide_bin()
    cur_path = os.getcwd()

    print 'Build expat library...'
    solution_path = os.path.join(cur_path, 'Externals/expat/lib/expat.vcproj')
    #print solution_path
    call([vs_cmd, solution_path, '/rebuild', 'Release'], shell=True)

    print 'Build SCEW library...'
    solution_path = os.path.join(cur_path, 'Externals/scew/win32/scew.vcproj')
    #print solution_path
    call([vs_cmd, solution_path, '/rebuild', 'Release'], shell=True)

    print 'Build pcre library...'
    solution_path = os.path.join(cur_path, 'Externals/pcre/Win32/pcre.vcproj')
    #print solution_path
    call([vs_cmd, solution_path, '/rebuild', 'MinSizeRel'], shell=True)

def build_targets():
    """Builds all WinMerge targets."""

    build_libraries()
    vs_cmd = get_vs_ide_bin()
    ret = build_winmerge(vs_cmd)
    if ret:
        ret = build_shellext(vs_cmd)
    return ret

def build_winmerge(vs_cmd):
    """Builds WinMerge executable targets."""

    cur_path = os.getcwd()
    solution_path = os.path.join(cur_path, 'Src\\Merge.vcproj')
    #print sol_path

    # devenv Src\Merge.dsp /rebuild Release
    print 'Build WinMerge executable...'
    ret = call([vs_cmd, solution_path, '/rebuild', 'UnicodeRelease'], shell = True)
    if ret == 0:
        return True
    else:
        print 'ERROR: Failed to build Unicode release target of WinMerge!'
        return False

def build_shellext(vs_cmd):
    '''Builds 32-bit and 64-bit ShellExtension.
    
    64-bit ShellExtension is build only if build_64bit is enabled in config.
    '''

    cur_path = os.getcwd()
    solution_path = os.path.join(cur_path, 'ShellExtension\\ShellExtension.vcproj')

    # devenv Src\Merge.dsp /rebuild Release
    print 'Build ShellExtension dlls...'
    ret = call([vs_cmd, solution_path, '/rebuild', 'Unicode Release MinDependency|Win32'])
    if ret == 0:
        if build_64bit == True:
            ret = call([vs_cmd, solution_path, '/rebuild', 'X64 Release|x64'])
        else:
            return True
    else:
        print 'ERROR: Failed to build Unicode target of ShellExtension!'
        return False

    if ret == 0:
        return True
    else:
        print 'ERROR: Failed to build 64-bit target of ShellExtension!'
        return False

def build_manual():
    """Builds manual's HTML Help (CHM) version for user install and
    HTML version for the Web. HTML version is created with ads."""

    curdir = os.getcwd()
    os.chdir('Docs/Users/Manual/build')
    print 'Build HTML Help (CHM) manual...' 
    call(['build_htmlhelp.bat'])

    # HTML manual not build in trunk.
    #print 'Build HTML manual for Web with ads...'
    #call(['build_html.bat', 'withads'])
    print 'Manual build finished.'
    os.chdir(curdir)

def build_innosetup_installer(target_folder):
    """Builds the InnoSetup installer for the WinMerge."""

    innosetup_exe = os.path.join(prog.innosetup_path, 'iscc.exe')
    cur_path = os.getcwd()
    os.chdir('Installer/InnoSetup')

    #output_switch = '/O"' + target_folder + '"'

    print 'Build Innosetup installer...'
    # Should be able to give folder for created file and Q switch to make build quiet
    #call([innosetup_exe, '/Q', output_switch, winmerge_iss])
    call([innosetup_exe, 'WinMerge.iss'])
    os.chdir(cur_path)

def get_and_create_bin_folder(dist_folder, folder):
    """Formats and creates binary distribution folder."""

    bin_folder = os.path.join(dist_folder, folder + '-exe')
    print 'Create binary distribution folder: ' + bin_folder
    os.mkdir(bin_folder)
    return bin_folder

def create_bin_folders(bin_folder, dist_src_folder):
    """Creates binary distribution folders."""

    cur_path = os.getcwd()
    os.chdir(bin_folder)
    print 'Create binary distribution folder structure...'
    lang_folder = os.path.join(bin_folder, 'Languages')
    os.mkdir(lang_folder)
    doc_folder = os.path.join(bin_folder, 'Docs')
    os.mkdir(doc_folder)
    filters_folder = os.path.join(bin_folder, 'Filters')
    plugins_folder = os.path.join(bin_folder, 'MergePlugins')
    os.chdir(cur_path)

    print 'Copying files to binary distribution folder...'
    shutil.copy('build/mergeunicoderelease/WinMergeU.exe', bin_folder)

    shutil.copy('build/ShellExtension/unicode release mindependency/ShellExtensionU.dll', bin_folder)
    shutil.copy('build/mergeunicoderelease/MergeLang.dll', bin_folder)
    shutil.copy('build/ShellExtension/x64 release/ShellExtensionX64.dll', bin_folder)
    shutil.copy('ShellExtension/Register.bat', bin_folder)
    shutil.copy('ShellExtension/UnRegister.bat', bin_folder)

    shutil.copy('build/pcre/pcre.dll', bin_folder)
    shutil.copy('build/expat/libexpat.dll', bin_folder)

    copy_po_files(lang_folder)
    filter_orig = os.path.join(dist_src_folder, 'Filters')
    shutil.copytree(filter_orig, filters_folder, False)

    # Copy compiled plugins dir and rename it
    plugin_dir = os.path.join(bin_folder, 'dlls')
    plugin_orig = os.path.join(dist_src_folder, 'Plugins/dlls')
    shutil.copytree(plugin_orig, plugin_dir, False)
    os.rename(plugin_dir, plugins_folder)

    shutil.copy('build/Manual/htmlhelp/WinMerge.chm', doc_folder)

    shutil.copy('Docs/Users/ReleaseNotes.html', doc_folder)
    shutil.copy('Docs/Users/ReadMe.txt', bin_folder)
    shutil.copy('Docs/Users/ChangeLog.txt', doc_folder)
    shutil.copy('Docs/Users/Contributors.txt', bin_folder)
    shutil.copy('Docs/Users/Files.txt', bin_folder)

def copy_po_files(dest_folder):
    """Copies all PO files to destination folder."""

    lang_folder = 'Src/Languages'
    files = os.listdir(lang_folder)

    print 'Copying PO files to binary folder...'
    for cur_file in files:
        fullpath = os.path.join(lang_folder, cur_file)
        if os.path.isfile(fullpath):
            file_name, file_ext = os.path.splitext(cur_file)
            if (file_ext == '.po'):
                shutil.copy(fullpath, dest_folder)

def find_winmerge_root():
    """Find WinMerge tree root folder from where to run rest of the script.

    This function checks if we are in WinMerge root folder. If we are in some
    other folder then we must try to find the WinMerge root folder. Because all
    other code assumes we are in WinMerge root folder. If the root folder is
    found current folder is changed into it."""

    # If we find Src and Filters -subfolders we are in root 
    if os.path.exists('Src') and os.path.exists('Filters'):
        return True
    
    # Check if we are in /Tools/Scripts
    if os.path.exists('../../Src') and os.path.exists('../../Filters'):
        os.chdir('../../')
        return True
    
    return False

def check_tools():
    """Check that needed external tools can be found."""

    global prog
    if not os.path.exists(prog.svn_binary):
        print 'Subversion binary could not be found from:'
        print prog.svn_binary
        print 'Please check script configuration and/or make sure Subversion is installed.'
        return False

    vs_cmd = get_vs_ide_bin()
    if not os.path.exists(vs_cmd):
        print 'Cannot find Visual Studio IDE binary from:'
        print vs_cmd
        print 'Please check script configuration.'
        return False
    
    innosetup_exe = os.path.join(prog.innosetup_path, 'iscc.exe')
    if not os.path.exists(innosetup_exe):
        print 'InnoSetup executable could not be found from:'
        print innosetup_exe
        print 'Please check script configuration.'
        return False

    pathhhc = os.path.join(root_path, 'Docs/Users/Manual/build/hhc/hhc.exe')
    folderdtd = os.path.join(root_path, 'Docs/Users/Manual/build/dtd')
    foldersaxon = os.path.join(root_path, 'Docs/Users/Manual/build/saxon')
    folderxerc = os.path.join(root_path, 'Docs/Users/Manual/build/xerces')
    folderxsl = os.path.join(root_path, 'Docs/Users/Manual/build/xsl')

    if not os.path.exists(pathhhc) or not os.path.exists(folderdtd) or \
            not os.path.exists(foldersaxon) or not os.path.exists(folderxerc) or \
            not os.path.exists(folderxsl):
        print 'Cannot find manual build tools'
        print 'Please download and install manual build tools from:'
        print 'https://sourceforge.net/project/showfiles.php?group_id=13216'
        print 'See also Docs/Developers/readme-manual.html'
        return False
    return True

def check_x64shellext():
    '''Checks that 64-bit ShellExtension is compiled prior to running this
    script.

    If we haven't enabled building 64-bit targets we must ensure the 64-bit
    ShellExtension is already compiled.
    '''

    if not os.path.exists('build/shellextension/x64 release/ShellExtensionX64.dll'):
        print 'ERROR: cannot create a release:'
        print 'You must compile 64-bit ShellExtension (ShellExtensionX64.dll)'
        print 'before running this script!'
        return False
    else:
        return True

def update_project_files(root_path):
    '''Upgrade project/solution files for the VS version used.'''

    print 'Update VS project/solution files for used VS version...'
    UpgradeProjects.tools.read_ini('Tools.ini')
    UpgradeProjects.upgrade_projects(root_path)

def main(argv):
    global prog
    ver_file = ''
    
    parser = optparse.OptionParser()
    parser.add_option('--version', '-v', action = 'store',
            help = 'Set global version number to use.')
    parser.add_option('--cleanup', '-c', action = 'store_true',
            help = 'Clean up build files (temp files, libraries, executables).')
    parser.add_option('--libraries', '-l', action = 'store_true',
            help = 'Build libraries (hekxedit, rawio32) only.')
    parser.add_option('--file', '-f', action = 'store',
            help = 'Set the version number ini file.')
    options, arguments = parser.parse_args()

    if options.version:
        prog_version = options.version
        print 'Start building WinMerge release version ' + prog_version

    if options.cleanup:
        if cleanup_build() == True:
            print 'Cleanup done.'
        sys.exit()

    if options.libraries:
        build_libraries()
        sys.exit()

    if options.file:
        ver_file = options.file

    if ver_file == '' and prog_version == '':
        print 'WARNING: No version number or INI file given, using default'
        print '    version number of 0.0.0.0 where applicable in this script.'
        prog_version = '0.0.0.0'

    # Check we are running from correct folder (and go to root if found)
    if find_winmerge_root() == False:
        print 'ERROR: Cannot find WinMerge root folder!'
        print 'The script must be run from WinMerge tree\'s root folder'
        print '(which has Src- and Filter -folders as subfolders) or from'
        print 'Tools/Scripts -folder (where this script is located).'
        sys.exit()

    # Now read settings from Tools.ini
    prog.read_ini('Tools.ini')
    print 'Compiler: ' + prog.vs_path
    print 'Path: ' + os.getcwd()

    # Remember the rootfolder
    root_path = os.getcwd()

    # Check all required tools are found (script configuration)
    if check_tools() == False:
        sys.exit()

    # Check 64-bit ShellExtension is compiled if not building 64-bit
    if build_64bit == False and check_x64shellext() == False:
        sys.exit()

    # Create the distribution folder if it doesn't exist
    try:
        if not os.path.exists(dist_root_folder):
            os.mkdir(dist_root_folder)
    except EnvironmentError, einst:
        print 'Error creating distribution folder: ' + dist_root_folder
        print einst
        sys.exit()

    # Remove old build's files
    if cleanup_build() == False:
        sys.exit()

    if len(ver_file) > 0:
        version_read = get_product_version(ver_file)
        if len(version_read) > 0:
            prog_version = version_read
        set_resource_version(ver_file)

    version_folder = 'WinMerge-' + prog_version
    dist_folder = get_and_create_dist_folder(version_folder)
    if dist_folder == '':
        sys.exit(1)
    dist_src_folder = get_src_dist_folder(dist_folder, version_folder)
    if svn_export(dist_src_folder) == False:
        sys.exit(1)

    if setup_translations() == False:
        sys.exit(1)

    update_project_files(root_path)

    if build_targets() == False:
        sys.exit(1)
    build_manual()
    build_innosetup_installer(dist_folder)

    dist_bin_folder = get_and_create_bin_folder(dist_folder, version_folder)
    create_bin_folders(dist_bin_folder, dist_src_folder)

    # Do the cleanup after creating binary distrib folders, as some files
    # and folders are copied from source folders to binary folders.
    cleanup_dlls_from_plugins(dist_src_folder)

    print 'WinMerge release script ready!'


### MAIN ###
if __name__ == "__main__":
    main(sys.argv[1:])
