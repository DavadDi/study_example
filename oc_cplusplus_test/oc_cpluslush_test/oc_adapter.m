//
//  oc_adapter.m
//  oc_cpluslush_test
//
//  Created by davad.di on 11/27/15.
//  Copyright © 2015 davad.di. All rights reserved.
//

#import "oc_adapter.h"
#include "test_class.hpp"

@implementation ObjectiveCAdaptor

- (id) init {
    if (self = [super init]) {
        testObj = new CPlusPlusClass();
    }
    
    return self;
}

- (void) dealloc {
    if (testObj != NULL) {
        delete testObj;
        testObj = NULL;
    }
    
    // 调用此方法需要关闭oc的对象引用计数功能
    // xcode中，打开“Build Setting”,
    // "Language Object-C"
    // 找到“Objective-C Automatic Reference Counting”项 -> NO
    // http://blog.csdn.net/x32sky/article/details/7496901
    [super dealloc];
}

- (void) objectiveFunc
{
    testObj->setInt(5);
    testObj->func();
}

@end
