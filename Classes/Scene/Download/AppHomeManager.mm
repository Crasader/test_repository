#import "Apphomemanager.h"
#import <sys/xattr.h>

using namespace std;

string AppHomeManager::getAppHome()
{    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *documentsPath = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
    NSString *baseDir = [documentsPath stringByAppendingPathComponent:@"/Resources/"]; //追加するディレクトリ名を指定
    NSLog(@"baseDir=%@", baseDir);
    
    //Resourcesを作成
    BOOL exists = [fileManager fileExistsAtPath:baseDir];
    if (!exists) {
        NSError *error;
        BOOL created = [fileManager createDirectoryAtPath:baseDir withIntermediateDirectories:YES attributes:nil error:&error];
        if (!created) {
            NSLog(@"Resourcesディレクトリ作成失敗");
            return "";
        }
        NSLog(@"Resourcesディレクトリ作成成功");
    } else {
        NSLog(@"Resourcesディレクトリ作成済み");
    }
    
    //ResourcesをiCloudバックアップ対象外にする
    const char* attrName = "com.apple.MobileBackup";
    u_int8_t attrValue = 1;
    
    int result = setxattr([baseDir UTF8String], attrName, &attrValue, sizeof(attrValue), 0, 0);
    if (result == 0) {
        NSLog(@"属性マーク設定成功");
    } else {
        NSLog(@"属性マーク設定失敗");
    }
    
    const char* home_dir_char = [documentsPath UTF8String];
    string home_dir = home_dir_char;
    return home_dir;
}

//送られてきたpathのディレクトリをiCloudバックアップ対象外にする
bool AppHomeManager::iCloudTakeOff(std::string path)
{
    NSString *baseDir = [NSString stringWithUTF8String:path.c_str()];
    
    //pathのディレクトリをiCloudバックアップ対象外にする
    const char* attrName = "com.apple.MobileBackup";
    u_int8_t attrValue = 1;
    BOOL i_result = YES;
    
    int result = setxattr([baseDir UTF8String], attrName, &attrValue, sizeof(attrValue), 0, 0);
    if (result == 0) {
        NSLog(@"属性マーク設定成功");
        i_result = YES;
    } else {
        NSLog(@"属性マーク設定失敗");
        i_result = NO;
    }
    
    return i_result;
}