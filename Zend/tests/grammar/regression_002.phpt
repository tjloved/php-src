--TEST--
Test to ensure ::class still works
--FILE--
<?php

class Foo
{
}
function fn2081157628()
{
    var_dump(Foo::class);
    var_dump(Foo::class);
    var_dump(Foo::CLASS);
    var_dump(Foo::CLASS);
}
fn2081157628();
--EXPECTF--
string(3) "Foo"
string(3) "Foo"
string(3) "Foo"
string(3) "Foo"
