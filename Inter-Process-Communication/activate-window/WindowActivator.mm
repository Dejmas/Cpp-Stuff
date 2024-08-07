
// WindowActivator.mm
#import <Cocoa/Cocoa.h>

bool activateWindowByProcessID(pid_t processID) {
    NSRunningApplication *app = [NSRunningApplication runningApplicationWithProcessIdentifier:processID];
    if (app) {
        [app activateWithOptions:NSApplicationActivateAllWindows];
        return true;
    } else {
        return false;
    }
}
