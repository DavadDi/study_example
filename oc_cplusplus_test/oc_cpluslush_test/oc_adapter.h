//
//  oc_adapter.h
//  oc_cpluslush_test
//
//  Created by davad.di on 11/27/15.
//  Copyright © 2015 davad.di. All rights reserved.
//

#ifndef oc_adapter_h
#define oc_adapter_h

#import <Foundation/Foundation.h>

class CPlusPlusClass;

@interface ObjectiveCAdaptor : NSObject {
@private
    CPlusPlusClass *testObj;
}

- (void) objectiveFunc;
@end

#endif /* oc_adapter_h */
