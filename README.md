HansoftSelectViewPresetClientPlugin
===================================

About this program
------------------
This program is a Hansoft SDK client plugin that adds functionality to the Hansoft client. Note that to use this
program you need to have the SDK option enabled on your Hansoft Server.

This plugin adds a menu item to the right-click menu in Hansoft called "Select view preset". Under this menu there
will be submenu-items for each of the view presets that potentially are applicable for the user. When selecting
a view preset from the menu this preset will immediately be applied for the logged in user.

Presets are created in the normal way by a Hansoft Main Manager. By setting the "Affecting users" option in the
preset you also control for which users the preset should be added as a menu selection by this plugin.

Terms and conditions
--------------------
HansoftSelectViewPresetClientPlugin is licensed under what is known as an MIT License
as stated in the [LICENSE.md](LICENSE.md).

This program is not part of the official Hansoft product or subject to its license agreement.
The program is provided as is and there is no obligation on Hansoft AB to provide support, update or enhance this program.

Building the program
--------------------
The program can be built with the freely available [Visual Studio Express 2012 for Desktop] [1]. 
You also need the [Hansoft SDK] [2] to be able to build the program. You will also need to
update the references to the  Hansoft SDK in the Visual Studio project.

[1]: http://www.microsoft.com/visualstudio/eng/products/visual-studio-express-for-windows-desktop  "Visual Studio Express 2012 for Desktop"
[2]: http://hansoft.com/support/downloads/                                                         "Hansoft SDK"

Installation
------------
Install the client plugin with the [HansoftClientPluginInstaller] [1].
[1]: http://github.com/Hansoft/HansoftClientPluginInstaller  "HansoftClientPluginInstaller"







































































