#include "Human.hpp"

const std::string	Human::identify() const
{
    return (brain.identify());
}

const Brain&		Human::getBrain() const
{
    const Brain& ref=brain;

    return (ref);
}