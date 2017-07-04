--TEST--
Testing dynamic calls
--FILE--
<?php

class foo
{
    public static function a()
    {
        print "ok\n";
    }
}
function fn91370058()
{
    $a = 'a';
    $b = 'a';
    $class = 'foo';
    foo::a();
    foo::$a();
    foo::${$b}();
    $class::a();
    $class::$a();
    $class::${$b}();
}
fn91370058();
--EXPECT--
ok
ok
ok
ok
ok
ok
