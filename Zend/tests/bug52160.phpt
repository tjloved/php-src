--TEST--
Bug #52160 (Invalid E_STRICT redefined constructor error)
--FILE--
<?php

class bar
{
    function __construct()
    {
    }
    static function bar()
    {
        var_dump(1);
    }
}
class foo
{
    static function foo()
    {
        var_dump(2);
    }
    function __construct()
    {
    }
}
class baz
{
    static function baz()
    {
        var_dump(3);
    }
}
function fn1616921364()
{
    bar::bar();
    foo::foo();
}
fn1616921364();
--EXPECTF--
Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; baz has a deprecated constructor in %s on line %d

Fatal error: Constructor baz::baz() cannot be static in %s on line %d
