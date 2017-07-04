--TEST--
Testing callback formats within class method
--FILE--
<?php

class foo
{
    public function test()
    {
        call_user_func(array('FOO', 'ABC'));
        call_user_func(array($this, 'ABC'));
        foo::XYZ();
        self::WWW();
        call_user_func('FOO::ABC');
    }
    function __call($a, $b)
    {
        print "__call:\n";
        var_dump($a);
    }
    public static function __callStatic($a, $b)
    {
        print "__callstatic:\n";
        var_dump($a);
    }
}
function fn38705361()
{
    $x = new foo();
    $x->test();
    $x::A();
    foo::B();
    $f = 'FOO';
    $f::C();
    $f::$f();
    foo::$f();
}
fn38705361();
--EXPECT--
__call:
string(3) "ABC"
__call:
string(3) "ABC"
__call:
string(3) "XYZ"
__call:
string(3) "WWW"
__call:
string(3) "ABC"
__callstatic:
string(1) "A"
__callstatic:
string(1) "B"
__callstatic:
string(1) "C"
__callstatic:
string(3) "FOO"
__callstatic:
string(3) "FOO"