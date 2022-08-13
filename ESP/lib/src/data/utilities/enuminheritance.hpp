#pragma once
#ifndef ENUMINHERITANCE_HPP
#define ENUMINHERITANCE_HPP
template <typename EnumT, typename BaseEnumT>
class InheritEnum
{
public:
    InheritEnum() {}
    InheritEnum(EnumT e)
        : enum_(e)
    {
    }

    InheritEnum(BaseEnumT e)
        : baseEnum_(e)
    {
    }

    explicit InheritEnum(int val)
        : enum_(static_cast<EnumT>(val))
    {
    }

    operator EnumT() const { return enum_; }

private:
    // Note - the value is declared as a union mainly for a debugging aid. If
    // the union is undesired and you have other methods of debugging, change it
    // to either of EnumT and do a cast for the constructor that accepts BaseEnumT.
    union
    {
        EnumT enum_;
        BaseEnumT baseEnum_;
    };
};
#endif // ENUMINHERITANCE_HPP

/* Example Usage */
//enum Fruit
//{
//    Orange,
//    Mango,
//    Banana
//};
//enum NewFruits
//{
//    Apple,
//    Pear
//};
//typedef InheritEnum<NewFruits, Fruit> MyFruit;
//
//void consume(MyFruit myfruit);
