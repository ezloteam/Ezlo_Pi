function Component()
{
    if (systemInfo.productType === "windows") { 
        Component.prototype.installVCRedist(); 
    }
}


Component.prototype.createOperations = function()
{
    try {
        // call the base create operations function
        component.createOperations();
        if (systemInfo.productType === "windows") {
            try {
                var userProfile = installer.environmentVariable("USERPROFILE");
                installer.setValue("UserProfile", userProfile);
                    component.addOperation("CreateShortcut",
                                       "@TargetDir@/ezlogic.exe",                        // target
                                       "@DesktopDir@/EzloPi.lnk",                       // link-path
                                       "workingDirectory=@TargetDir@",                   // working-dir
                                       "iconPath=@TargetDir@/ezlogic.exe", "iconId=0",   // icon
                                       "description=Start App");                         // description
                    component.addOperation("CreateShortcut",
                                           "@TargetDir@/ezlogic.exe",                        // target
                                           "@StartMenuDir@/EzloPi.lnk",                     // link-path
                                           "workingDirectory=@TargetDir@",                   // working-dir
                                           "iconPath=@TargetDir@/ezlogic.exe", "iconId=0",   // icon
                                           "description=Start App");                         // description
            } catch (e) {
                // Do nothing if key doesn't exist
            }
        }
    } catch (e) {
        print(e);
    }
}

Component.prototype.installVCRedist = function()
{
    var registryVC2015_22x64 = installer.execute("reg", new Array("QUERY", "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\x64", "/v", "Installed"))[0];
    var doInstall = false;
	console.log("Registry output." + registryVC2015_22x64);
    if (!registryVC2015_22x64) {
        doInstall = true;
    }
	/*
    else
    {
        var bld = installer.execute("reg", new Array("QUERY", "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\x64", "/v", "Bld"))[0];

        var elements = bld.split(" ");

        bld = parseInt(elements[elements.length-1]);
        if (bld < 31332)
        {
            doInstall = true;
        }
    }
	*/
    if (doInstall)
    {
        QMessageBox.information("vcRedist.install", "Install VS Redistributables", "The application requires Visual Studio 2015-2022 Redistributables. Please follow the steps to install it now.", QMessageBox.OK);
        var dir = installer.value("TargetDir");
        installer.execute(dir + "/VC_redist.x64.exe"); // "/norestart", "/active");
    }
}