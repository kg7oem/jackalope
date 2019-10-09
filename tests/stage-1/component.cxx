// Jackalope Audio Engine
// Copyright 2019 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#include <jackalope/component.h>

#include "tests.h"

using namespace jackalope;

static void type_parsing()
{
    test_case(component::extract_component_name("foo[bar]") == "foo");
    test_case(component::extract_component_extra("blah[baz]") == "baz");
}

int main(void)
{
    start_testing(0);

    run_test(type_parsing);
}
