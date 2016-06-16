--TEST--
Bug #47165 (Possible memory corruption when passing return value by reference)
--FILE--
<?php

class Foo
{
    var $bar = array();
    static function bar()
    {
        static $instance = null;
        $instance = new Foo();
        return $instance->bar;
    }
}
function fn2130944920()
{
    extract(Foo::bar());
    echo "ok\n";
}
fn2130944920();
--EXPECT--
ok
