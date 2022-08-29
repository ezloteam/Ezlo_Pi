function Component()
{
    // default constructor
}


Component.prototype.createOperations = function()
{
    try {
        // call the base create operations function
        component.createOperations();
//        if (installer.value("os") == "win") {
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
