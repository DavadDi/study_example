//
//  test_class.hpp
//  oc_cpluslush_test
//
//  Created by davad.di on 11/27/15.
//  Copyright © 2015 davad.di. All rights reserved.
//

#ifndef test_class_hpp
#define test_class_hpp

#include <stdio.h>

class CPlusPlusClass {
public:
    CPlusPlusClass();
    virtual ~CPlusPlusClass();
    void func();
    void setInt (int i) {
        m_i = i;
    }
    
    void print();

private:
    int m_i;
};

#endif /* test_class_hpp */
