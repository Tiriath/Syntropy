
/// \file unit1.h
/// \brief TODO: Add brief description here
///
/// \author Raffaele D. Facendola - 2016

#pragma once

#include "syntropy.h"
#include "meta_class.h"

#include "unit2.h"

#include "any.h"

#include <tuple>
#include <iostream>

#define TEST_TRUE(test) \
std::cout << ((test) ? "PASSED - " : "NOT PASSED - ") << #test << std::endl;

#define TEST_FALSE(test) \
std::cout << (!(test) ? "PASSED - " : "NOT PASSED - ") << #test << std::endl;


struct Blob{

    int blob_;

};

class Foo : public Bar {

    friend class syntropy::MetaClassDefinition<Foo>;

public:

    Foo()
        : const_value_(666)
        , const_pointer_(nullptr){}

    Foo(const Foo& other)
        : value_(other.value_)
        , const_value_(other.const_value_)
        , pointer_(other.pointer_)
        , pointer_to_const_(other.pointer_to_const_)
        , const_pointer_(other.const_pointer_)
        , blob_(other.blob_) {

        std::cout << "Copy ctor!" << std::endl;

    }

    float GetValue() const {

        return value_;

    }

    void SetValue(float value) {

        value_ = value;

    }

    float GetConstValue() const {

        return const_value_;

    }
    
    float* GetPointer() const {

        return pointer_;

    }

    void SetPointer(float* pointer) {

        pointer_ = pointer;

    }

    const float* GetPointerToConst() const {

        return pointer_to_const_;

    }

    void SetPointerToConst(const float* pointer_to_const) {

        pointer_to_const_ = pointer_to_const;

    }

    float* const GetConstPointer() const {

        return const_pointer_;

    }

    const Blob& GetBlob() const {

        return blob_;

    }

    void SetBlob(const Blob& blob) {

        blob_ = blob;

    }

    const Blob& GetAccessor() const {

        return blob_;

    }

    Blob& GetAccessor() {

        return blob_;

    }

public:

    float value_;
    const float const_value_;
    float* pointer_;
    const float * pointer_to_const_;
    float* const const_pointer_;

    Blob blob_;


};

template <>
class syntropy::MetaClassDefinition<Bar> : public syntropy::MetaClassDeclaration {

public:

    MetaClassDefinition() :
        MetaClassDeclaration("syntropy::Bar") {}


};

template <>
class syntropy::MetaClassDefinition<Foo> : public syntropy::MetaClassDeclaration {

public:

    MetaClassDefinition() :
        MetaClassDeclaration("syntropy::Foo"){

        DefineBaseClass<Bar>(); 

        DefineProperty("value", &Foo::value_);
        DefineProperty("const_value", &Foo::const_value_);
        DefineProperty("pointer", &Foo::pointer_);
        DefineProperty("pointer_to_const", &Foo::pointer_to_const_);
        DefineProperty("const_pointer", &Foo::const_pointer_);

        DefineProperty("Value", &Foo::GetValue, &Foo::SetValue);
        DefineProperty("ConstValue", &Foo::GetConstValue);
        DefineProperty("Pointer", &Foo::GetPointer, &Foo::SetPointer);
        DefineProperty("PointerToConst", &Foo::GetPointerToConst, &Foo::SetPointerToConst);
        DefineProperty("ConstPointer", &Foo::GetConstPointer);      
        DefineProperty("Blob", &Foo::GetBlob, &Foo::SetBlob);
        DefineProperty("Accessor", &Foo::GetAccessor, &Foo::GetAccessor);

    }


};



class Tester {

public:

    void SynopsisTest() {

        auto& meta_class = syntropy::MetaClass::GetClass<Foo>();

        std::cout << "Class '" << meta_class.GetName().GetString() << "'" << std::endl;

        for (const auto& property : meta_class.GetProperties()) {

            std::cout << "Property '" << property.first.GetString() << "': " << property.second.GetType().name() << std::endl;

        }

        std::cout << std::endl;

    }

    void FieldTest() {

        Foo foo;

        auto& meta_class = syntropy::MetaClass::GetClass<Foo>();

        auto value = meta_class.GetProperty("value");
        auto const_value = meta_class.GetProperty("const_value");
        auto pointer = meta_class.GetProperty("pointer");
        auto pointer_to_const = meta_class.GetProperty("pointer_to_const");
        auto const_pointer = meta_class.GetProperty("const_pointer");

        float x = 0;
        float* p = &x;
        const float* q = &x;

        TEST_TRUE(value->Write(foo, 100));
        TEST_TRUE(value->Read(foo, x));

        TEST_FALSE(const_value->Write(foo, x));
        TEST_TRUE(const_value->Read(foo, x));

        TEST_TRUE(pointer->Write(foo, p));
        TEST_TRUE(pointer->Read(foo, p));

        TEST_TRUE(pointer_to_const->Write(foo, q));
        TEST_TRUE(pointer_to_const->Read(foo, q));

        TEST_FALSE(const_pointer->Write(foo, p));
        TEST_TRUE(const_pointer->Read(foo, p));

        std::cout << std::endl;

    }

    void PropertyTest() {

        Foo foo;

        auto& meta_class = syntropy::MetaClass::GetClass<Foo>();

        auto value = meta_class.GetProperty("Value");
        auto const_value = meta_class.GetProperty("ConstValue");
        auto pointer = meta_class.GetProperty("Pointer");
        auto pointer_to_const = meta_class.GetProperty("PointerToConst");
        auto const_pointer = meta_class.GetProperty("ConstPointer");
        auto blob = meta_class.GetProperty("Blob");
        auto accessor = meta_class.GetProperty("Accessor");

        Blob bb;

        float x = 100;
        float* p = &x;
        const float* q = &x;

        const float y(10);

        TEST_TRUE(value->Write(foo, y));
        TEST_TRUE(value->Read(foo, x));

        TEST_FALSE(const_value->Write(foo, y));
        TEST_TRUE(const_value->Read(foo, x));

        TEST_TRUE(pointer->Write(foo, p));
        TEST_TRUE(pointer->Read(foo, p));

        TEST_TRUE(pointer_to_const->Write(foo, q));
        TEST_TRUE(pointer_to_const->Read(foo, q));

        TEST_FALSE(const_pointer->Write(foo, p));
        TEST_TRUE(const_pointer->Read(foo, p));

        TEST_TRUE(blob->Write(foo, bb));
        TEST_TRUE(blob->Read(foo, bb));

        TEST_TRUE(accessor->Write(foo, bb));
        TEST_TRUE(accessor->Read(foo, bb));

        std::cout << std::endl;

    }

    void Do() {

        SynopsisTest();
        FieldTest();
        PropertyTest();

        system("pause");

    }

};
