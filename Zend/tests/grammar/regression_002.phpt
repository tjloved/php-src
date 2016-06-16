--TEST--
Test to ensure ::class still works
--FILE--
<?php

class Foo
{
}
function fn616082767()
{
    var_dump(Foo::class);
    var_dump(Foo::class);
    var_dump(Foo::CLASS);
    var_dump(Foo::CLASS);
}
fn616082767();
--EXPECTF--
string(3) "Foo"
string(3) "Foo"
string(3) "Foo"
string(3) "Foo"
