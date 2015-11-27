//
//  main.m
//  oc_cpluslush_test
//
//  Created by davad.di on 11/27/15.
//  Copyright © 2015 davad.di. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "oc_adapter.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        ObjectiveCAdaptor *testObjectiveCObj = [[ObjectiveCAdaptor alloc] init];
        [testObjectiveCObj objectiveFunc];
        
        // 调用此方法需要关闭oc的对象引用计数功能
        // xcode中，打开“Build Setting”,
        // "Language Object-C"
        // 找到“Objective-C Automatic Reference Counting”项 -> NO
        // http://blog.csdn.net/x32sky/article/details/7496901
        // http://philjordan.eu/article/strategies-for-using-c++-in-objective-c-projects
        [testObjectiveCObj release];
        
        NSLog(@"Hello, World!");
    }
    return 0;
}
