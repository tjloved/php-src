--TEST--
028: Name ambiguity (class name & external namespace name)
--FILE--
<?php

class Foo
{
    function __construct()
    {
        echo "Method - " . __CLASS__ . "::" . __FUNCTION__ . "\n";
    }
    static function Bar()
    {
        echo "Method - " . __CLASS__ . "::" . __FUNCTION__ . "\n";
    }
}
function fn142725562()
{
    require "ns_028.inc";
    $x = new Foo();
    Foo\Bar();
    $x = new Foo\Foo();
    Foo\Foo::Bar();
    \Foo\Bar();
}
fn142725562();
--EXPECT--
Method - Foo::__construct
Func   - Foo\Bar
Method - Foo\Foo::__construct
Method - Foo\Foo::Bar
Func   - Foo\Bar
