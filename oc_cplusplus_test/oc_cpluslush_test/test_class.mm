//
//  test_class.cpp
//  oc_cpluslush_test
//
//  Created by davad.di on 11/27/15.
//  Copyright © 2015 davad.di. All rights reserved.
//

#include "test_class.hpp"

CPlusPlusClass::CPlusPlusClass() : m_i(0) 
{
    printf("CPlusPlusClass::CPlusPlusClass()\n");
    func();
}

CPlusPlusClass::~CPlusPlusClass() 
{
    printf("CPlusPlusClass::~CPlusPlusClass()\n");
}

void CPlusPlusClass::func() {
    printf("CPlusPlusClass func print: %d\n", m_i);
}

void CPlusPlusClass::print(){
    printf("Hello from c++ class");
}

