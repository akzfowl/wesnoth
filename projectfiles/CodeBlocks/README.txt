Compiling Wesnoth on Windows using CodeBlocks
---------------------------------------------

(Last tested using Wesnoth 1.11.7+ on Code::Blocks 12.11.)

1.  Get a Wesnoth source tarball or Git repository clone. The folder which
    contains the data/, projectfiles/, and src/ subfolders is referred to as
    <wesnoth_root> in this file.

2.  Install CodeBlocks from <http://www.codeblocks.org/>.
    MinGW is not needed.

3.  Install tdm-gcc-4.5.2 from <http://sourceforge.net/projects/tdm-gcc/files/TDM-GCC Installer/Previous/1.1006.0/>.
    Note that the project files in <wesnoth_root>/projectfiles/CodeBlocks/ may
    contain a setting to compile with OpenMP support, so you should make sure
    that this option is enabled while installing the compiler (mark the
    checkbox for this when choosing components to install).

    NOTE: The newest version of tdm-gcc will NOT work; you need the
    aforementioned one. You must also make sure to download the 32-bit version
    (tdm-gcc-4.5.2.exe), NOT the 64-bit version (tdm64-gcc-4.5.2.exe). Building
    a 64-bit Wesnoth executable on Windows is currently not supported and will
    fail with the SDK package provided in the next step.

4.  Download the latest CodeBlocksWinSDK*.zip package from <http://sourceforge.net/projects/wesnoth/files/unofficial/Windows%20Compile%20Stuff/>.
    The package contains the right version/build combination of source headers,
    build-time libraries (*.a) and run-time libraries (*.dll) needed to build
    and run Wesnoth. Older versions of the package may no longer be useful
    after new dependencies are added to Wesnoth or its version requirements
    change.

    Unpack the file to any path of your choice, which will be referred to as
    <sdk_root> for the remainder of this file.

    The exact names of the folders containing the required files may vary; take
    note of them for the next steps.

5.  In CodeBlocks, open <wesnoth_root>/projectfiles/CodeBlocks/wesnoth.workspace.

    NOTE: The first time CodeBlocks is opened you will be asked to select a
    compiler. If installation from step 3 is complete it may detect it for you,
    in which case you can select the GNU GCC compiler which will produce some
    default selections for step 6 - be sure to change any that don't match as
    directed below.

6.  Go to the Settings -> Compiler option in the menu, and choose the
    Global compiler settings -> Toolchain executables tab in the settings
    dialog. Enter the following settings into the text boxes:

    * Compiler's installation directory: the path to which you installed
      tdm-gcc-4.5.2 (click on ... to browse for it).

    * C compiler, C++ compiler, Linker for dynamic libs: g++.exe

    * Linker for static libs: ar.exe

7.  Change to the Search directories -> Compiler tab and choose Add; enter the
    path to <sdk_root>/include_tdm_gcc/.

8.  Change to the Search directories -> Linker tab and choose Add; enter the
    path to <sdk_root>/lib_tdm_gcc/.

9.  OPTIONAL: By default, CodeBlocks will only run one compiler instance at a
    time, making the overall build process very slow even with fast hardware.
    If you have a multi-core processor, you may make better use of its power by
    increasing the value of the option "Number of processes for parallel
    builds" in the Build options tab. It is recommended to set this to the
    number of CPU cores your system has.

10. Close the settings dialog.

11. Choose the Build -> Build workspace option in the CodeBlocks menu. Once
    finished, wesnoth.exe and wesnothd.exe should appear in <wesnoth_root>.

12. To be able to run your build, copy all *.dll files from the <sdk_root>/dll/
    folder to <wesnoth_root> where the *.exe files are.
